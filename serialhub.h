#ifndef SERIALHUB_H
#define SERIALHUB_H

#include "serial.h"
#include "csvwriter.h"
#include <vector>
#include <thread>
#include <algorithm>
#include <unistd.h>


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
};

class SerialHub : public QObject{
    Q_OBJECT
public:
    SerialHub();
    void deploy(const char* port, const char* testerID, const char* batteryID, int current);
    void procedure();
    deviceInfo getDeviceConfig(const char* port);
public slots:
    void on_ThreadTerminate();
private:
    std::vector<SerialThread*> devices;
    std::vector<deviceInfo> deviceList;
};

#endif // SERIALHUB_H
