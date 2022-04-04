#include <iostream>
#include <Ice/Ice.h>
#include <Server.h>
#include <string>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

using namespace std;
using namespace Server;

class SerialPortI : public SerialPort
{
public:
    virtual ::std::string readSerialPort(const ::Ice::Current& = ::Ice::Current());
};

::std::string SerialPortI::readSerialPort(const ::Ice::Current&)
{
    int serial_port = open("/dev/pts/0", O_RDONLY | O_NOCTTY);

    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }

    char read_buf [2048];

    int n = read(serial_port, &read_buf, sizeof(read_buf));

    if (n > 0) {
        return string(read_buf);
    } else {
        return "No data";
    }
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
