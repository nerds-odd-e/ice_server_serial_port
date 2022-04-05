module Server
{
    interface SerialPort
    {
        string readSerialPort();
        void writeSerialPort(string message);
    };
};