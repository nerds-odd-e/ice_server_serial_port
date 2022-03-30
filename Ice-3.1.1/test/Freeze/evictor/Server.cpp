// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <TestI.h>
#include <Ice/Ice.h>

using namespace std;

class ServantFactory : public Ice::ObjectFactory
{
public:

    virtual Ice::ObjectPtr
    create(const string& type)
    {
        assert(type == "::Test::Servant");
        return new Test::ServantI;
    }

    virtual void
    destroy()
    {
    }
};


class FacetFactory : public Ice::ObjectFactory
{
public:

    virtual Ice::ObjectPtr
    create(const string& type)
    {
        assert(type == "::Test::Facet");
        return new Test::FacetI;
    }

    virtual void
    destroy()
    {
    }
};
int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator, const string& envName)
{
    communicator->getProperties()->setProperty("Factory.Endpoints", "default -p 12010 -t 30000");

    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("Factory");

    Test::RemoteEvictorFactoryPtr factory = new Test::RemoteEvictorFactoryI(adapter, envName);
    adapter->add(factory, communicator->stringToIdentity("factory"));

    Ice::ObjectFactoryPtr servantFactory = new ServantFactory;
    communicator->addObjectFactory(servantFactory, "::Test::Servant");

    Ice::ObjectFactoryPtr facetFactory = new FacetFactory;
    communicator->addObjectFactory(facetFactory, "::Test::Facet");

    adapter->activate();

    communicator->waitForShutdown();

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;
    string envName = "db";

    try
    {
        communicator = Ice::initialize(argc, argv);
        status = run(argc, argv, communicator, envName);
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
