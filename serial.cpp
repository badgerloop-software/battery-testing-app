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
void Serial::sendMessage(QByteArray message) {
    //qDebug()<<"set";
    newMessage=1;
    toSend = message;
}

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
void Serial::threadProcedure() {
    for(;;){
            if(newMessage) {
                //qDebug()<<"send";
                send(toSend);
                newMessage =0;
            }
            QByteArray recieve=read();
            if(recieve=="end") {
                terminate = 1;
                return;
            }
            buffer.append(recieve);
            //qDebug()<<read();
        }
}

void Serial::start() {
    t=new std::thread(&Serial::threadProcedure,this);
    t->detach();
}

/**
 * @brief Serial::getData returns the buffered data and clear the buffer
 * @return
 */
QByteArray Serial::getData() {
    QByteArray tmp= buffer;
    buffer.clear();
    return tmp;
}
