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

class Serial {
public:
    Serial(const char* portName);
    void send(QByteArray Message);
    QByteArray read();
private:
    FILE *port;
    const char* portName;
    QSerialPort serial;
};

#endif //SERIAL_SERIAL_H

