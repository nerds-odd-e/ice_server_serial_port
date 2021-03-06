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
using namespace Ice;
using namespace Test;

TestIntfPrx
allTests(const CommunicatorPtr& communicator)
{
    cout << "testing stringToProxy... " << flush;
    ObjectPrx base = communicator->stringToProxy("test:default -p 12010 -t 10000");
    test(base);
    cout << "ok" << endl;

    cout << "testing checked cast... " << flush;
    TestIntfPrx obj = TestIntfPrx::checkedCast(base);
    test(obj);
    test(obj == base);
    cout << "ok" << endl;

    {
        cout << "creating/destroying/recreating object adapter... " << flush;
	ObjectAdapterPtr adapter = 
	    communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default -p 9999");
	try
	{
	    communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default -p 9998");
	    test(false);
	}
	catch(const AlreadyRegisteredException&)
	{
	}
	adapter->deactivate();
	adapter->waitForDeactivate();
	//
	// Use a different port than the first adapter to avoid an "address already in use" error.
	//
	adapter = communicator->createObjectAdapterWithEndpoints("TransientTestAdapter", "default -p 9998");
	adapter->deactivate();
	adapter->waitForDeactivate();
        cout << "ok" << endl;
    }

    cout << "creating/activating/deactivating object adapter in one operation... " << flush;
    obj->transient();
    cout << "ok" << endl;

    cout << "deactivating object adapter in the server... " << flush;
    obj->deactivate();
    cout << "ok" << endl;

    cout << "testing whether server is gone... " << flush;
    try
    {
	obj->ice_ping();
	test(false);
    }
    catch(const LocalException&)
    {
	cout << "ok" << endl;
    }

    return obj;
}
