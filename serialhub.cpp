#include "serialhub.h"

SerialHub::SerialHub() {

}

/**
 * @brief SerialHub::deploy create a new serial object and add to the vector
 * @param port
 * @param testerID
 * @param deviceID
 * @param current
 */
void SerialHub::deploy(const char* port,
                              const char* testerID,
                              const char* deviceID, int current){

    deviceInfo device;
    device.devicePort=port;
    device.deviceID=deviceID;
    deviceList.push_back(device);

    devices.push_back(new SerialThread("/dev/cu.usbserial-2110"));

    std::string toSend="StartTest,";
    toSend+=testerID;
    toSend+=",";
    toSend+=deviceID;
    toSend+=",";
    toSend+=current;
    devices[devices.size()-1]->sendMessage("startTest,123,321,123");
    devices[devices.size()-1]->start();

    //qDebug()<<devices[0]->getData();
    //devices[0]->sendMessage("lll");
}

/**
 * @brief SerialHub::procedure procedure to run the program
 */
void SerialHub::procedure() {
    devices[devices.size()-1]->sendMessage("startTest,123,321,123");
    QByteArray bytes=devices[0]->getData();
    if(bytes!="")
        qDebug()<<bytes;
}



