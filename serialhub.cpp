#include "serialhub.h"

SerialHub::SerialHub() {

    for(QSerialPortInfo port: QSerialPortInfo::availablePorts()){
        if(port.portName().contains("cu")){
            Serial serial(("/dev/"+port.portName().toStdString()).c_str());
            serial.send("tester,\n");
            QByteArray reply=serial.read();
            if(reply.contains("yes")){
                QStringList testID=QString(reply).split(",");
                deviceList.push_back(deviceInfo(("/dev/"+port.portName().toStdString()).c_str(),testID[1].toStdString(),"",0));
                devices.push_back(new SerialThread(("/dev/"+port.portName().toStdString()).c_str(),"","",1));
                devices[devices.size()-1]->start();
                QObject::connect(&*devices[devices.size()-1],&SerialThread::batteryStatusChange
                        ,this,&SerialHub::on_batteryStatusChange);
                QObject::connect(&*devices[devices.size()-1],&SerialThread::ThreadTerminate
                        ,this,&SerialHub::on_threadTerminate);
                qDebug()<<port.portName();
            }
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




    std::string toSend="starttest,";
    toSend+=testerID;
    toSend+=",";
    toSend+=batteryID;
    toSend+=",";
    toSend+=current;

    int index = getIndex(testerID);
    devices[index]->terminateThread();
    deviceList[index]=deviceInfo(port,testerID, batteryID, current);
    deviceList[index].batReady=1;
    deviceList[index].running=1;

    devices[index]=new SerialThread(port,toSend.c_str(),batteryID,0);
    QObject::connect(&*devices[index],&SerialThread::batteryStatusChange
            ,this,&SerialHub::on_batteryStatusChange);
    QObject::connect(&*devices[index],&SerialThread::ThreadTerminate
            ,this,&SerialHub::on_threadTerminate);
    devices[index]->start();
}

int SerialHub::getIndex(std::string testerID) {
    for(int i = 0 ; i < deviceList.size() ; i ++) {
        if(deviceList[i].testerID==testerID) {
            return i;
        }
    }
    return -1;
}

deviceInfo SerialHub::getDeviceConfig(std::string testerID) {
    for(int i = 0 ; i < deviceList.size() ; i++){
        if(deviceList[i].testerID==testerID) {
            return deviceList[i];
        }
    }
    deviceInfo device;
    device.batteryID="DNE";
    device.testerID="";
    device.discharge_current=0;
    return device;
}

void SerialHub::on_batteryStatusChange() {
    qDebug()<<"called";
    for(int i = 0 ; i < devices.size() ; i ++) {
        deviceList[i].batReady=devices[i]->batReady;
        if(devices[i]->batReady){
            qDebug()<<i;
        }
    }
    emit statusChange();
}

void SerialHub::on_threadTerminate() {
    for (int i = 0 ; i < devices.size() ; i++){
        if(devices[i]->runState==2){
            deviceList[i].running=0;
            deviceList[i].batteryID="";
            deviceList[i].discharge_current=0;
            deviceList[i].batReady=devices[i]->batReady;
        }

    }
    emit statusChange();
}

