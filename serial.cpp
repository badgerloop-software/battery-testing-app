//
// Created by Mingcan Li on 2/10/22.
//

#include "serial.h"

Serial::Serial() {}



Serial::Serial(const char* portName) {
    serial.setPortName(portName);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    while(!serial.isOpen()) serial.open(QIODevice::ReadWrite);
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


void SerialThread::run() {

    Serial serial(portName.c_str());
    serial.send(message.c_str());
    serial.send(message.c_str());
    serial.send(message.c_str());
    serial.send(message.c_str());
    serial.send(message.c_str());
    /*serial.send("abc");
    serial.send("abc");
    serial.send("abc");
    serial.send("abc");
    serial.send("abc");*/
    for(;;){
            QByteArray receive=serial.read();
            if(receive=="end"){
                //emit ThreadTerminate();
                return;
            }

            //do stuff here
            qDebug()<<receive;
    }
}
