//
// Created by Mingcan Li on 2/10/22.
//

#include "Serial.h"

Serial::Serial(const char* portName) {
    this->portName=portName;
}

void Serial::send(const char* Message) {
    FILE *serPort = fopen(portName, "w");

    if (serPort == NULL)
    {
        printf("ERROR");
        return;
    }

    fwrite(Message, sizeof(Message), sizeof(Message), serPort);
    fclose(serPort);
}

void Serial::read() {
    char readBuffer[1024];
    int numBytesRead;

    FILE *serPort = fopen(portName, "r");

    if (serPort == NULL)
    {
        printf("ERROR");
    }

    printf(portName);
    printf(":\n");
    while(1)
    {
        memset(readBuffer, 0, 1024);
        fread(readBuffer, sizeof(readBuffer),1024,serPort);
        if(sizeof(readBuffer) != 0)
        {
            printf(readBuffer);
        }
    }
}
