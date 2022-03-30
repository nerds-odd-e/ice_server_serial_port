// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>

#include <StaticMutexTest.h>
#include <TestCommon.h>

using namespace std;
using namespace IceUtil;

static const string mutexTestName("static mutex");

static StaticMutex staticMutex = ICE_STATIC_MUTEX_INITIALIZER;

class StaticMutexTestThread : public Thread
{
public:
    
    StaticMutexTestThread() :
	_tryLock(false)
    {
    }

    virtual void run()
    {	
	StaticMutex::TryLock tlock(staticMutex);
	test(!tlock.acquired());

	{
	    Mutex::Lock lock(_tryLockMutex);
	    _tryLock = true;
	}
	_tryLockCond.signal();

	StaticMutex::Lock lock(staticMutex);
    }

    void
    waitTryLock()
    {
	Mutex::Lock lock(_tryLockMutex);
	while(!_tryLock)
	{
	    _tryLockCond.wait(lock);
	}
    }

private:

    bool _tryLock;
    //
    // Use native Condition variable here, not Monitor.
    //
    Cond _tryLockCond;
    Mutex _tryLockMutex;
};

typedef Handle<StaticMutexTestThread> StaticMutexTestThreadPtr;

StaticMutexTest::StaticMutexTest() :
    TestBase(mutexTestName)
{
}

void
StaticMutexTest::run()
{
    StaticMutexTestThreadPtr t;
    ThreadControl control;

    {
	StaticMutex::Lock lock(staticMutex);

	// LockT testing: 
	//

	test(lock.acquired());

	try
	{
	    lock.acquire();
	    test(false);
	}
	catch(const ThreadLockedException&)
	{
	    // Expected
	}

	try
	{
	    lock.tryAcquire();
	    test(false);
	}
	catch(const ThreadLockedException&)
	{
	    // Expected
	}

	test(lock.acquired());
	lock.release();
	test(!lock.acquired());

	try
	{
	    lock.release();
	    test(false);
	}
	catch(const ThreadLockedException&)
	{
	    // Expected
	}
	
	StaticMutex::TryLock lock2(staticMutex);
	try
	{
	    test(lock.tryAcquire() == false);
	}
	catch(const ThreadLockedException&)
	{
	}
	lock2.release();
	test(lock.tryAcquire() == true);
	test(lock.acquired());	

	// TEST: Start thread, try to acquire the mutex.
	t = new StaticMutexTestThread;
	control = t->start();
	
	// TEST: Wait until the tryLock has been tested.
	t->waitTryLock();
    }

    //
    // TEST: Once the mutex has been released, the thread should
    // acquire the mutex and then terminate.
    //
    control.join();
}
