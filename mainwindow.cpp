#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new Serial("/dev/cu.usbserial-2110");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_pressed() {
    std::string batteryID=ui->batteryID->text().toStdString(),
            testerID=ui->testerID->text().toStdString(),
            current=ui->current->text().toStdString();
    if(batteryID=="" || testerID=="" || current==""){
        QMessageBox msgbox;
        msgbox.setText("Please fill in all data");
        msgbox.exec();
        return;
    }
    std::string toSend = "StartTest,"+ui->batteryID->text().toStdString()+','+ui->testerID->text().toStdString()+','+ui->current->text().toStdString();
    serial->send(toSend.c_str());
    while(1){
        std::cout<<"112";
    }
}

