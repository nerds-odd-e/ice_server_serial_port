// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_GRID_SERVERCACHE_H
#define ICE_GRID_SERVERCACHE_H

#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceGrid/Descriptor.h>
#include <IceGrid/Internal.h>
#include <IceGrid/Query.h>
#include <IceGrid/Allocatable.h>
#include <IceGrid/Cache.h>

namespace IceGrid
{

class ServerCache;
class ObjectCache;
class AdapterCache;
class AllocatableObjectCache;
class NodeCache;

class NodeEntry;
typedef IceUtil::Handle<NodeEntry> NodeEntryPtr;

class ServerEntry : public Allocatable
{
public:
    
    ServerEntry(ServerCache&, const std::string&);

    void sync();
    void update(const ServerInfo&);
    void destroy();

    ServerInfo getServerInfo(bool = false) const;
    std::string getId() const;

    ServerPrx getProxy(int&, int&, std::string&, bool);
    AdapterPrx getAdapter(const std::string&, bool);
    NodeEntryPtr getNode() const;
    std::string getApplication() const;
    float getLoad(LoadSample) const;

    bool canRemove();
    bool isDestroyed();
    
    void loadCallback(const ServerPrx&, const AdapterPrxDict&, int, int);
    void destroyCallback();
    void exception(const Ice::Exception&);

    virtual void allocated(const SessionIPtr&);
    virtual void released(const SessionIPtr&);
    virtual bool release(const SessionIPtr&, bool);

private:
    
    void syncImpl(bool);

    ServerCache& _cache;
    const std::string _id;
    std::auto_ptr<ServerInfo> _loaded;
    std::auto_ptr<ServerInfo> _load;
    std::auto_ptr<ServerInfo> _destroy;

    ServerPrx _proxy;
    AdapterPrxDict _adapters;
    int _activationTimeout;
    int _deactivationTimeout;

    bool _synchronizing;
    bool _updated;
    std::auto_ptr<Ice::Exception> _exception;

    SessionIPtr _session;
};
typedef IceUtil::Handle<ServerEntry> ServerEntryPtr;
typedef std::vector<ServerEntryPtr> ServerEntrySeq;

class ServerCache : public CacheByString<ServerEntry>
{
public:

    ServerCache(const Ice::CommunicatorPtr&, NodeCache&, AdapterCache&, ObjectCache&, AllocatableObjectCache&);

    ServerEntryPtr add(const ServerInfo&);
    ServerEntryPtr get(const std::string&) const;
    bool has(const std::string&) const;
    ServerEntryPtr remove(const std::string&, bool = true);

    void clear(const std::string&);
    
    NodeCache& getNodeCache() const { return _nodeCache; }
    Ice::CommunicatorPtr getCommunicator() const { return _communicator; }
    
private:
    
    void addCommunicator(const CommunicatorDescriptorPtr&, const ServerEntryPtr&);
    void removeCommunicator(const CommunicatorDescriptorPtr&, const ServerEntryPtr&);

    friend struct AddCommunicator;
    friend struct RemoveCommunicator;

    const Ice::CommunicatorPtr _communicator;
    NodeCache& _nodeCache;
    AdapterCache& _adapterCache;
    ObjectCache& _objectCache;
    AllocatableObjectCache& _allocatableObjectCache;
};

};

#endif
