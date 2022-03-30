// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Ice/Locator.h>
#include <TestCommon.h>
#include <Test.h>

using namespace std;

Test::MyClassPrx
allTests(const Ice::CommunicatorPtr& communicator, 
	 const Ice::InitializationData& initData, bool collocated)
{
    cout << "testing stringToProxy... " << flush;
    string ref = "test:default -p 12010 -t 10000";
    Ice::ObjectPrx base = communicator->stringToProxy(ref);
    test(base);
    cout << "ok" << endl;

    cout << "testing ice_getCommunicator... " << flush;
    test(base->ice_getCommunicator() == communicator);
    cout << "ok" << endl;

    cout << "testing proxy methods... " << flush;
    test(communicator->identityToString(base->ice_identity(communicator->stringToIdentity("other"))->ice_getIdentity())
         == "other");
    test(base->ice_facet("facet")->ice_getFacet() == "facet");
    test(base->ice_adapterId("id")->ice_getAdapterId() == "id");
    test(base->ice_twoway()->ice_isTwoway());
    test(base->ice_oneway()->ice_isOneway());
    test(base->ice_batchOneway()->ice_isBatchOneway());
    test(base->ice_datagram()->ice_isDatagram());
    test(base->ice_batchDatagram()->ice_isBatchDatagram());
    test(base->ice_secure(true)->ice_isSecure());
    test(!base->ice_secure(false)->ice_isSecure());
    test(base->ice_collocationOptimized(true)->ice_isCollocationOptimized());
    test(!base->ice_collocationOptimized(false)->ice_isCollocationOptimized());
    cout << "ok" << endl;

    cout << "testing proxy comparison... " << flush;

    test(communicator->stringToProxy("foo") == communicator->stringToProxy("foo"));
    test(communicator->stringToProxy("foo") != communicator->stringToProxy("foo2"));
    test(communicator->stringToProxy("foo") < communicator->stringToProxy("foo2"));
    test(!(communicator->stringToProxy("foo2") < communicator->stringToProxy("foo")));

    Ice::ObjectPrx compObj = communicator->stringToProxy("foo");

    test(compObj->ice_facet("facet") == compObj->ice_facet("facet"));
    test(compObj->ice_facet("facet") != compObj->ice_facet("facet1"));
    test(compObj->ice_facet("facet") < compObj->ice_facet("facet1"));
    test(!(compObj->ice_facet("facet") < compObj->ice_facet("facet")));

    test(compObj->ice_oneway() == compObj->ice_oneway());
    test(compObj->ice_oneway() != compObj->ice_twoway());
    test(compObj->ice_twoway() < compObj->ice_oneway());
    test(!(compObj->ice_oneway() < compObj->ice_twoway()));

    test(compObj->ice_secure(true) == compObj->ice_secure(true));
    test(compObj->ice_secure(false) != compObj->ice_secure(true));
    test(compObj->ice_secure(false) < compObj->ice_secure(true));
    test(!(compObj->ice_secure(true) < compObj->ice_secure(false)));

    test(compObj->ice_collocationOptimized(true) == compObj->ice_collocationOptimized(true));
    test(compObj->ice_collocationOptimized(false) != compObj->ice_collocationOptimized(true));
    test(compObj->ice_collocationOptimized(false) < compObj->ice_collocationOptimized(true));
    test(!(compObj->ice_collocationOptimized(true) < compObj->ice_collocationOptimized(false)));

    test(compObj->ice_connectionCached(true) == compObj->ice_connectionCached(true));
    test(compObj->ice_connectionCached(false) != compObj->ice_connectionCached(true));
    test(compObj->ice_connectionCached(false) < compObj->ice_connectionCached(true));
    test(!(compObj->ice_connectionCached(true) < compObj->ice_connectionCached(false)));

    test(compObj->ice_endpointSelection(Ice::Random) == compObj->ice_endpointSelection(Ice::Random));
    test(compObj->ice_endpointSelection(Ice::Random) != compObj->ice_endpointSelection(Ice::Ordered));
    test(compObj->ice_endpointSelection(Ice::Random) < compObj->ice_endpointSelection(Ice::Ordered));
    test(!(compObj->ice_endpointSelection(Ice::Ordered) < compObj->ice_endpointSelection(Ice::Random)));

    test(compObj->ice_connectionId("id2") == compObj->ice_connectionId("id2"));
    test(compObj->ice_connectionId("id1") != compObj->ice_connectionId("id2"));
    test(compObj->ice_connectionId("id1") < compObj->ice_connectionId("id2"));
    test(!(compObj->ice_connectionId("id2") < compObj->ice_connectionId("id1")));

    test(compObj->ice_compress(true) == compObj->ice_compress(true));
    test(compObj->ice_compress(false) != compObj->ice_compress(true));
    test(compObj->ice_compress(false) < compObj->ice_compress(true));
    test(!(compObj->ice_compress(true) < compObj->ice_compress(false)));

    test(compObj->ice_timeout(20) == compObj->ice_timeout(20));
    test(compObj->ice_timeout(10) != compObj->ice_timeout(20));
    test(compObj->ice_timeout(10) < compObj->ice_timeout(20));
    test(!(compObj->ice_timeout(20) < compObj->ice_timeout(10)));

    Ice::ObjectPrx compObj1 = communicator->stringToProxy("foo:tcp -h 127.0.0.1 -p 10000");
    Ice::ObjectPrx compObj2 = communicator->stringToProxy("foo:tcp -h 127.0.0.1 -p 10001");
    test(compObj1 != compObj2);
    test(compObj1 < compObj2);
    test(!(compObj2 < compObj1));

    compObj1 = communicator->stringToProxy("foo@MyAdapter1");
    compObj2 = communicator->stringToProxy("foo@MyAdapter2");
    test(compObj1 != compObj2);
    test(compObj1 < compObj2);
    test(!(compObj2 < compObj1));

    test(compObj1->ice_locatorCacheTimeout(20) == compObj1->ice_locatorCacheTimeout(20));
    test(compObj1->ice_locatorCacheTimeout(10) != compObj1->ice_locatorCacheTimeout(20));
    test(compObj1->ice_locatorCacheTimeout(10) < compObj1->ice_locatorCacheTimeout(20));
    test(!(compObj1->ice_locatorCacheTimeout(20) < compObj1->ice_locatorCacheTimeout(10)));

    compObj1 = communicator->stringToProxy("foo:tcp -h 127.0.0.1 -p 1000");
    compObj2 = communicator->stringToProxy("foo@MyAdapter1");
    test(compObj1 != compObj2);
    test(compObj1 < compObj2);
    test(!(compObj2 < compObj1));

    //
    // TODO: Ideally we should also test comparison of fixed proxies.
    //

    cout << "ok" << endl;

    cout << "testing checked cast... " << flush;
    Test::MyClassPrx cl = Test::MyClassPrx::checkedCast(base);
    test(cl);
    
    Test::MyDerivedClassPrx derived = Test::MyDerivedClassPrx::checkedCast(cl);
    test(derived);
    test(cl == base);
    test(derived == base);
    test(cl == derived);
    
    Ice::LocatorPrx loc = Ice::LocatorPrx::checkedCast(base);
    test(loc == 0);

    //
    // Upcasting
    //
    Test::MyClassPrx cl2 = Test::MyClassPrx::checkedCast(derived);
    Ice::ObjectPrx obj = Ice::ObjectPrx::checkedCast(derived);
    test(cl2);
    test(obj);
    test(cl2 == obj);
    test(cl2 == derived);

    //
    // Now with alternate API
    //
    cl = checkedCast<Test::MyClassPrx>(base);
    test(cl);
    derived = checkedCast<Test::MyDerivedClassPrx>(cl);
    test(derived);
    test(cl == base);
    test(derived == base);
    test(cl == derived);
    
    loc = checkedCast<Ice::LocatorPrx>(base);
    test(loc == 0);

    cl2 = checkedCast<Test::MyClassPrx>(derived);
    obj = checkedCast<Ice::ObjectPrx>(derived);
    test(cl2);
    test(obj);
    test(cl2 == obj);
    test(cl2 == derived);

    cout << "ok" << endl;

    cout << "testing checked cast with context... " << flush;
    string cref = "context:default -p 12010 -t 10000";
    Ice::ObjectPrx cbase = communicator->stringToProxy(cref);
    test(cbase);

    Test::TestCheckedCastPrx tccp = Test::TestCheckedCastPrx::checkedCast(cbase);
    Ice::Context c = tccp->getContext();
    test(c.size() == 0);

    c["one"] = "hello";
    c["two"] = "world";
    tccp = Test::TestCheckedCastPrx::checkedCast(cbase, c);
    Ice::Context c2 = tccp->getContext();
    test(c == c2);

    //
    // Now with alternate API
    //
    tccp = checkedCast<Test::TestCheckedCastPrx>(cbase);
    c = tccp->getContext();
    test(c.size() == 0);

    tccp = checkedCast<Test::TestCheckedCastPrx>(cbase, c);
    c2 = tccp->getContext();
    test(c == c2);

    cout << "ok" << endl;

    if(!collocated)
    {
	cout << "testing timeout... " << flush;
	Test::MyClassPrx clTimeout = Test::MyClassPrx::uncheckedCast(cl->ice_timeout(500));
	try
	{
	    clTimeout->opSleep(2000);
	    test(false);
	}
	catch(const Ice::TimeoutException&)
	{
	}
	cout << "ok" << endl;
    }

    cout << "testing twoway operations... " << flush;
    void twoways(const Ice::CommunicatorPtr&, const Ice::InitializationData&, const Test::MyClassPrx&);
    twoways(communicator, initData, cl);
    twoways(communicator, initData, derived);
    derived->opDerived();
    cout << "ok" << endl;

    if(!collocated)
    {
	cout << "testing twoway operations with AMI... " << flush;
	void twowaysAMI(const Ice::CommunicatorPtr&, const Ice::InitializationData&, const Test::MyClassPrx&);
	twowaysAMI(communicator, initData, cl);
	twowaysAMI(communicator, initData, derived);
	cout << "ok" << endl;

	cout << "testing batch oneway operations... " << flush;
	void batchOneways(const Test::MyClassPrx&);
	batchOneways(cl);
	batchOneways(derived);
	cout << "ok" << endl;
    }

    return cl;
}
