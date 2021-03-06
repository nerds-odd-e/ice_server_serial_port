// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>

using namespace std;
using namespace Test;

int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator)
{
    bool withDeploy = false;
    
    for(int i = 1; i < argc; ++i)
    {
	if(strcmp(argv[i], "--with-deploy") == 0)
	{
	    withDeploy = true;
	    break;
	}
    }

    if(!withDeploy)
    {
	void allTests(const Ice::CommunicatorPtr&);
	allTests(communicator);
    }
    else
    {
	void allTestsWithDeploy(const Ice::CommunicatorPtr&);
	allTestsWithDeploy(communicator);
    }

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    int status;

    Ice::CommunicatorPtr communicator;

    try
    {
	communicator = Ice::initialize(argc, argv);
	status = run(argc, argv, communicator);
    }
    catch(const Ice::Exception& ex)
    {
	cerr << ex << endl;
	status = EXIT_FAILURE;
    }

    if(communicator)
    {
	try
	{
	    communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
	    cerr << ex << endl;
	    status = EXIT_FAILURE;
	}
    }

    return status;
}
