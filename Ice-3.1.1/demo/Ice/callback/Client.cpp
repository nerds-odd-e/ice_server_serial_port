// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Application.h>
#include <Callback.h>

using namespace std;
using namespace Demo;

class CallbackReceiverI : public CallbackReceiver
{
public:

    virtual void callback(const Ice::Current&)
    {
	cout << "received callback" << endl;
    }
};

class CallbackClient : public Ice::Application
{
public:

    virtual int run(int, char*[]);

private:

    void menu();
};

int
main(int argc, char* argv[])
{
    CallbackClient app;
    return app.main(argc, argv, "config.client");
}

int
CallbackClient::run(int argc, char* argv[])
{
    Ice::PropertiesPtr properties = communicator()->getProperties();
    const char* proxyProperty = "Callback.Client.CallbackServer";
    std::string proxy = properties->getProperty(proxyProperty);
    if(proxy.empty())
    {
	cerr << appName() << ": property `" << proxyProperty << "' not set" << endl;
	return EXIT_FAILURE;
    }

    CallbackSenderPrx twoway = CallbackSenderPrx::checkedCast(
	communicator()->stringToProxy(proxy)->ice_twoway()->ice_timeout(-1)->ice_secure(false));
    if(!twoway)
    {
	cerr << appName() << ": invalid proxy" << endl;
	return EXIT_FAILURE;
    }
    CallbackSenderPrx oneway = CallbackSenderPrx::uncheckedCast(twoway->ice_oneway());
    CallbackSenderPrx batchOneway = CallbackSenderPrx::uncheckedCast(twoway->ice_batchOneway());
    CallbackSenderPrx datagram = CallbackSenderPrx::uncheckedCast(twoway->ice_datagram());
    CallbackSenderPrx batchDatagram = CallbackSenderPrx::uncheckedCast(twoway->ice_batchDatagram());
    
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Callback.Client");
    adapter->add(new CallbackReceiverI, communicator()->stringToIdentity("callbackReceiver"));
    adapter->activate();

    CallbackReceiverPrx twowayR = CallbackReceiverPrx::uncheckedCast(
	adapter->createProxy(communicator()->stringToIdentity("callbackReceiver")));
    CallbackReceiverPrx onewayR = CallbackReceiverPrx::uncheckedCast(twowayR->ice_oneway());
    CallbackReceiverPrx datagramR = CallbackReceiverPrx::uncheckedCast(twowayR->ice_datagram());

    bool secure = false;
    string secureStr = "";

    menu();

    char c;
    do
    {
	try
	{
	    cout << "==> ";
	    cin >> c;
	    if(c == 't')
	    {
		twoway->initiateCallback(twowayR);
	    }
	    else if(c == 'o')
	    {
		oneway->initiateCallback(onewayR);
	    }
	    else if(c == 'O')
	    {
		batchOneway->initiateCallback(onewayR);
	    }
	    else if(c == 'd')
	    {
                if(secure)
                {
                    cout << "secure datagrams are not supported" << endl;
                }
                else
                {
                    datagram->initiateCallback(datagramR);
                }
	    }
	    else if(c == 'D')
	    {
                if(secure)
                {
                    cout << "secure datagrams are not supported" << endl;
                }
                else
                {
                    batchDatagram->initiateCallback(datagramR);
                }
	    }
	    else if(c == 'f')
	    {
		communicator()->flushBatchRequests();
	    }
	    else if(c == 'S')
	    {
		secure = !secure;
		secureStr = secure ? "s" : "";
		
		twoway = CallbackSenderPrx::uncheckedCast(twoway->ice_secure(secure));
		oneway = CallbackSenderPrx::uncheckedCast(oneway->ice_secure(secure));
		batchOneway = CallbackSenderPrx::uncheckedCast(batchOneway->ice_secure(secure));
		datagram = CallbackSenderPrx::uncheckedCast(datagram->ice_secure(secure));
		batchDatagram = CallbackSenderPrx::uncheckedCast(batchDatagram->ice_secure(secure));

		twowayR = CallbackReceiverPrx::uncheckedCast(twowayR->ice_secure(secure));
		onewayR = CallbackReceiverPrx::uncheckedCast(onewayR->ice_secure(secure));
		datagramR = CallbackReceiverPrx::uncheckedCast(datagramR->ice_secure(secure));
		
		if(secure)
		{
		    cout << "secure mode is now on" << endl;
		}
		else
		{
		    cout << "secure mode is now off" << endl;
		}
	    }
	    else if(c == 's')
	    {
		twoway->shutdown();
	    }
	    else if(c == 'x')
	    {
		// Nothing to do
	    }
	    else if(c == '?')
	    {
		menu();
	    }
	    else
	    {
		cout << "unknown command `" << c << "'" << endl;
		menu();
	    }
	}
	catch(const Ice::Exception& ex)
	{
	    cerr << ex << endl;
	}
    }
    while(cin.good() && c != 'x');

    return EXIT_SUCCESS;
}

void
CallbackClient::menu()
{
    cout <<
	"usage:\n"
	"t: send callback as twoway\n"
	"o: send callback as oneway\n"
	"O: send callback as batch oneway\n"
	"d: send callback as datagram\n"
	"D: send callback as batch datagram\n"
	"f: flush all batch requests\n"
	"S: switch secure mode on/off\n"
	"s: shutdown server\n"
	"x: exit\n"
	"?: help\n";
}
