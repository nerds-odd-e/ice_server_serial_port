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

using namespace std;
using namespace Slice;
using namespace IceUtil;

void
usage(const char* n)
{
    cerr << "Usage: " << n << " [options] docbook-file slice-files...\n";
    cerr <<
        "Options:\n"
        "-h, --help           Show this message.\n"
        "-v, --version        Display the Ice version.\n"
        "-DNAME               Define NAME as 1.\n"
        "-DNAME=DEF           Define NAME as DEF.\n"
        "-UNAME               Remove any definition for NAME.\n"
        "-IDIR                Put DIR in the include file search path.\n"
	"-E                   Print preprocessor output on stdout.\n"
        "-s, --stand-alone    Create stand-alone docbook file.\n"
        "--chapter            Use \"chapter\" instead of \"section\" as\n"
        "                     top-level element.\n"
	"--noindex            Suppress generation of index pages.\n"
	"--sort-fields        Sort fields of structures, classes, and exceptions.\n"
        "-d, --debug          Print debug messages.\n"
        "--ice                Permit `Ice' prefix (for building Ice source code only)\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

int
main(int argc, char* argv[])
{
    string cppArgs;
    bool preprocess;
    bool standAlone;
    bool chapter;
    bool noIndex;
    bool debug;
    bool ice;
    bool caseSensitive;
    bool sortFields;

    IceUtil::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("D", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("U", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("I", "", IceUtil::Options::NeedArg, "", IceUtil::Options::Repeat);
    opts.addOpt("E");
    opts.addOpt("s", "stand-alone");
    opts.addOpt("", "chapter");
    opts.addOpt("", "noindex");
    opts.addOpt("", "sort-fields");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
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
	vector<string> optargs = opts.argVec("I");
	for(vector<string>::const_iterator i = optargs.begin(); i != optargs.end(); ++i)
	{
	    cppArgs += " -I\"" + *i + "\"";
	}
    }
    preprocess = opts.isSet("E");
    standAlone = opts.isSet("s") || opts.isSet("stand-alone");
    chapter = opts.isSet("chapter");
    noIndex = opts.isSet("noindex");
    sortFields = opts.isSet("sort-fields");
    debug = opts.isSet("d") || opts.isSet("debug");
    ice = opts.isSet("ice");
    caseSensitive = opts.isSet("case-sensitive");

    if(args.empty())
    {
	cerr << argv[0] << ": no docbook file specified" << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    string docbook(args[0]);
    string suffix;
    string::size_type pos = docbook.rfind('.');
    if(pos != string::npos)
    {
	suffix = docbook.substr(pos);
	transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    }
    if(suffix != ".sgml")
    {
	cerr << argv[0] << ": docbook file must end with `.sgml'" << endl;
	return EXIT_FAILURE;
    }

    if(args.size() < 2)
    {
	cerr << argv[0] << ": no input file" << endl;
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    UnitPtr p = Unit::createUnit(true, false, ice, caseSensitive);

    int status = EXIT_SUCCESS;

    for(vector<string>::size_type idx = 1; idx < args.size(); ++idx)
    {
	Preprocessor icecpp(argv[0], args[idx], cppArgs);
	FILE* cppHandle = icecpp.preprocess(true);

	if(cppHandle == 0)
	{
	    p->destroy();
	    return EXIT_FAILURE;
	}
	if(preprocess)
	{
	    char buf[4096];
	    while(fgets(buf, static_cast<int>(sizeof(buf)), cppHandle) != NULL)
	    {
		if(fputs(buf, stdout) == EOF)
		{
		    p->destroy();
		    return EXIT_FAILURE;
		}
	    }
	}
	else
	{
	    status = p->parse(cppHandle, debug);
	}

	if(!icecpp.close())
	{
	    p->destroy();
	    return EXIT_FAILURE;
	}
    }

    if(status == EXIT_SUCCESS && !preprocess)
    {
	Gen gen(argv[0], docbook, standAlone, chapter, noIndex, sortFields);
	if(!gen)
	{
	    p->destroy();
	    return EXIT_FAILURE;
	}
	gen.generate(p);
    }
    
    p->destroy();

    return status;
}
