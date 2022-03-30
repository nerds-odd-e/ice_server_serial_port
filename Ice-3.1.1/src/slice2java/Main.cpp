// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Options.h>
#include <Slice/Preprocessor.h>
#include <Gen.h>

#ifdef __BCPLUSPLUS__
#  include <iterator>
#endif

using namespace std;
using namespace Slice;

void
usage(const char* n)
{
    cerr << "Usage: " << n << " [options] slice-files...\n";
    cerr <<        
        "Options:\n"
        "-h, --help              Show this message.\n"
        "-v, --version           Display the Ice version.\n"
        "-DNAME                  Define NAME as 1.\n"
        "-DNAME=DEF              Define NAME as DEF.\n"
        "-UNAME                  Remove any definition for NAME.\n"
        "-IDIR                   Put DIR in the include file search path.\n"
	"-E                      Print preprocessor output on stdout.\n"
        "--output-dir DIR        Create files in the directory DIR.\n"
        "--tie                   Generate TIE classes.\n"
        "--impl                  Generate sample implementations.\n"
        "--impl-tie              Generate sample TIE implementations.\n"
	"--depend                Generate Makefile dependencies.\n"
        "-d, --debug             Print debug messages.\n"
        "--ice                   Permit `Ice' prefix (for building Ice source code only)\n"
        "--checksum CLASS        Generate checksums for Slice definitions into CLASS.\n"
        "--stream                Generate marshaling support for public stream API.\n"
        "--meta META             Define global metadata directive META.\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

int
main(int argc, char* argv[])
{
    string cppArgs;
    vector<string> includePaths;
    bool preprocess;
    string output;
    bool tie;
    bool impl;
    bool implTie;
    bool depend;
    bool debug;
    bool ice;
    string checksumClass;
    bool stream;
    StringList globalMetadata;
    bool caseSensitive;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("D", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("U", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("I", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("E");
    opts.addOpt("", "output-dir", IceUtil::Options::NeedArg);
    opts.addOpt("", "tie");
    opts.addOpt("", "impl");
    opts.addOpt("", "impl-tie");
    opts.addOpt("", "depend");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "checksum", IceUtil::Options::NeedArg);
    opts.addOpt("", "stream");
    opts.addOpt("", "meta", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("", "case-sensitive");

    vector<string>args;
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
	    cppArgs += " -D\"" + *i + "\"";
	}
    }
    if(opts.isSet("U"))
    {
        vector<string> optargs = opts.argVec("U");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cppArgs += " -U\"" + *i + "\"";
	}
    }
    if(opts.isSet("I"))
    {
        includePaths = opts.argVec("I");
	for(vector<string>::const_iterator i = includePaths.begin(); i != includePaths.end(); ++i)
	{
	    cppArgs += " -I\"" + *i + "\"";
	}
    }
    preprocess = opts.isSet("E");
    if(opts.isSet("output-dir"))
    {
	output = opts.optArg("output-dir");
    }
    tie = opts.isSet("tie");
    impl = opts.isSet("impl");
    implTie = opts.isSet("impl-tie");
    depend = opts.isSet("depend");
    debug = opts.isSet("d") || opts.isSet("debug");
    ice = opts.isSet("ice");
    if(opts.isSet("checksum"))
    {
	checksumClass = opts.optArg("checksum");
    }
    stream = opts.isSet("stream");
    if(opts.isSet("meta"))
    {
	vector<string> v = opts.argVec("meta");
	copy(v.begin(), v.end(), back_inserter(globalMetadata));
    }
    caseSensitive = opts.isSet("case-sensitive");

    if(args.empty())
    {
        cerr << argv[0] << ": no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if(impl && implTie)
    {
        cerr << argv[0] << ": cannot specify both --impl and --impl-tie" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    ChecksumMap checksums;

    for(vector<string>::const_iterator i = args.begin(); i != args.end(); ++i)
    {
	if(depend)
	{
	    Preprocessor icecpp(argv[0], *i, cppArgs);
	    icecpp.printMakefileDependencies(Preprocessor::Java);
	}
	else
	{
	    Preprocessor icecpp(argv[0], *i, cppArgs);
	    FILE* cppHandle = icecpp.preprocess(false);

	    if(cppHandle == 0)
	    {
		return EXIT_FAILURE;
	    }

	    if(preprocess)
	    {
	        char buf[4096];
		while(fgets(buf, static_cast<int>(sizeof(buf)), cppHandle) != NULL)
		{
		    if(fputs(buf, stdout) == EOF)
		    {
		        return EXIT_FAILURE;
		    }
		}
		if(!icecpp.close())
		{
		    return EXIT_FAILURE;
		}
	    }
	    else
	    {
		UnitPtr p = Unit::createUnit(false, false, ice, caseSensitive, globalMetadata);
		int parseStatus = p->parse(cppHandle, debug, Ice);

		if(!icecpp.close())
		{
		    p->destroy();
		    return EXIT_FAILURE;
		}	    
		
		if(parseStatus == EXIT_FAILURE)
		{
		    status = EXIT_FAILURE;
		}
		else
		{
		    Gen gen(argv[0], icecpp.getBaseName(), includePaths, output);
		    if(!gen)
		    {
			p->destroy();
			return EXIT_FAILURE;
		    }
		    gen.generate(p, stream);
		    if(tie)
		    {
			gen.generateTie(p);
		    }
		    if(impl)
		    {
			gen.generateImpl(p);
		    }
		    if(implTie)
		    {
			gen.generateImplTie(p);
		    }
		    if(!checksumClass.empty())
		    {
			//
			// Calculate checksums for the Slice definitions in the unit.
			//
			ChecksumMap m = createChecksums(p);
			copy(m.begin(), m.end(), inserter(checksums, checksums.begin()));
		    }
		}
		p->destroy();
	    }
	}
    }

    if(!checksumClass.empty())
    {
	//
	// Look for the Java5 metadata.
	//
	bool java5 = false;
	for(StringList::iterator p = globalMetadata.begin(); p != globalMetadata.end(); ++p)
	{
	    if((*p) == "java:java5")
	    {
		java5 = true;
		break;
	    }
	}
	Gen::writeChecksumClass(checksumClass, output, checksums, java5);
    }

    return status;
}
