// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <IceUtil/Options.h>
#include <Ice/Application.h>
#include <Ice/SliceChecksums.h>
#include <IceGrid/Parser.h>
#include <IceGrid/FileParserI.h>
#include <fstream>

using namespace std;
using namespace Ice;
using namespace IceGrid;

class Client : public Application
{
public:

    void usage();
    virtual int run(int, char*[]);
};

int
main(int argc, char* argv[])
{
    Client app;
    return app.main(argc, argv);
}

void
Client::usage()
{
    cerr << "Usage: " << appName() << " [options] [file...]\n";
    cerr <<	
	"Options:\n"
	"-h, --help           Show this message.\n"
	"-v, --version        Display the Ice version.\n"
	"-DNAME               Define NAME as 1.\n"
	"-DNAME=DEF           Define NAME as DEF.\n"
	"-UNAME               Remove any definition for NAME.\n"
	"-IDIR                Put DIR in the include file search path.\n"
	"-e COMMANDS          Execute COMMANDS.\n"
	"-d, --debug          Print debug messages.\n"
        "-s, --server         Start icegridadmin as a server (to parse XML files).\n"
	;
}

int
Client::run(int argc, char* argv[])
{
    string cpp("cpp");
    string commands;
    bool debug;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("D", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("U", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("I", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("e", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("d", "debug");
    opts.addOpt("s", "server");

    vector<string> args;
    try
    {
    	args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtil::Options::BadOpt& e)
    {
        cerr << e.reason << endl;
	usage();
	return EXIT_FAILURE;
    }

    if(opts.isSet("h") || opts.isSet("help"))
    {
	usage();
	return EXIT_SUCCESS;
    }
    if(opts.isSet("v") || opts.isSet("version"))
    {
	cout << ICE_STRING_VERSION << endl;
	return EXIT_SUCCESS;
    }

    if(opts.isSet("s") || opts.isSet("server"))
    {
	ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("FileParser", "tcp -h localhost");
	adapter->activate();
	ObjectPrx proxy = adapter->add(new FileParserI, communicator()->stringToIdentity("FileParser"));
	cout << proxy << endl;

	communicator()->waitForShutdown();
	return EXIT_SUCCESS;
    }


    if(opts.isSet("D"))
    {
	vector<string> optargs = opts.argVec("D");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cpp += " -D" + *i;
	}
    }
    if(opts.isSet("U"))
    {
	vector<string> optargs = opts.argVec("U");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cpp += " -U" + *i;
	}
    }
    if(opts.isSet("I"))
    {
	vector<string> optargs = opts.argVec("I");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cpp += " -I" + *i;
	}
    }
    if(opts.isSet("e"))
    {
	vector<string> optargs = opts.argVec("e");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    commands += *i + ";";
	}
    }
    debug = opts.isSet("d") || opts.isSet("debug");

    if(!args.empty() && !commands.empty())
    {
	cerr << appName() << ": `-e' option cannot be used if input files are given" << endl;
	usage();
	return EXIT_FAILURE;
    }
    
    Ice::PropertiesPtr properties = communicator()->getProperties();

    const string instanceNameProperty = "IceGrid.InstanceName";
    Identity identity;
    identity.category = properties->getPropertyWithDefault(instanceNameProperty, "IceGrid");

    identity.name = "Admin";
    AdminPrx admin = 
        AdminPrx::checkedCast(communicator()->stringToProxy("\"" + communicator()->identityToString(identity) + "\""));
    if(!admin)
    {
	cerr << appName() << ": no valid administrative interface" << endl;
	return EXIT_FAILURE;
    }

    identity.name = "Query";
    QueryPrx query = 
        QueryPrx::checkedCast(communicator()->stringToProxy("\"" + communicator()->identityToString(identity) + "\""));
    if(!query)
    {
	cerr << appName() << ": no valid query interface" << endl;
	return EXIT_FAILURE;
    }

    Ice::SliceChecksumDict serverChecksums = admin->getSliceChecksums();
    Ice::SliceChecksumDict localChecksums = Ice::sliceChecksums();

    //
    // The following slice types are only used by the admin CLI.
    //
    localChecksums.erase("::IceGrid::FileParser");
    localChecksums.erase("::IceGrid::ParseException");
			 
    for(Ice::SliceChecksumDict::const_iterator q = localChecksums.begin(); q != localChecksums.end(); ++q)
    {
        Ice::SliceChecksumDict::const_iterator r = serverChecksums.find(q->first);
        if(r == serverChecksums.end())
        {
            cerr << appName() << ": server is using unknown Slice type `" << q->first << "'" << endl;
        }
        else if(q->second != r->second)
        {
            cerr << appName() << ": server is using a different Slice definition of `" << q->first << "'" << endl;
        }
    }

    ParserPtr p = Parser::createParser(communicator(), admin, query);

    int status = EXIT_SUCCESS;

    if(args.empty()) // No files given
    {
	if(!commands.empty()) // Commands were given
	{
	    int parseStatus = p->parse(commands, debug);
	    if(parseStatus == EXIT_FAILURE)
	    {
		status = EXIT_FAILURE;
	    }
	}
	else // No commands, let's use standard input
	{
	    p->showBanner();

	    int parseStatus = p->parse(stdin, debug);
	    if(parseStatus == EXIT_FAILURE)
	    {
		status = EXIT_FAILURE;
	    }
	}
    }
    else // Process files given on the command line
    {
	for(vector<string>::const_iterator i = args.begin(); i != args.end(); ++i)
	{
	    ifstream test(i->c_str());
	    if(!test)
	    {
		cerr << appName() << ": can't open `" << *i << "' for reading: " << strerror(errno) << endl;
		return EXIT_FAILURE;
	    }
	    test.close();
	    
	    string cmd = cpp + " " + *i;
#ifdef _WIN32
	    FILE* cppHandle = _popen(cmd.c_str(), "r");
#else
	    FILE* cppHandle = popen(cmd.c_str(), "r");
#endif
	    if(cppHandle == NULL)
	    {
		cerr << appName() << ": can't run C++ preprocessor: " << strerror(errno) << endl;
		return EXIT_FAILURE;
	    }
	    
	    int parseStatus = p->parse(cppHandle, debug);
	    
#ifdef _WIN32
	    _pclose(cppHandle);
#else
	    pclose(cppHandle);
#endif

	    if(parseStatus == EXIT_FAILURE)
	    {
		status = EXIT_FAILURE;
	    }
	}
    }

    return status;
}
