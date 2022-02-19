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

    Serial newSerial(port);
    newSerial.start();

    std::string message = "startTest,";
    message+=(device.discharge_current);
    message+=",";
    message+=device.testerID;
    newSerial.sendMessage(message.c_str());

    devices.push_back(&newSerial);
    /*SerialHub *serialHub;
    serialHub->moveToThread(&thread);
    connect(&thread,&QThread::finished,serialHub,&QObject::deleteLater);
    thread.start();*/
}

/**
 * @brief SerialHub::procedure procedure to run the program
 */
void SerialHub::procedure() {
    for(int i = 0;i<deviceList.size(); i ++){
        QByteArray toSend;
        toSend.setNum(i);
        devices[i]->sendMessage(toSend);
        qDebug()<<devices[i]->getData();
        usleep(3000000);
    }
}



