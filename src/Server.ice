module Server
{
    interface SerialPort
    {
        string readSerialPort();
        void writeSerialPort(string message);
    };

    interface Driver
    {
        string readInfoFromDevice();
    };
};
