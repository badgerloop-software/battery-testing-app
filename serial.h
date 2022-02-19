//
// Created by Mingcan Li on 2/10/22.
//

#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H

#include <stdio.h>
#include <string.h>
#include <QCoreApplication>
#include <iostream>
#include <QSerialPort>
#include <QDebug>
#include <unistd.h>

class Serial :public QObject{
    Q_OBJECT
public:
    Serial(const char* portName);
    Serial();
    void open(const char* portName);
    void sendMessage(QByteArray message);
    bool terminate=0;
    void start();
    QByteArray getData();

private:
    void send(QByteArray Message);
    QByteArray read();
    bool newMessage=0;
    QByteArray toSend=NULL;

    void threadProcedure();
    QByteArray buffer;
    std::thread *t;
    const char* portName;
    QSerialPort serial;
};

#endif //SERIAL_SERIAL_H

