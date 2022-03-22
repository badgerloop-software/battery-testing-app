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
#include <QThread>
#include <QDebug>
#include <unistd.h>
#include <QMessageBox>

#define IDLE 0
#define READY 1
#define CHARGE 2
#define DISCHARGE 3
#define ERROR 4

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
    SerialThread(const char* portName);

    bool batReady=0;
public slots:
    void handleErrorDecision(std::string port, int prevState, int decision);
    void startTest(std::string port, const char* batteryID, double dischargeCurrent);
signals:
    void batteryStatusChange();
    void testEnded(std::string port);
    void error(std::string port, QString message, int prevState);
private:
    int idleState();
    int readyState();
    int chargeState();
    int dischargeState();
    void run() override;
    int currState;
    double current;
    std::string portName;
    std::string fileName;
    CSVWriter *csv;
    Serial *serial;
};


#endif //SERIAL_SERIAL_H

