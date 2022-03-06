//
// Created by Mingcan Li on 2/10/22.
//

#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H

#include "csvwriter.h"
#include <stdio.h>
#include <string.h>
#include <QCoreApplication>
#include <iostream>
#include <QSerialPort>
#include <QDebug>
#include <unistd.h>

class Serial {
public:
    Serial(const char* portName);
    Serial();
    void open(const char* portName);
    void send(QByteArray Message);
    QByteArray read();
private:
    QSerialPort serial;
};


class SerialThread : public QThread {
    Q_OBJECT
public:
    SerialThread(const char* portName,const char* message, const char * fileName, bool idle);
    //QByteArray getData();
    void terminateThread(){terminate=1;}
    bool batReady=0;
    int runState; //0-idle, 1 running, 2 finished
    bool idleThread;
signals:
    void ThreadTerminate();
    void batteryStatusChange();
private:
    bool terminate=0;

    std::string message;
    std::string portName;
    std::string fileName;
    void run() override;
};

#endif //SERIAL_SERIAL_H

