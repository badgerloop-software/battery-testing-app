#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //serial = new Serial("/dev/cu.usbserial-2110");

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(indexChanged(int)));
    serial= new SerialHub();
    ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
    init_combobox();
    connect(serial,&SerialHub::statusChange,this,&MainWindow::on_statusChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_combobox(){
    std::vector<deviceInfo> devices = serial->getDevices();
    for(int i = 0 ; i < devices.size() ; i++) {
        ui->comboBox->addItem(devices[i].testerID.c_str());
    }
}

void MainWindow::indexChanged(int index) {
    QString selected = ui->comboBox->currentText();
    deviceInfo device = serial->getDeviceConfig(selected.toStdString().c_str());
    if(device.batteryID=="DNE") {
        ui->batteryID->setText("");
        ui->current->setText("");
        ui->startButton->setText("Start");
    } else {
        ui->batteryID->setText(device.batteryID.c_str());
        ui->current->setText(std::to_string(device.discharge_current).c_str());
        if(device.running)
            ui->startButton->setText("Running...");
        else
            ui->startButton->setText("Start");

        if(device.batReady) {
             ui->BatteryStat->setStyleSheet("QLabel { background-color : green; }");
        } else {
             ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
        }
    }
}

void MainWindow::on_startButton_pressed() {
    std::string batteryID=ui->batteryID->text().toStdString(),
            current=ui->current->text().toStdString();
    if(batteryID=="" || current==""){
        QMessageBox msgbox;
        msgbox.setText("Please fill in all data");
        msgbox.exec();
        return;
    }
    std::string port=serial->getDeviceConfig(ui->comboBox->currentText().toStdString()).devicePort;
    serial->deploy(port.c_str(),ui->comboBox->currentText().toStdString().c_str(),batteryID.c_str(),std::stoi(current));
    ui->startButton->setText("Running...");
}

void MainWindow::on_statusChange() {
    deviceInfo device = serial->getDeviceConfig(ui->comboBox->currentText().toStdString().c_str());
    ui->batteryID->setText(device.batteryID.c_str());
    ui->current->setText(std::to_string(device.discharge_current).c_str());
    if(device.running)
        ui->startButton->setText("Running...");
    else
        ui->startButton->setText("Start");

    if(device.batReady) {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : green; }");
    } else {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
    }
}
