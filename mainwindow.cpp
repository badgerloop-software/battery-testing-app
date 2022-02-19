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
    qDebug()<<ui->comboBox->currentText();
}

void MainWindow::on_startButton_pressed() {
    std::string batteryID=ui->batteryID->text().toStdString(),
            testerID=ui->testerID->text().toStdString(),
            current=ui->current->text().toStdString();
    serial->deploy("/dev/cu.usbseria;l-2110","123","312",123);
    if(batteryID=="" || testerID=="" || current==""){
        QMessageBox msgbox;
        msgbox.setText("Please fill in all data");
        msgbox.exec();
        return;
    }
    std::string toSend = "StartTest,"+ui->batteryID->text().toStdString()+','+ui->testerID->text().toStdString()+','+ui->current->text().toStdString();
    while(1){
        serial->procedure();
        qApp->processEvents();
    }
}

