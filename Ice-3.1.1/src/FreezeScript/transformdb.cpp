// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <FreezeScript/Transformer.h>
#include <FreezeScript/TransformAnalyzer.h>
#include <FreezeScript/Exception.h>
#include <FreezeScript/Util.h>
#include <Freeze/Initialize.h>
#include <Freeze/Transaction.h>
#include <Freeze/Catalog.h>
#include <IceUtil/Options.h>
#include <db_cxx.h>
#include <sys/stat.h>
#include <fstream>
#include <algorithm>

using namespace std;

#ifdef _WIN32
#   define FREEZE_SCRIPT_DB_MODE 0
#else
#   define FREEZE_SCRIPT_DB_MODE (S_IRUSR | S_IWUSR)
#endif

static void
usage(const char* n)
{
 
//
// -a is not fully implemented yet
//  cerr << "Usage: " << n << " [options] [dbenv db newdbenv | dbenv newdbenv]\n";
//  
    cerr << "Usage: " << n << " [options] [dbenv db newdbenv] \n";
    cerr <<
        "Options:\n"
        "-h, --help            Show this message.\n"
        "-v, --version         Display the Ice version.\n"
        "-DNAME                Define NAME as 1.\n"
        "-DNAME=DEF            Define NAME as DEF.\n"
        "-UNAME                Remove any definition for NAME.\n"
        "-d, --debug           Print debug messages.\n"
        "--ice                 Permit `Ice' prefix (for building Ice source code only)\n"
        "-o FILE               Output transformation descriptors into the file FILE.\n"
        "                      Database transformation is not performed.\n"
// 
// -a is not fully implemented yet
//
//      "-a                    Transform all databases in the dbenv\n"
        "-i                    Ignore incompatible type changes.\n"
        "-p                    Purge objects whose types no longer exist.\n"
        "-c                    Use catastrophic recovery on the old database environment.\n"
        "-w                    Suppress duplicate warnings during transformation.\n"
        "-f FILE               Execute the transformation descriptors in the file FILE.\n"
        "--include-old DIR     Put DIR in the include file search path for old Slice\n"
        "                      definitions.\n"
        "--include-new DIR     Put DIR in the include file search path for new Slice\n"
        "                      definitions.\n"
        "--old SLICE           Load old Slice definitions from the file SLICE.\n"
        "--new SLICE           Load new Slice definitions from the file SLICE.\n"
        "-e                    Indicates the database is an Evictor database.\n"
        "--key TYPE[,TYPE]     Specifies the Slice types of the database key. If the\n"
        "                      type names have not changed, only one needs to be\n"
        "                      specified. Otherwise, the type names are specified as\n"
        "                      old-type,new-type.\n"
        "--value TYPE[,TYPE]   Specifies the Slice types of the database value. If the\n"
        "                      type names have not changed, only one needs to be\n"
        "                      specified. Otherwise, the type names are specified as\n"
        "                      old-type,new-type.\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

static Slice::TypePtr
findType(const string& prog, const Slice::UnitPtr& u, const string& type)
{
    Slice::TypeList l;

    l = u->lookupType(type, false);
    if(l.empty())
    {
        cerr << prog << ": error: unknown type `" << type << "'" << endl;
        return 0;
    }

    return l.front();
}

static void
transformDb(bool evictor,  const Ice::CommunicatorPtr& communicator,
	    const FreezeScript::ObjectFactoryPtr& objectFactory,
	    DbEnv& dbEnv, DbEnv& dbEnvNew, const string& dbName, 
	    const Freeze::ConnectionPtr& connectionNew, vector<Db*>& dbs,
	    const Slice::UnitPtr& oldUnit, const Slice::UnitPtr& newUnit, 
	    DbTxn* txnNew, bool purgeObjects, bool suppress, string descriptors)
{
    if(evictor)
    {
	//
	// The evictor database file contains multiple databases. We must first
	// determine the names of those databases, ignoring any whose names
	// begin with "$index:". Each database represents a separate facet, with
	// the facet name used as the database name. The database named "$default"
	// represents the main object.
	//
	vector<string> dbNames;
	{
	    Db db(&dbEnv, 0);
	    db.open(0, dbName.c_str(), 0, DB_UNKNOWN, DB_RDONLY, 0);
	    Dbt dbKey, dbValue;
	    dbKey.set_flags(DB_DBT_MALLOC);
	    dbValue.set_flags(DB_DBT_USERMEM | DB_DBT_PARTIAL);
	    
	    Dbc* dbc = 0;
	    db.cursor(0, &dbc, 0);
	    
	    while(dbc->get(&dbKey, &dbValue, DB_NEXT) == 0)
	    {
		string s(static_cast<char*>(dbKey.get_data()), dbKey.get_size());
		if(s.find("$index:") != 0)
		{
		    dbNames.push_back(s);
		}
		free(dbKey.get_data());
	    }
	    
	    dbc->close();
	    db.close(0);
	}
	
	//
	// Transform each database. We must delay closing the new databases
	// until after the transaction is committed or aborted.
	//
	for(vector<string>::iterator p = dbNames.begin(); p != dbNames.end(); ++p)
	{
	    string name = p->c_str();
	    
	    Db db(&dbEnv, 0);
	    db.open(0, dbName.c_str(), name.c_str(), DB_BTREE, DB_RDONLY, FREEZE_SCRIPT_DB_MODE);
	    
	    Db* dbNew = new Db(&dbEnvNew, 0);
	    dbs.push_back(dbNew);
	    dbNew->open(txnNew, dbName.c_str(), name.c_str(), DB_BTREE, DB_CREATE | DB_EXCL, FREEZE_SCRIPT_DB_MODE);
	    
	    //
	    // Execute the transformation descriptors.
	    //
	    istringstream istr(descriptors);
	    string facet = (name == "$default" ? string("") : name);
	    FreezeScript::transformDatabase(communicator, objectFactory, oldUnit, newUnit, &db, dbNew, txnNew, 0,
					    dbName, facet, purgeObjects, cerr, suppress, istr);
	    
	    db.close(0);
	}
	
	Freeze::Catalog catalogNew(connectionNew, Freeze::catalogName());
	Freeze::CatalogData catalogData;
	catalogData.evictor = true;
	catalogNew.put(Freeze::Catalog::value_type(dbName, catalogData));
    }
    else
    {
	//
	// Transform a map database.
	//
	Db db(&dbEnv, 0);
	db.open(0, dbName.c_str(), 0, DB_BTREE, DB_RDONLY, FREEZE_SCRIPT_DB_MODE);
	
	Db* dbNew = new Db(&dbEnvNew, 0);
	dbs.push_back(dbNew);
	dbNew->open(txnNew, dbName.c_str(), 0, DB_BTREE, DB_CREATE | DB_EXCL, FREEZE_SCRIPT_DB_MODE);
	
	//
	// Execute the transformation descriptors.
	//
	istringstream istr(descriptors);
	FreezeScript::transformDatabase(communicator, objectFactory, oldUnit, newUnit, &db, dbNew, txnNew,
					connectionNew, dbName, "", purgeObjects, cerr, suppress, istr);
	
	db.close(0);
    }
}

static int
run(int argc, char** argv, const Ice::CommunicatorPtr& communicator)
{
    string oldCppArgs;
    string newCppArgs;
    bool debug;
    bool ice = true; // Needs to be true in order to create default definitions.
    string outputFile;
    bool allDb;
    bool ignoreTypeChanges;
    bool purgeObjects;
    bool catastrophicRecover;
    bool suppress;
    string inputFile;
    vector<string> oldSlice;
    vector<string> newSlice;
    bool evictor;
    bool caseSensitive;
    string keyTypeNames;
    string valueTypeNames;
    string dbEnvName, dbName, dbEnvNameNew;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("D", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("U", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("o", "", IceUtil::Options::NeedArg);
    opts.addOpt("a");
    opts.addOpt("i");
    opts.addOpt("p");
    opts.addOpt("c");
    opts.addOpt("w");
    opts.addOpt("f", "", IceUtil::Options::NeedArg);
    opts.addOpt("", "include-old", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "include-new", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "old", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "new", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("e");
    opts.addOpt("", "key", IceUtil::Options::NeedArg);
    opts.addOpt("", "value", IceUtil::Options::NeedArg);
    opts.addOpt("", "case-sensitive");

    vector<string> args;
    try
    {
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtil::Options::BadOpt& e)
    {
	cerr << argv[0] << ": " << e.reason << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    if(opts.isSet("h") || opts.isSet("help"))
    {
	usage(argv[0]);
	return EXIT_SUCCESS;
    }
    if(opts.isSet("v") || opts.isSet("version"))
    {
	cout << ICE_STRING_VERSION << endl;
	return EXIT_SUCCESS;
    }
    if(opts.isSet("D"))
    {
	vector<string> optargs = opts.argVec("D");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    oldCppArgs += " -D" + *i;
	    newCppArgs += " -D" + *i;
	}
    }
    if(opts.isSet("U"))
    {
	vector<string> optargs = opts.argVec("U");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    oldCppArgs += " -U" + *i;
	    newCppArgs += " -U" + *i;
	}
    }
    debug = opts.isSet("d") || opts.isSet("debug");

    // No need to set --ice here, it is always true.

    if(opts.isSet("o"))
    {
	outputFile = opts.optArg("o");
    }
    allDb = opts.isSet("a");
    ignoreTypeChanges = opts.isSet("i");
    purgeObjects = opts.isSet("p");
    catastrophicRecover = opts.isSet("c");
    suppress = opts.isSet("w");
    if(opts.isSet("f"))
    {
	inputFile = opts.optArg("f");
    }
    if(opts.isSet("include-old"))
    {
	vector<string> optargs = opts.argVec("include-old");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    oldCppArgs += " -I" + *i;
	}
    }
    if(opts.isSet("include-new"))
    {
	vector<string> optargs = opts.argVec("include-new");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    newCppArgs += " -I" + *i;
	}
    }
    if(opts.isSet("old"))
    {
	vector<string> optargs = opts.argVec("old");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    oldSlice.push_back(*i);
	}
    }
    if(opts.isSet("new"))
    {
	vector<string> optargs = opts.argVec("new");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    newSlice.push_back(*i);
	}
    }
    evictor = opts.isSet("e");
    if(opts.isSet("key"))
    {
	keyTypeNames = opts.optArg("key");
    }
    if(opts.isSet("value"))
    {
	valueTypeNames = opts.optArg("value");
    }
    caseSensitive = opts.isSet("case-sensitive");

    if(outputFile.empty() && (allDb && args.size() != 2 || !allDb && args.size() != 3))
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if(!args.empty())
    {
        dbEnvName = args[0];
    }
    if(args.size() > 1)
    {
	if(allDb)
	{
	    dbEnvNameNew = args[1];
	}
	else
	{
	    dbName = args[1];
	}
    }
    if(args.size() > 2)
    {
        dbEnvNameNew = args[2];
    }

    Slice::UnitPtr oldUnit = Slice::Unit::createUnit(true, true, ice, caseSensitive);
    FreezeScript::Destroyer<Slice::UnitPtr> oldD(oldUnit);
    if(!FreezeScript::parseSlice(argv[0], oldUnit, oldSlice, oldCppArgs, debug))
    {
        return EXIT_FAILURE;
    }

    Slice::UnitPtr newUnit = Slice::Unit::createUnit(true, true, ice, caseSensitive);
    FreezeScript::Destroyer<Slice::UnitPtr> newD(newUnit);
    if(!FreezeScript::parseSlice(argv[0], newUnit, newSlice, newCppArgs, debug))
    {
        return EXIT_FAILURE;
    }

    //
    // Install the evictor types in the Slice units.
    //
    FreezeScript::createEvictorSliceTypes(oldUnit);
    FreezeScript::createEvictorSliceTypes(newUnit);

    //
    // If no input file was provided, then we need to analyze the Slice types.
    //
    string descriptors;
    
    if(inputFile.empty())
    {
        ostringstream out;
        vector<string> missingTypes;
        vector<string> analyzeErrors;

        string oldKeyName, newKeyName, oldValueName, newValueName;

	if(evictor)
        {
            oldKeyName = newKeyName = "::Ice::Identity";
            oldValueName = newValueName = "::Freeze::ObjectRecord";
        }
        else
        {
            string::size_type pos;

            if(keyTypeNames.empty() || valueTypeNames.empty())
            {
                usage(argv[0]);
                return EXIT_FAILURE;
            }

            pos = keyTypeNames.find(',');
            if(pos == 0 || pos == keyTypeNames.size())
            {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            if(pos == string::npos)
            {
                oldKeyName = keyTypeNames;
                newKeyName = keyTypeNames;
            }
            else
            {
                oldKeyName = keyTypeNames.substr(0, pos);
                newKeyName = keyTypeNames.substr(pos + 1);
            }

            pos = valueTypeNames.find(',');
            if(pos == 0 || pos == valueTypeNames.size())
            {
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            if(pos == string::npos)
            {
                oldValueName = valueTypeNames;
                newValueName = valueTypeNames;
            }
            else
            {
                oldValueName = valueTypeNames.substr(0, pos);
                newValueName = valueTypeNames.substr(pos + 1);
            }
        }

        Slice::TypePtr oldKeyType = findType(argv[0], oldUnit, oldKeyName);
        Slice::TypePtr newKeyType = findType(argv[0], newUnit, newKeyName);
        Slice::TypePtr oldValueType = findType(argv[0], oldUnit, oldValueName);
        Slice::TypePtr newValueType = findType(argv[0], newUnit, newValueName);
        if(!oldKeyType || !newKeyType || !oldValueType || !newValueType)
        {   
            return EXIT_FAILURE;
        }

        FreezeScript::TransformAnalyzer analyzer(oldUnit, newUnit, ignoreTypeChanges);
        analyzer.analyze(oldKeyType, newKeyType, oldValueType, newValueType, out, missingTypes, analyzeErrors);

        if(!analyzeErrors.empty())
        {
            for(vector<string>::const_iterator p = analyzeErrors.begin(); p != analyzeErrors.end(); ++p)
            {
                cerr << argv[0] << ": " << *p << endl;
            }
        }

        if(!missingTypes.empty())
        {
            sort(missingTypes.begin(), missingTypes.end());
            unique(missingTypes.begin(), missingTypes.end());
            if(!analyzeErrors.empty())
            {
                cerr << endl;
            }
            cerr << "The following types had no matching definitions in the new Slice:" << endl;
            for(vector<string>::const_iterator p = missingTypes.begin(); p != missingTypes.end(); ++p)
            {
                cerr << "  " << *p << endl;
            }
        }

        if(!analyzeErrors.empty())
        {
            return EXIT_FAILURE;
        }

        descriptors = out.str();

        if(!outputFile.empty())
        {
            ofstream of(outputFile.c_str());
            if(!of.good())
            {
                cerr << argv[0] << ": unable to open file `" << outputFile << "'" << endl;
                return EXIT_FAILURE;
            }
            of << descriptors;
            of.close();
            return EXIT_SUCCESS;
        }
    }
    else
    {
        //
        // Read the input file.
        //
        ifstream in(inputFile.c_str());
        char buff[1024];
        while(true)
        {
            in.read(buff, 1024);
            descriptors.append(buff, in.gcount());
            if(in.gcount() < 1024)
            {
                break;
            }
        }
        in.close();
    }

    if(dbEnvName == dbEnvNameNew)
    {
        cerr << argv[0] << ": database environment names must be different" << endl;
        return EXIT_FAILURE;
    }

    FreezeScript::ObjectFactoryPtr objectFactory = new FreezeScript::ObjectFactory;
    communicator->addObjectFactory(objectFactory, "");

    //
    // Transform the database.
    //
    DbEnv dbEnv(0);
    DbEnv dbEnvNew(0);
    Freeze::TransactionPtr txNew = 0;
    vector<Db*> dbs;
    int status = EXIT_SUCCESS;
    try
    {
#ifdef _WIN32
        //
        // Berkeley DB may use a different C++ runtime.
        //
        dbEnv.set_alloc(::malloc, ::realloc, ::free);
        dbEnvNew.set_alloc(::malloc, ::realloc, ::free);
#endif

        //
        // Open the old database environment. Use DB_RECOVER_FATAL if -c is specified.
        // No transaction is created for the old environment.
        //
	// DB_THREAD is for compatibility with Freeze (the catalog)
        {
            u_int32_t flags = DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN | DB_CREATE | DB_THREAD;
            if(catastrophicRecover)
            {
                flags |= DB_RECOVER_FATAL;
            }
            else
            {
                flags |= DB_RECOVER;
            }
            dbEnv.open(dbEnvName.c_str(), flags, FREEZE_SCRIPT_DB_MODE);
        }

        //
        // Open the new database environment and start a transaction.
        //
	//
	// DB_THREAD is for compatibility with Freeze (the catalog)
	//
        {
            u_int32_t flags = DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_CREATE
		| DB_THREAD;
            dbEnvNew.open(dbEnvNameNew.c_str(), flags, FREEZE_SCRIPT_DB_MODE);
        }

	//
	// TODO: handle properly DbHome config (curremtly it will break if it's set for the new env)
	//

	//
	// Open the catalog of the new environment, and start a transaction
	//
	Freeze::ConnectionPtr connectionNew = Freeze::createConnection(communicator, dbEnvNameNew, dbEnvNew);
	txNew = connectionNew->beginTransaction();
	DbTxn* txnNew = Freeze::getTxn(txNew);


	if(allDb)
	{
	    //
	    // Transform all the databases listed in the old catalog
	    //
	    //
	    // Create each time a new descriptor with the appropriate default database element
	    //

	    Freeze::ConnectionPtr connection = Freeze::createConnection(communicator, dbEnvName, dbEnv);
	    Freeze::Catalog catalog(connection, Freeze::catalogName());
	    for(Freeze::Catalog::const_iterator p = catalog.begin(); p != catalog.end(); ++p)
	    {
		string dbElement;
		if(p->second.evictor)
		{
		    dbElement = "<database key=\"::Ice::Identity\" value=\"::Freeze::ObjectRecord\"> <record/> </database>";
		}
		else
		{
		    //
		    // Temporary: FreezeScript should accept type-ids
		    //
		    string value = p->second.value;
		    if(value == "::Ice::Object")
		    {
			value = "Object";
		    }
		    else if(value == "::Ice::Object*")
		    {
			value = "Object*";
		    }

		    dbElement = "<database key=\"" + p->second.key + "\" value=\"" + value + "\"> <record/> </database>"; 
		}
		
		string localDescriptor = descriptors;
		string transformdbTag = "<transformdb>";
		size_t pos = localDescriptor.find(transformdbTag);
		if(pos == string::npos)
		{
		    cerr << argv[0] << ": Invalid descriptor " << endl;
		    status = EXIT_FAILURE;
		    break;
		}
		localDescriptor.replace(pos, transformdbTag.size(), transformdbTag + dbElement); 
		
		if(debug)
		{
		    cout << "Processing database " << p->first << " with descriptor: " << localDescriptor << endl;
		}

		transformDb(p->second.evictor, communicator, objectFactory, dbEnv, dbEnvNew, p->first, connectionNew,
			    dbs, oldUnit, newUnit, txnNew, purgeObjects, suppress, localDescriptor);
	    }
	}
	else
	{
	    transformDb(evictor, communicator, objectFactory, dbEnv, dbEnvNew, dbName, connectionNew, dbs, 
			oldUnit, newUnit, txnNew, purgeObjects, suppress, descriptors);
	}
    }
    catch(const DbException& ex)
    {
        cerr << argv[0] << ": database error: " << ex.what() << endl;
        status = EXIT_FAILURE;
    }
    catch(...)
    {
        try
        {
            if(txNew != 0)
            {
                txNew->rollback();
            }
            for(vector<Db*>::iterator p = dbs.begin(); p != dbs.end(); ++p)
            {
                Db* db = *p;
                db->close(0);
                delete db;
            }
            dbEnv.close(0);
            dbEnvNew.close(0);
        }
        catch(const DbException& ex)
        {
            cerr << argv[0] << ": database error: " << ex.what() << endl;
        }
        throw;
    }

    if(txNew != 0)
    {
        try
        {
            if(status == EXIT_FAILURE)
            {
                txNew->rollback();
            }
            else
            {
                txNew->commit();

                //
                // Checkpoint to migrate changes from the log to the database(s).
                //
                dbEnvNew.txn_checkpoint(0, 0, DB_FORCE);

                for(vector<Db*>::iterator p = dbs.begin(); p != dbs.end(); ++p)
                {
                    Db* db = *p;
                    db->close(0);
                    delete db;
                }
            }
        }
        catch(const DbException& ex)
        {
            cerr << argv[0] << ": database error: " << ex.what() << endl;
            status = EXIT_FAILURE;
        }
    }
    dbEnv.close(0);
    dbEnvNew.close(0);

    return status;
}

int
main(int argc, char* argv[])
{
    Ice::CommunicatorPtr communicator;
    int status = EXIT_SUCCESS;
    try
    {
        communicator = Ice::initialize(argc, argv);
        status = run(argc, argv, communicator);
    }
    catch(const FreezeScript::FailureException& ex)
    {
        string reason = ex.reason();
        cerr << argv[0] << ": " << reason;
        if(reason[reason.size() - 1] != '\n')
        {
            cerr << endl;
        }
        return EXIT_FAILURE;
    }
    catch(const IceUtil::Exception& ex)
    {
        cerr << argv[0] << ": " << ex << endl;
        return EXIT_FAILURE;
    }

    if(communicator)
    {
        communicator->destroy();
    }

    return status;
}
