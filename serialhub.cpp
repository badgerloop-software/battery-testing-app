#include "serialhub.h"

/**
 * @brief SerialHub::SerialHub create a new SerialHub object containing all connected battery testers
 *                             (determined by pinging each serial port and checking the response)
 */
SerialHub::SerialHub() {
    for(QSerialPortInfo port: QSerialPortInfo::availablePorts()){
        //qDebug()<<port.portName();
        if(port.portName().contains("ttyUSB") || port.portName().contains("ttyACM") || port.portName().contains("cu")){
            //qDebug()<<"contains USB or ACM";
            Serial serial(("/dev/"+port.portName().toStdString()).c_str());
            serial.send("tester,\n");
            QByteArray reply = serial.read();
            //qDebug()<<"Reply: "<<reply;
            if(reply.contains("yes")){
                QStringList testID=QString(reply).split(",");
                deviceInfoList.push_back(new deviceInfo(("/dev/"+port.portName().toStdString()).c_str(),testID[1].toStdString(), "", 1.5));
                devices.push_back(new SerialThread(("/dev/"+port.portName().toStdString()).c_str()));

                QObject::connect(&*devices[devices.size()-1], &SerialThread::batteryStatusChange,
                                 this, &SerialHub::on_batteryStatusChange);
                QObject::connect(&*devices[devices.size()-1], &SerialThread::testEnded,
                                 this, &SerialHub::on_testEnded);
                QObject::connect(&*devices[devices.size()-1], &SerialThread::error,
                                 this, &SerialHub::on_error);
                QObject::connect(this, &SerialHub::errorDecisionMade,
                                 &*devices[devices.size()-1], &SerialThread::handleErrorDecision);
                QObject::connect(this, &SerialHub::signalStartTest,
                                 &*devices[devices.size()-1], &SerialThread::startTest);
                QObject::connect(&*devices[devices.size()-1], &SerialThread::finished,
                                 &*devices[devices.size()-1], &SerialThread::deleteLater);

                devices[devices.size()-1]->start();
            }
        }
    }
}

void SerialHub::startTest(const char* testerID, const char* batteryID, double current) {
    int deviceIndex = getIndex(testerID);

    if(deviceIndex != -1) {
        deviceInfoList[deviceIndex]->batteryID = batteryID;
        deviceInfoList[deviceIndex]->discharge_current = current;
        deviceInfoList[deviceIndex]->running = true;
        emit signalStartTest(deviceInfoList[deviceIndex]->devicePort, batteryID, current);
    }
}

/**
 * @brief SerialHub::getIndex Get index of device in deviceInfoList with the specified testerID
 * @param testerID The testerID of the device to find
 * @return The index of the device in deviceInfoList with a testerID matching the provided string; if not found, -1 is returned
 */
int SerialHub::getIndex(std::string testerID) {
    for(int i = 0 ; i < deviceInfoList.size() ; i ++) {
        if(deviceInfoList[i]->testerID == testerID) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief SerialHub::getDeviceConfig Get device from deviceInfoList using the provided testerID
 * @param testerID The testerID of the desired device
 * @return The device with a testerID matching the provided string; If not found, an empty device is returned
 */
deviceInfo* SerialHub::getDeviceConfig(std::string testerID) {
    for(int i = 0 ; i < deviceInfoList.size() ; i++) {
        if(deviceInfoList[i]->testerID == testerID) {
            return deviceInfoList[i];
        }
    }

    deviceInfo *deviceInformation = new deviceInfo();
    deviceInformation->batteryID = "DNE";
    deviceInformation->testerID = "";
    deviceInformation->discharge_current = 1.5;

    return deviceInformation;
}

/**
 * @brief SerialHub::on_batteryStatusChange Handle batteryStatusChange signal from SerialThread. Notifies MainWindow of status change
 */
void SerialHub::on_batteryStatusChange() {
    //qDebug()<<"SerialHub: on_BatteryStatusChange() called";

    for(int i = 0 ; i < devices.size() ; i ++) {
        deviceInfoList[i]->batReady = devices[i]->batReady;

        if(devices[i]->batReady){
            //qDebug()<<i<<" is ready (batReady is true)";
        }
    }

    emit statusChange();
}

/**
 * @brief SerialHub::on_testEnded Handle testEnded signal from SerialThread. Notifies MainWindow of status change
 */
void SerialHub::on_testEnded(std::string port) {
    for (int i = 0 ; i < devices.size() ; i++){
        if(deviceInfoList[i]->devicePort == port) {
            deviceInfoList[i]->running = false;
            deviceInfoList[i]->batteryID = "";
            deviceInfoList[i]->discharge_current = 1.5;
            deviceInfoList[i]->batReady = devices[i]->batReady;
        }
    }

    emit statusChange();
}

/**
 * @brief SerialHub::on_error Handle error signal from SerialThread
 * @param port Serial port of the tester throwing an error
 * @param message Error message from tester
 * @param prevState State tester was in when error was thrown
 */
void SerialHub::on_error(std::string port, QString message, int prevState) {
    QString title = "";

    for (int i = 0 ; i < devices.size() ; i++){
        if(deviceInfoList[i]->devicePort == port) {
            title += QString::fromStdString(deviceInfoList[i]->testerID);
            switch(prevState) {
                case IDLE:
                    title += ": Idle";
                    break;
                case READY:
                    title += ": Ready";
                    break;
                case CHARGE:
                    title += ": Charging";
                    break;
                case DISCHARGE:
                    title += ": Discharging";
                    break;
                default:
                    //qDebug()<<"SerialHub: Error was from state other than idle, ready, charging, or discharging";
                    break;
            }
        }
    }

    QMessageBox err;
    err.setWindowTitle(title);
    err.setText(message);
    err.setStandardButtons(QMessageBox::Cancel|QMessageBox::Retry);

    //qDebug()<<"error state is running";

    emit errorDecisionMade(port, prevState, err.exec());
}
