//
// Created by Mingcan Li on 2/10/22.
//

#include "serial.h"

Serial::Serial() {}



Serial::Serial(const char* portName ) {
    serial.setPortName(portName);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    while(!serial.isOpen()) serial.open(QIODevice::ReadWrite);
    usleep(3000000);
}

void Serial::open(const char* portName) {
    serial.setPortName(portName);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    while(!serial.isOpen()) serial.open(QIODevice::ReadWrite);
    usleep(3000000);
}

/**
 * @brief Serial::sendMessage notify the thread by changing the bool value to send message
 * @param message
 */
void Serial::send(QByteArray Message) {
    if (serial.isOpen() && serial.isWritable()) {
        serial.write(Message);
        serial.flush();
        serial.waitForBytesWritten(1000);
    }
}


QByteArray Serial::read() {
    serial.waitForReadyRead(1000);
    return serial.readAll();
}

/**
 * @brief Serial::threadProcedure get data recieved from the serial port and put into buffer for access through getdata()
 */


SerialThread::SerialThread(const char* portName,const char* message, const char * fileName, bool idle) {
    this->portName=portName;
    this->message=message;
    this->fileName=fileName;
    idleThread=idle;
}

void SerialThread::run() {
    CSVWriter csv(fileName.c_str());
    csv.write("header\n");
    Serial serial(portName.c_str());
    serial.send(message.c_str());
    bool write=1;
    if(!idleThread) {
        runState =1;
        for(;;){
            QByteArray receive=serial.read();
            if(receive.contains("end") || receive.contains("error")){
                runState=2;
                emit ThreadTerminate();
                usleep(2000);

                break;
            }
            qDebug()<<receive;
            if(!receive.contains("yes"))
                csv.write(receive);
        }
    }
    serial.send("clear,");
    serial.send("clear,");
    while(!terminate) {
        runState=0;
        QByteArray receive=serial.read();
        if(receive.contains("ready")) {
            if(!batReady) {
                batReady=1;
                emit batteryStatusChange();
            }
        }

        else if(receive.contains("idle")) {
            if(batReady) {
                batReady=0;

            }
        }
        qDebug()<<receive;

        serial.send("ready,");
    }
}
