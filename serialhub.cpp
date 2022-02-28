#include "serialhub.h"

SerialHub::SerialHub() {

    for(QSerialPortInfo port: QSerialPortInfo::availablePorts()){
        Serial serial(("/dev/"+port.portName().toStdString()).c_str());
        serial.send("tester?\n");
        if(serial.read().contains("yes")){
            deviceList.push_back(deviceInfo(("/dev/"+port.portName().toStdString()).c_str(),"","",0));
            qDebug()<<port.portName();
        }
    }
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



    std::string toSend="starttest,";
    toSend+=testerID;
    toSend+=",";
    toSend+=batteryID;
    toSend+=",";
    toSend+=current;
    devices.push_back(new SerialThread(port,toSend.c_str(),batteryID));
    devices[devices.size()-1]->start();
    QObject::connect(&*devices[devices.size()-1],&SerialThread::ThreadTerminate
            ,this,&SerialHub::on_ThreadTerminate);
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

void SerialHub::on_ThreadTerminate(){
    for (int i = 0 ; i < devices.size() ; i++){
        if(devices[i]->terminate){
            devices.erase(devices.begin()+i);
            deviceList.erase(deviceList.begin()+i);
        }
    }
}


