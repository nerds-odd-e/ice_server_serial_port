#ifndef EVICTORBASE_H
#define EVICTORBASE_H

#include <Ice/ServantLocator.h>
#include <IceUtil/Handle.h>
#include <IceUtil/Mutex.h>
#include <map>
#include <list>

class EvictorBase : virtual public Ice::ServantLocator
{
public:

    EvictorBase(int size = 1000);

    virtual Ice::ObjectPtr locate(const Ice::Current&, Ice::LocalObjectPtr&);
    virtual void finished(const Ice::Current&, const Ice::ObjectPtr&, const Ice::LocalObjectPtr&);
    virtual void deactivate(const std::string&);

    virtual Ice::ObjectPtr add(const Ice::Current&, Ice::LocalObjectPtr&) = 0;
    virtual void evict(const Ice::ObjectPtr&, const Ice::LocalObjectPtr&) = 0;

private:

    struct EvictorEntry;
    typedef IceUtil::Handle<EvictorEntry> EvictorEntryPtr;

    typedef std::map<Ice::Identity, EvictorEntryPtr> EvictorMap;
    typedef std::list<EvictorMap::iterator> EvictorQueue;

    struct EvictorEntry : public Ice::LocalObject
    {
	Ice::ObjectPtr servant;
	Ice::LocalObjectPtr userCookie;
	EvictorQueue::iterator pos;
	int useCount;
    };

    struct EvictorCookie : public Ice::LocalObject
    {
	EvictorEntryPtr entry;
    };
    typedef IceUtil::Handle<EvictorCookie> EvictorCookiePtr;

    EvictorMap _map;
    EvictorQueue _queue;
    Ice::Int _size;

    IceUtil::Mutex _mutex;

    void evictServants();
};

typedef IceUtil::Handle<EvictorBase> EvictorBasePtr;

#endif
