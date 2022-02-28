//this for the ui
#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap("Initializing.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();
    MainWindow w;
    w.show();
    splash.finish(&w);
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

*/

/*#include "serial.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    Serial serial("/dev/cu.usbserial-1130");
    serial.send("tester?\n");
    qDebug()<<serial.read();
}*/

