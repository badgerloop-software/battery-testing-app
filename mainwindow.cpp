#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(indexChanged(int)));
    serialHub= new SerialHub();
    ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
    init_combobox();
    connect(serialHub,&SerialHub::statusChange,this,&MainWindow::on_statusChange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_combobox(){
    std::vector<deviceInfo*> devices = serialHub->getDevices();
    for(int i = 0 ; i < devices.size() ; i++) {
        ui->comboBox->addItem(devices[i]->testerID.c_str());
    }
}

void MainWindow::indexChanged(int index) {
    QString selected = ui->comboBox->currentText();
    deviceInfo *device = serialHub->getDeviceConfig(selected.toStdString().c_str());
    if(device->batteryID=="DNE") {
        ui->batteryID->setText("");
        ui->current->setText(std::to_string(device->discharge_current).c_str());
        ui->startButton->setText("Start");
    } else {
        ui->batteryID->setText(device->batteryID.c_str());
        ui->current->setText(std::to_string(device->discharge_current).c_str());
        if(device->running)
            ui->startButton->setText("Running...");
        else
            ui->startButton->setText("Start");

        if(device->batReady) {
             ui->BatteryStat->setStyleSheet("QLabel { background-color : green; }");
        } else {
             ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
        }
    }
}

void MainWindow::on_startButton_pressed() {
    std::string batteryID = ui->batteryID->text().toStdString(),
                current = ui->current->text().toStdString();
    deviceInfo* devInfo = serialHub->getDeviceConfig(ui->comboBox->currentText().toStdString());

    if(ui->startButton->text() == "Start") {
        // Check validity of battery ID, discharge current, and state (should be ready)
        try {
            if(!devInfo->batReady) {
                QMessageBox msgbox;
                msgbox.setText("Please insert a battery and press the button on the battery tester");
                msgbox.exec();
                qApp->processEvents();
                return;
            } else if(batteryID == "" || current == "") {
                QMessageBox msgbox;
                msgbox.setText("Please fill in all data");
                msgbox.exec();
                return;
            } else if(std::stod(current) <= 0) {
                QMessageBox msgbox;
                msgbox.setText("Please enter a valid current");
                msgbox.exec();
                return;
            } else {
                std::ifstream f("../Battery Testing Data/" + batteryID.substr(batteryID.length()-6, 6) + ".csv");

                if(f.good()) {
                    QMessageBox fileExistsBox;
                    fileExistsBox.setWindowTitle("WARNING");
                    fileExistsBox.setText(QString::fromStdString("Test data for battery " + batteryID + " already exists. If you continue, battery " + batteryID + "'s existing test data will be removed.\nAre you sure you want to continue?"));
                    fileExistsBox.setStandardButtons(QMessageBox::No|QMessageBox::Yes);

                    if(fileExistsBox.exec() == QMessageBox::No) {
                        return;
                    }
                }

            }
        } catch(const std::invalid_argument& ia) {
            QMessageBox msgbox;
            msgbox.setText("Please enter a valid current");
            msgbox.exec();
            return;
        }

        devInfo->batteryID = batteryID;
        devInfo->discharge_current = std::stod(current);
        devInfo->running = true;
        emit serialHub->signalStartTest(devInfo->devicePort, batteryID.c_str(), std::stod(current));

        ui->startButton->setText("Running...");
    }
}

void MainWindow::on_statusChange() {
    deviceInfo *device = serialHub->getDeviceConfig(ui->comboBox->currentText().toStdString().c_str());
    ui->batteryID->setText(device->batteryID.c_str());
    ui->current->setText(std::to_string(device->discharge_current).c_str());
    if(device->running)
        ui->startButton->setText("Running...");
    else
        ui->startButton->setText("Start");

    if(device->batReady) {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : green; }");
    } else {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
    }
}
