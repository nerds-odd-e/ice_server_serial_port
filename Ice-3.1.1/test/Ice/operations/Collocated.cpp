// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestI.h>

using namespace std;

int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator,
    const Ice::InitializationData& initData)
{
    communicator->getProperties()->setProperty("TestAdapter.Endpoints", "default -p 12010 -t 10000");
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("TestAdapter");
    Ice::Identity id = communicator->stringToIdentity("test");
    adapter->add(new MyDerivedClassI(adapter, id), id);
    adapter->add(new TestCheckedCastI, communicator->stringToIdentity("context"));
    adapter->activate();

    Test::MyClassPrx allTests(const Ice::CommunicatorPtr&, const Ice::InitializationData&, bool);
    allTests(communicator, initData, true);

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;

    try
    {
	Ice::InitializationData initData;
	initData.properties = Ice::createProperties(argc, argv);
	communicator = Ice::initialize(argc, argv, initData);
	status = run(argc, argv, communicator, initData);
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
