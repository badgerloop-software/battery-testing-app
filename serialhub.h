#ifndef SERIALHUB_H
#define SERIALHUB_H

#include "serial.h"
#include "csvwriter.h"
#include <vector>
#include <thread>
#include <algorithm>
#include <unistd.h>
#include <QSerialPortInfo>

struct deviceInfo{
    deviceInfo(){}
    deviceInfo(std::string port,
               std::string testerID,
               std::string batteryID, int current){
        this->devicePort=port;
        this->testerID=testerID;
        this->batteryID=batteryID;
        this->discharge_current = current;
    }
    std::string devicePort;
    std::string testerID;
    std::string batteryID;
    int discharge_current;
    bool running=0;
    bool batReady;
};

class SerialHub : public QObject{
    Q_OBJECT
public:
    SerialHub();
    void deploy(const char* port, const char* testerID, const char* batteryID, int current);
    deviceInfo getDeviceConfig(std::string testerID);
    std::vector<deviceInfo> getDevices(){return deviceList;};
public slots:
    void on_batteryStatusChange();
    void on_threadTerminate();
signals:
    void statusChange();
private:
    int getIndex(std::string testerID);
    std::vector<SerialThread*> devices;
    std::vector<deviceInfo> deviceList;
};

#endif // SERIALHUB_H
