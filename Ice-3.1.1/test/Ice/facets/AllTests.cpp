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

class EmptyI : virtual public Empty
{
};

GPrx
allTests(const Ice::CommunicatorPtr& communicator)
{
    cout << "testing facet registration exceptions... " << flush;
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("FacetExceptionTestAdapter");
    Ice::ObjectPtr obj = new EmptyI;
    adapter->add(obj, communicator->stringToIdentity("d"));
    adapter->addFacet(obj, communicator->stringToIdentity("d"), "facetABCD");
    try
    {
	adapter->addFacet(obj, communicator->stringToIdentity("d"), "facetABCD");
	test(false);
    }
    catch(Ice::AlreadyRegisteredException&)
    {
    }
    adapter->removeFacet(communicator->stringToIdentity("d"), "facetABCD");
    try
    {
	adapter->removeFacet(communicator->stringToIdentity("d"), "facetABCD");
	test(false);
    }
    catch(Ice::NotRegisteredException&)
    {
    }
    cout << "ok" << endl;

    cout << "testing removeAllFacets..." << flush;
    Ice::ObjectPtr obj1 = new EmptyI;
    Ice::ObjectPtr obj2 = new EmptyI;
    adapter->addFacet(obj1, communicator->stringToIdentity("id1"), "f1");
    adapter->addFacet(obj2, communicator->stringToIdentity("id1"), "f2");
    Ice::ObjectPtr obj3 = new EmptyI;
    adapter->addFacet(obj1, communicator->stringToIdentity("id2"), "f1");
    adapter->addFacet(obj2, communicator->stringToIdentity("id2"), "f2");
    adapter->addFacet(obj3, communicator->stringToIdentity("id2"), "");
    Ice::FacetMap fm = adapter->removeAllFacets(communicator->stringToIdentity("id1"));
    test(fm.size() == 2);
    test(fm["f1"] == obj1);
    test(fm["f2"] == obj2);
    try
    {
	adapter->removeAllFacets(communicator->stringToIdentity("id1"));
	test(false);
    }
    catch(Ice::NotRegisteredException&)
    {
    }
    fm = adapter->removeAllFacets(communicator->stringToIdentity("id2"));
    test(fm.size() == 3);
    test(fm["f1"] == obj1);
    test(fm["f2"] == obj2);
    test(fm[""] == obj3);
    cout << "ok" << endl;

    adapter->deactivate();

    cout << "testing stringToProxy... " << flush;
    string ref = "d:default -p 12010 -t 10000";
    Ice::ObjectPrx db = communicator->stringToProxy(ref);
    test(db);
    cout << "ok" << endl;

    cout << "testing checked cast... " << flush;
    DPrx d = DPrx::checkedCast(db);
    test(d);
    test(d == db);
    cout << "ok" << endl;

    cout << "testing non-facets A, B, C, and D... " << flush;
    test(d->callA() == "A");
    test(d->callB() == "B");
    test(d->callC() == "C");
    test(d->callD() == "D");
    cout << "ok" << endl;

    cout << "testing facets A, B, C, and D... " << flush;
    DPrx df = DPrx::checkedCast(d, "facetABCD");
    test(df);
    test(df->callA() == "A");
    test(df->callB() == "B");
    test(df->callC() == "C");
    test(df->callD() == "D");
    cout << "ok" << endl;

    cout << "testing facets E and F... " << flush;
    FPrx ff = FPrx::checkedCast(d, "facetEF");
    test(ff);
    test(ff->callE() == "E");
    test(ff->callF() == "F");
    cout << "ok" << endl;

    cout << "testing facet G... " << flush;
    GPrx gf = GPrx::checkedCast(ff, "facetGH");
    test(gf);
    test(gf->callG() == "G");
    cout << "ok" << endl;

    cout << "testing whether casting preserves the facet... " << flush;
    HPrx hf = HPrx::checkedCast(gf);
    test(hf);
    test(hf->callG() == "G");
    test(hf->callH() == "H");
    cout << "ok" << endl;

    return gf;
}
