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
    const char* devicePort;
    const char* testerID;
    const char* deviceID;
    int discharge_current;
};

class SerialHub{
public:
    SerialHub();
    void deploy(const char* port, const char* testerID, const char* deviceID, int current);
    void procedure();
private:
    std::vector<SerialThread*> devices;
    std::vector<deviceInfo> deviceList;
};

#endif // SERIALHUB_H
