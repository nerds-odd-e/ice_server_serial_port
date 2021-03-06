// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/DisableWarnings.h>
#include <Ice/Ice.h>
#include <TestI.h>
#include <TestCommon.h>

using namespace std;

class Server : public Ice::Application
{
public:

    virtual int run(int argc, char* argv[]);
};

int
Server::run(int argc, char* argv[])
{
    Ice::PropertiesPtr properties = communicator()->getProperties();

    Ice::StringSeq args = Ice::argsToStringSeq(argc, argv);
    args = properties->parseCommandLineOptions("Test", args);
    Ice::stringSeqToArgs(args, argc, argv);

    string name = properties->getProperty("Ice.ProgramName");

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Server");
    Ice::ObjectPtr object = new TestI(adapter, properties);
    adapter->add(object, communicator()->stringToIdentity(name));

    shutdownOnInterrupt();
    try
    {
        adapter->activate();
    }
    catch(const Ice::ObjectAdapterDeactivatedException&)
    {
    }
    communicator()->waitForShutdown();
    ignoreInterrupt();
    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    //
    // Test if MY_ENV_VARIABLE is set.
    //
    char* value = getenv("MY_ENV_VARIABLE");
    test(value != 0 && string(value) == "12");

    Server app;
    int rc = app.main(argc, argv);
    return rc;
}
