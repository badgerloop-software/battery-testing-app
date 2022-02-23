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
public:
    SerialThread(const char* portName) {this->portName=portName;};
    QByteArray getData();
    bool terminate=0;
private:
    const char* portName;
    void run() override;
};

#endif //SERIAL_SERIAL_H

