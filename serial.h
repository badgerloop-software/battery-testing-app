//
// Created by Mingcan Li on 2/10/22.
//

#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H

#include <stdio.h>
#include <string.h>

class Serial {
public:
    Serial(const char* portName);
    void send(const char* Message);
    void read(); //Still under construction..
private:
    FILE *port;
    const char* portName;
};


#endif //SERIAL_SERIAL_H
