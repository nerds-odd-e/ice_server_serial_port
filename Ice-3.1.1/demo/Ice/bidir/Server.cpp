// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <CallbackI.h>
#include <Ice/Application.h>

using namespace std;
using namespace Demo;

class CallbackServer : public Ice::Application
{
public:

    virtual int run(int, char*[]);
};

int
main(int argc, char* argv[])
{
    CallbackServer app;
    return app.main(argc, argv, "config.server");
}

int
CallbackServer::run(int argc, char* argv[])
{
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Callback.Server");
    CallbackSenderIPtr sender = new CallbackSenderI(communicator());
    adapter->add(sender, communicator()->stringToIdentity("sender"));
    adapter->activate();

    sender->start();
    try
    {
	communicator()->waitForShutdown();
    }
    catch(...)
    {
	sender->destroy();
	throw;
    }
    sender->destroy();

    return EXIT_SUCCESS;
}
