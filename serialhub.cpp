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
                              const char* batteryID, int current){

    /*deviceInfo device;
    device.devicePort=port;
    device.batteryID=batteryID;*/
    deviceList.push_back(deviceInfo(port,testerID, batteryID, current));



    std::string toSend="StartTest,";
    toSend+=testerID;
    toSend+=",";
    toSend+=batteryID;
    toSend+=",";
    toSend+=current;
    devices.push_back(new SerialThread(port,toSend.c_str()));
    devices[devices.size()-1]->start();
}

deviceInfo SerialHub::getDeviceConfig(const char *port) {
    for(int i = 0 ; i < deviceList.size() ; i++){
        if(deviceList[i].devicePort==port) {
            return deviceList[i];
        }
    }
    deviceInfo device;
    device.batteryID="DNE";
    device.testerID="";
    device.discharge_current=0;
    return device;
}
/**
 * @brief SerialHub::procedure procedure to run the program
 */
void SerialHub::procedure() {
    //devices[devices.size()-1]->sendMessage("startTest,123,321,123");
    /*QByteArray bytes=devices[0]->getData();
    if(bytes!="")
        qDebug()<<bytes;*/
}



