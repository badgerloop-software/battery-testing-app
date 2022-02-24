#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //serial = new Serial("/dev/cu.usbserial-2110");
    init_combobox();
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(indexChanged(int)));
    serial= new SerialHub();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_combobox(){
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(port.portName());
    }
}

void MainWindow::indexChanged(int index) {
    std::string selected ="/dev/";
    selected += ui->comboBox->currentText().toStdString();
    deviceInfo device = serial->getDeviceConfig(selected.c_str());
    if(device.batteryID=="DNE") {
        ui->batteryID->setText("");
        ui->current->setText("");
        ui->testerID->setText("");
        ui->startButton->setText("Start");
    } else {
        ui->batteryID->setText(device.batteryID.c_str());
        ui->current->setText(std::to_string(device.discharge_current).c_str());
        ui->testerID->setText(device.testerID.c_str());
        ui->startButton->setText("Running...");
    }
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
    std::string port="/dev/";
    port += ui->comboBox->currentText().toStdString();
    serial->deploy(port.c_str(),testerID.c_str(),batteryID.c_str(),std::stoi(current));
    ui->startButton->setText("Running...");
}

