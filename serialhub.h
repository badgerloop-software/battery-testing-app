#ifndef SERIALHUB_H
#define SERIALHUB_H

#include "serial.h"
#include "csvwriter.h"
#include <vector>
#include <thread>
#include <algorithm>
#include <unistd.h>
#include <QSerialPortInfo>
#include <QDebug>


struct deviceInfo : public QObject{
    Q_OBJECT
public:
    deviceInfo(){}
    deviceInfo(std::string port,
               std::string testerID,
               std::string batteryID,
               double current){
        this->devicePort=port;
        this->testerID=testerID;
        this->batteryID=batteryID;
        this->discharge_current = current;
    }
    std::string devicePort;
    std::string testerID;
    std::string batteryID;
    double discharge_current;
    bool running=false;
    bool batReady=false;
    bool error=false;
    int testerState = IDLE;
    QString errorMessage;
    QString errorTitle;
};

class SerialHub : public QObject{
    Q_OBJECT
public:
    SerialHub();
    void startTest(const char* testerID, const char* batteryID, double current);
    deviceInfo* getDeviceConfig(std::string testerID);
    std::vector<deviceInfo*> getDevices(){return deviceInfoList;};
public slots:
    void on_testerStateChange(int newState, std::string port);
    void on_testEnded(std::string port);
    void on_error(std::string port, QString message, int prevState);
    void on_voltageChange(std::string port, QString voltage) { emit propagateVoltageChange(port, voltage); }; // TODO
signals:
    void statusChange();
    void signalStartTest(std::string port, const char* batteryID, double current);
    void errorDecisionMade(std::string port, int prevState, int decision);
    void propagateVoltageChange(std::string port, QString voltage); // TODO
    void testerMovedToReady(std::string port); // TODO Tester Ready Mouse
private:
    int getIndex(std::string testerID);
    std::vector<SerialThread*> devices;
    std::vector<deviceInfo*> deviceInfoList;
};


#endif // SERIALHUB_H
