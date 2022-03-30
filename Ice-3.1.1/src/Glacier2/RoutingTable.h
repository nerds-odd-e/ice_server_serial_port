// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_ROUTING_TABLE_H
#define ICE_ROUTING_TABLE_H

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <list>
#include <Glacier2/ProxyVerifier.h>

namespace Glacier2
{

class RoutingTable;
typedef IceUtil::Handle<RoutingTable> RoutingTablePtr;

class RoutingTable : public IceUtil::Shared, public IceUtil::Mutex
{
public:

    RoutingTable(const Ice::CommunicatorPtr&);

    Ice::ObjectProxySeq add(const Ice::ObjectProxySeq&, const Ice::Current&); // Returns evicted proxies.
    Ice::ObjectPrx get(const Ice::Identity&); // Returns null if no proxy can be found.

private:

    const Ice::CommunicatorPtr _communicator;
    const int _traceLevel;
    const int _maxSize;

    struct EvictorEntry;
    typedef IceUtil::Handle<EvictorEntry> EvictorEntryPtr;

    typedef std::map<Ice::Identity, EvictorEntryPtr> EvictorMap;
    typedef std::list<EvictorMap::iterator> EvictorQueue;

    friend struct EvictorEntry;
    struct EvictorEntry : public IceUtil::Shared
    {
	Ice::ObjectPrx proxy;
	EvictorQueue::iterator pos;
    };

    EvictorMap _map;
    EvictorQueue _queue;

    ProxyVerifier _verifier; 
};

}

#endif
