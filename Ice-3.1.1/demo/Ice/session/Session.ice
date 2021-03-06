// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef SESSION_ICE
#define SESSION_ICE

module Demo
{

interface Hello
{
    nonmutating void sayHello();
};

//
// The session object. This is used to create per-session objects on
// behalf of the client. If the session is not refreshed on a periodic
// basis, it will be automatically destroyed.
//
interface Session
{
    //
    // Create a new per-session hello object. The created object will
    // be automatically destroyed when the session is destroyed.
    //
    Hello* createHello();

    //
    // Refresh a session. If a session is not refreshed on a regular
    // basis by the client, it will be automatically destroyed.
    //
    idempotent void refresh();

    nonmutating string getName();

    //
    // Destroy the session explicitly.
    //
    void destroy();
};

interface SessionFactory
{
    //
    // Create a session with the given name. Note that the name is
    // only used for diagnostic purposes. It is not used as unique
    // session id.
    //
    Session* create(string name);

    idempotent void shutdown();
};

};

#endif
