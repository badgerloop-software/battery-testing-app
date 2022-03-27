//
// Created by Mingcan Li on 2/10/22.
//

#include "serial.h"

Serial::Serial() {}

Serial::Serial(const char* portName ) {
    serial.setPortName(portName);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    while(!serial.isOpen()) serial.open(QIODevice::ReadWrite);
    usleep(3000000);
}

void Serial::open(const char* portName) {
    serial.setPortName(portName);
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    while(!serial.isOpen()) serial.open(QIODevice::ReadWrite);
    usleep(3000000);
}

/**
 * @brief Serial::sendMessage Send serial message
 * @param message Message to send
 */
void Serial::send(QByteArray Message) {
    if (serial.isOpen() && serial.isWritable()) {
        serial.write(Message);
        serial.flush();
        serial.waitForBytesWritten(1000);
    }
}

QByteArray Serial::read() {
    serial.waitForReadyRead(1000);
    return serial.readAll();
}


//-------------------------------------------------------------------------------------------


SerialThread::SerialThread(const char* portName) {
    this->portName=portName;
    currState = IDLE; // Start in idle state
}

void SerialThread::run() {
    //qDebug()<<"start SerialThread";

    this->serial = new Serial(portName.c_str());

    while(true) {
        switch(currState) {
            case IDLE:
                currState = idleState();
                break;
            case READY:
                currState = readyState();
                break;
            case CHARGE:
                currState = chargeState();
                break;
            case DISCHARGE:
                currState = dischargeState();
                break;
            case ERROR:
                // Wait for user to resume or cancel the test and change the state
                break;
            default:
                //qDebug()<<"Warning: Default state was reached";
                usleep(2000000);
                break;
        }
    }
}



int SerialThread::idleState(){
    QByteArray receive = serial->read();
    if(receive.contains("ready")) {
        batReady = 1;

        emit testerStateChange(READY, portName);

        qDebug()<<"ready: "<<receive;
        return READY;
    } else if(receive.contains("error")) {
        qDebug()<<"error: "<<receive;

        emit error(portName, receive.toStdString().c_str(), currState);
        return ERROR;
    }

    return IDLE;
}


int SerialThread::readyState() {
    QByteArray receive=serial->read();

    if(receive.contains("charge")) {
        qDebug()<<"charge: "<<receive;//<<QString::fromStdString(portName);

        this->csv = new CSVWriter();
        csv->open(fileName.c_str());

        emit testerStateChange(CHARGE, portName);

        return CHARGE;
    } else if(receive.contains("error")) {
        qDebug()<<"error: "<<receive;

        emit error(portName, receive.toStdString().c_str(), currState);
        return ERROR;
    }

    return READY;
}

/**
 * @brief SerialThread::startTest Handle signalStartTest signal sent from SerialHub when battery is ready and start button is pressed.
 *        Sets fileName, discharge current, and sends starttest message to Arduino to transition into the charge state
 * @param port Serial port of the tester which is to start a test
 * @param batteryID ID of the battery which is to be tested
 * @param current Discharge current for the test
 */
void SerialThread::startTest(std::string port, const char* batteryID, double dischargeCurrent) {
    if(port == portName) {
        fileName = "../Battery Testing Data/";
        if(std::string(batteryID).length() >= 6)
            fileName += std::string(batteryID).substr(std::string(batteryID).length()-6, 6) + ".csv";
        else
            fileName += std::string(batteryID) + ".csv";

        current = dischargeCurrent;

        std::string toSend = "starttest,";
        toSend += std::to_string(current);

        //qDebug()<<QString::fromStdString(toSend);
        serial->send(toSend.c_str());
    }
}


int SerialThread::chargeState() {
    QByteArray receive = serial->read();
    if(receive.contains("discharge")) {
        qDebug()<<"discharge: "<<receive;

        csv->write("volts,therm1,therm2,therm3\n"); // Add headers to csv file

        emit testerStateChange(DISCHARGE, portName);

        return DISCHARGE;
    } else if(receive.contains("error")) {
        qDebug()<<"error: "<<receive;

        emit error(portName, receive.toStdString().c_str(), currState);
        return ERROR;
    }

    return CHARGE;
}

int SerialThread::dischargeState() {
    QByteArray receive = serial->read();
    if(receive.contains("error")) {
        qDebug()<<"error: "<<receive;

        emit error(portName, receive.toStdString().c_str(), currState);
        return ERROR;
    } else if(receive.contains("finish")) {
        qDebug()<<"finish: "<<receive;

        csv->close();
        delete csv;
        batReady = 0;

        // TODO Remove emit testerStateChange(IDLE, portName); // TODO change to FINISH when finish state is added (to account for possible wait in firmware)

        emit testEnded(portName);
        return IDLE;
    } else {
        if(!receive.contains("discharge")) {
            csv->write(receive);
            qDebug()<<receive;
        }
        return DISCHARGE;
    }
}


/**
 * @brief SerialThread::handleErrorDecision Handles user's decision to resume or cancel the test after an error. Triggered by errorDecisionMade()
 * @param port Serial port of tester with the error being handled
 * @param prevState State tester was in when error was thrown
 * @param errorDecision int indicating user's decision to resume test (QMessageBox::Retry) or cancel test (QMessageBox::Cancel)
 */
void SerialThread::handleErrorDecision(std::string port, int prevState, int decision) {
    if(port == this->portName) {
        if(decision == QMessageBox::Retry) { // Resume test from previous state
            //qDebug()<<"SerialThread beginning of resumeTest() (for correct port/tester) "<<QString::fromStdString(portName);
            switch(prevState) {
                case IDLE:
                    serial->send("resume:idle");
                    qDebug()<<"resume: idle";
                    break;
                case READY:
                    serial->send("resume:ready");
                    qDebug()<<"resume: ready";
                    break;
                case CHARGE:
                    serial->send("resume:charge");
                    qDebug()<<"resume: charge";
                    break;
                case DISCHARGE:
                    serial->send("resume:discharge");
                    qDebug()<<"resume: discharge";
                    break;
                default:
                    qDebug()<<"Warning: The previous state in which an error was thrown was not idle, ready, charge, or discharge";
                    break;
            }

            currState = prevState;
        } else if(decision == QMessageBox::Abort) { // Abort test and return to idle state
            //qDebug()<<"SerialThread beginning of cancelTest() (for correct port/tester) "<<QString::fromStdString(portName);
            if(prevState != IDLE) {
                //qDebug()<<"Error didn't come from idle state";

                if(prevState != READY) {
                    //qDebug()<<"Error didn't come from ready state";

                    csv->remove(fileName.c_str());
                    delete csv;
                }

                batReady = 0;
                emit testEnded(portName);
            }
            qDebug()<<"SerialThread abort test (resume: idle)";

            serial->send("resume:idle");
            currState = IDLE;
        }
    }
}
