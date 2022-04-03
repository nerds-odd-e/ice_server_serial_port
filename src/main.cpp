#include <iostream>
#include <Ice/Ice.h>
#include <Server.h>
#include <string>

using namespace std;
using namespace Server;

class SerialPortI : public SerialPort
{
public:
    virtual ::std::string readSerialPort(const ::Ice::Current& = ::Ice::Current());
};

::std::string SerialPortI::readSerialPort(const ::Ice::Current&)
{
    return "Hello world!";
}

int main(int argc, char* argv[]) {
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter
                = ic->createObjectAdapterWithEndpoints(
                        "ServerAdapter", "default -p 10000");
        Ice::ObjectPtr object = new SerialPortI();
        adapter->add(object, ic->stringToIdentity("SerialPort")
        );
        adapter->activate();
        ic->waitForShutdown();
    } catch (const Ice::Exception &e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }
    return status;
}
