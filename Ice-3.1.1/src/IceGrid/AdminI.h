// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_ADMIN_I_H
#define ICE_GRID_ADMIN_I_H

#include <IceGrid/Admin.h>

namespace IceGrid
{

class Database;
typedef IceUtil::Handle<Database> DatabasePtr;

class TraceLevels;
typedef IceUtil::Handle<TraceLevels> TraceLevelsPtr;

class RegistryI;
typedef IceUtil::Handle<RegistryI> RegistryIPtr;

class AdminSessionI;
typedef IceUtil::Handle<AdminSessionI> AdminSessionIPtr;

class AdminI : public Admin, public IceUtil::Mutex
{
public:

    AdminI(const DatabasePtr&, const RegistryIPtr&, const AdminSessionIPtr&);
    virtual ~AdminI();

    virtual void addApplication(const ApplicationDescriptor&, const Ice::Current&);
    virtual void syncApplication(const ApplicationDescriptor&, const Ice::Current&);
    virtual void updateApplication(const ApplicationUpdateDescriptor&, const Ice::Current&);
    virtual void removeApplication(const std::string&, const Ice::Current&);
    virtual void instantiateServer(const std::string&, const std::string&, const ServerInstanceDescriptor&, 
				   const Ice::Current&);
    virtual void patchApplication_async(const AMD_Admin_patchApplicationPtr&, const std::string&, bool, 
					const Ice::Current&);
    virtual ApplicationDescriptor getApplicationDescriptor(const ::std::string&, const Ice::Current&) const;
    virtual ApplicationDescriptor getDefaultApplicationDescriptor(const Ice::Current&) const;
    virtual Ice::StringSeq getAllApplicationNames(const Ice::Current&) const;

    virtual ServerInfo getServerInfo(const ::std::string&, const Ice::Current&) const;
    virtual ServerState getServerState(const ::std::string&, const Ice::Current&) const;
    virtual Ice::Int getServerPid(const ::std::string&, const Ice::Current&) const;
    virtual void startServer(const ::std::string&, const Ice::Current&);
    virtual void stopServer(const ::std::string&, const Ice::Current&);
    virtual void patchServer_async(const AMD_Admin_patchServerPtr&, const ::std::string&, bool, const Ice::Current&);
    virtual void sendSignal(const ::std::string&, const ::std::string&, const Ice::Current&);
    virtual void writeMessage(const ::std::string&, const ::std::string&, Ice::Int, const Ice::Current&);
    virtual Ice::StringSeq getAllServerIds(const Ice::Current&) const;
    virtual void enableServer(const ::std::string&, bool, const Ice::Current&);
    virtual bool isServerEnabled(const ::std::string&, const Ice::Current&) const;

    virtual AdapterInfoSeq getAdapterInfo(const ::std::string&, const ::Ice::Current&) const;
    virtual void removeAdapter(const std::string&, const Ice::Current&);
    virtual Ice::StringSeq getAllAdapterIds(const ::Ice::Current&) const;

    virtual void addObject(const ::Ice::ObjectPrx&, const ::Ice::Current&);
    virtual void updateObject(const ::Ice::ObjectPrx&, const ::Ice::Current&);
    virtual void addObjectWithType(const ::Ice::ObjectPrx&, const ::std::string&, const ::Ice::Current&);
    virtual void removeObject(const ::Ice::Identity&, const ::Ice::Current&);
    virtual ObjectInfo getObjectInfo(const Ice::Identity&, const ::Ice::Current&) const;
    virtual ObjectInfoSeq getObjectInfosByType(const std::string&, const ::Ice::Current&) const;
    virtual ObjectInfoSeq getAllObjectInfos(const std::string&, const ::Ice::Current&) const;

    virtual NodeInfo getNodeInfo(const std::string&, const Ice::Current&) const;
    virtual bool pingNode(const std::string&, const Ice::Current&) const;
    virtual LoadInfo getNodeLoad(const std::string&, const Ice::Current&) const;
    virtual void shutdownNode(const std::string&, const Ice::Current&);
    virtual std::string getNodeHostname(const std::string&, const Ice::Current&) const;
    virtual Ice::StringSeq getAllNodeNames(const ::Ice::Current&) const;

    virtual void shutdown(const Ice::Current&);

    virtual Ice::SliceChecksumDict getSliceChecksums(const Ice::Current&) const;

private:

    const DatabasePtr _database;
    const RegistryIPtr _registry;
    const TraceLevelsPtr _traceLevels;
    const AdminSessionIPtr _session;
};

}

#endif
