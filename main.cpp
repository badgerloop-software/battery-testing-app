#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

//this part demonstrates how serial class work

/*#include "serial.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Serial serial("/dev/cu.usbserial-110");
    while(1){
       serial.send("abc");

        qDebug()<<serial.read();

    }
    return a.exec();
}*/
