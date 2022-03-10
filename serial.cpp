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
    csv.open(fileName.c_str());
    csv.write("header\n");
    Serial serial(portName.c_str());
    serial.send(message.c_str());
    /*bool write=1;
    if(!idleThread) {
        dischargeState(&serial);
    }
    serial.send("clear,");
    serial.send("clear,");
    idleState(&serial);*/
    if(idleThread) {
        idleState(&serial);
    } else {
        chargeState(&serial);
    }
}



void SerialThread::idleState(Serial *serial){
    while(!terminate) {
        runState=0;
        QByteArray receive=serial->read();
        if(receive.contains("ready")) {
            readyState(serial);
            break;
        }

        else if(receive.contains("idle")) {
            if(batReady) {
                batReady=0;
            }
        }

        qDebug()<<"idle func "<<receive;

        serial->send("ready,");
    }
}


void SerialThread::readyState(Serial *serial) {
    batReady=1;
    emit batteryStatusChange();
    while(!terminate) {
        QByteArray receive=serial->read();
        qDebug()<<"ready func "<<receive;
    }
}

void SerialThread::dischargeState(Serial *serial) {
    runState =1;
    while(!terminate){
        QByteArray receive=serial->read();
        if(receive.contains("end")){
            runState=2;
            emit ThreadTerminate();
            //usleep(2000);
            idleState(serial);
            break;
        }
        qDebug()<<"discharge func "<<receive;
    }
}

void SerialThread::chargeState(Serial *serial) {
    while(!terminate){
        QByteArray receive=serial->read();
        if(receive.contains("discharge")){
            dischargeState(serial);
            break;
        }
        if(receive.contains("error")){
            errorState(message.c_str(),serial);
        }
        qDebug()<<"charge func "<<receive;
    }
}

void SerialThread::errorState(QString message, Serial *serial) {
    QMessageBox err;
    err.setText(message);
    err.setStandardButtons(QMessageBox::Cancel
                              |QMessageBox::Retry);

    int result = err.exec();
    if(result==QMessageBox::Cancel) {
        idleState(serial);
    }

    if(result==QMessageBox::Retry) {
        return;
    }
}


