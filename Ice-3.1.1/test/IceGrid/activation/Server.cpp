// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <TestI.h>

using namespace std;

class Server : public Ice::Application
{
public:

    virtual int run(int argc, char* argv[]);

};

int
Server::run(int argc, char* argv[])
{
    Ice::StringSeq args = Ice::argsToStringSeq(argc, argv);
    Ice::PropertiesPtr properties = communicator()->getProperties();
    args = properties->parseCommandLineOptions("", args);
    Ice::stringSeqToArgs(args, argc, argv);
    if(properties->getPropertyAsInt("FailOnStartup") > 0)
    {
	return EXIT_FAILURE;
    }

    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("TestAdapter");
    TestI* test = new TestI(adapter);
    Ice::ObjectPtr obj = test;
    adapter->add(test, communicator()->stringToIdentity(properties->getProperty("Ice.ServerId")));

    int delay = properties->getPropertyAsInt("ActivationDelay");
    if(delay > 0)
    {
	IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(delay));
    }

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

    delay = properties->getPropertyAsInt("DeactivationDelay");
    if(delay > 0)
    {
	IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(delay));
    }

    return test->isFailed() ? EXIT_FAILURE : EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    Server app;
    int rc = app.main(argc, argv);
    return rc;
}
