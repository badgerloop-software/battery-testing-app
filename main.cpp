//this for the ui
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}


/*#include <QApplication>
#include <QWindow>
#include <QSerialPortInfo>
#include <QComboBox>

#include "serialhub.h"
#include <unistd.h>
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    SerialHub serial;
    serial.deploy("/dev/cu.usbserial-2110","123","123",321);
    //serials.deployThreads("/dev/cu.usbserial-2110","123","321",123);
    serial.procedure();
    return a.exec();
}



#include "serial.h"
#include "serialhubq.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    SerialThread serial("/dev/cu.usbserial-2110");
    serial.start();
    for(int i = 0;; i ++){
        QByteArray toSend;
        toSend.setNum(i);
        serial.sendMessage(toSend);
        qDebug()<<serial.getData();
        usleep(3000000);
    }
}
*/
