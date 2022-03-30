// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_REFERENCE_FACTORY_H
#define ICE_REFERENCE_FACTORY_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <Ice/ReferenceFactoryF.h>
#include <Ice/Reference.h> // For Reference::Mode
#include <Ice/ConnectionIF.h>

namespace IceInternal
{

class ReferenceFactory : public ::IceUtil::Shared, public ::IceUtil::Mutex
{
public:

    //
    // Make a polymorphic copy of a reference.
    //
    ReferencePtr copy(const Reference* r) const;

    //
    // Create a direct reference.
    //
    ReferencePtr create(const ::Ice::Identity&, const ::Ice::Context&, const ::std::string&,
			Reference::Mode, bool, const ::std::vector<EndpointIPtr>&,
			const RouterInfoPtr&, bool);
    //
    // Create an indirect reference.
    //
    ReferencePtr create(const ::Ice::Identity&, const ::Ice::Context&, const ::std::string&,
			Reference::Mode, bool, const ::std::string&,
			const RouterInfoPtr&, const LocatorInfoPtr&, bool, int);
    //
    // Create a fixed reference.
    //
    ReferencePtr create(const ::Ice::Identity&, const ::Ice::Context&, const ::std::string&,
	                Reference::Mode, const ::std::vector< ::Ice::ConnectionIPtr>&);

    //
    // Create a reference from a string.
    //
    ReferencePtr create(const ::std::string&);

    //
    // Create a reference by unmarshaling it from a stream.
    //
    ReferencePtr create(const ::Ice::Identity&, BasicStream*);

    void setDefaultRouter(const ::Ice::RouterPrx&);
    ::Ice::RouterPrx getDefaultRouter() const;

    void setDefaultLocator(const ::Ice::LocatorPrx&);
    ::Ice::LocatorPrx getDefaultLocator() const;

private:

    ReferenceFactory(const InstancePtr&, const ::Ice::CommunicatorPtr&);
    void destroy();
    friend class Instance;

    InstancePtr _instance;
    ::Ice::CommunicatorPtr _communicator;
    ::Ice::RouterPrx _defaultRouter;
    ::Ice::LocatorPrx _defaultLocator;
};

}

#endif
