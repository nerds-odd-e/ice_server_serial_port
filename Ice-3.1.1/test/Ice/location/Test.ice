// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TEST_ICE
#define TEST_ICE

#include <Ice/Locator.ice>

module Test
{

interface TestLocatorRegistry extends ::Ice::LocatorRegistry
{
    //
    // Allow remote addition of objects to the locator registry.
    //
    void addObject(Object* obj);
};

interface TestLocator extends ::Ice::Locator
{
    //
    // Returns the number of request on the locator interface.
    //
    nonmutating int getRequestCount();
};
    
interface ServerManager
{
    void startServer();
    void shutdown();
};

interface Hello
{
    void sayHello();
};

interface TestIntf
{
    void shutdown();

    Hello* getHello();

    Hello* getReplicatedHello();

    void migrateHello();
};

};

#endif
