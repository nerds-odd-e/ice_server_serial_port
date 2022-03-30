
// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_LOCATOR_REGISTRY_I_H
#define ICE_GRID_LOCATOR_REGISTRY_I_H

#include <IceGrid/Internal.h>
#include <Ice/Locator.h>

namespace IceGrid
{

class Database;
typedef IceUtil::Handle<Database> DatabasePtr;

class LocatorRegistryI : public Ice::LocatorRegistry
{
public:

    LocatorRegistryI(const DatabasePtr&, bool);
    
    virtual void setAdapterDirectProxy_async(const Ice::AMD_LocatorRegistry_setAdapterDirectProxyPtr&,
					     const std::string&, const Ice::ObjectPrx&, const Ice::Current&);

    virtual void setReplicatedAdapterDirectProxy_async(
	const Ice::AMD_LocatorRegistry_setReplicatedAdapterDirectProxyPtr&, const std::string&, const std::string&, 
	const Ice::ObjectPrx&, const Ice::Current&);

    virtual void setServerProcessProxy_async(const Ice::AMD_LocatorRegistry_setServerProcessProxyPtr&,
					     const ::std::string&, const ::Ice::ProcessPrx&, const ::Ice::Current&);

private:
    
    const DatabasePtr _database;
    const bool _dynamicRegistration;
};

}

#endif
