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
    init_table();
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

void MainWindow::init_table() {
    std::vector<deviceInfo*> devices = serialHub->getDevices();
    ui->testerTable->insertColumn(0);
    ui->testerTable->insertColumn(1);
    ui->testerTable->insertColumn(2);

    QStringList headers = { "Teseter ID", "Test State", "Error" };
    ui->testerTable->setHorizontalHeaderLabels(headers);

    for(int r=0; r < devices.size(); r++) {
        testStates.push_back(new QLabel("Idle"));
        errorButtons.push_back(new QPushButton());
        errorButtons[r]->setCheckable(true);
        errorButtons[r]->setStyleSheet("QPushButton { background-color : white } QPushButton:pressed { background-color : white } ");
        connect(errorButtons[r], &QPushButton::toggled, this, &MainWindow::on_errorButtonPressed);

        ui->testerTable->insertRow(r);

        ui->testerTable->setCellWidget(r, 0, new QLabel(QString::fromStdString(devices[r]->testerID)));
        ui->testerTable->setCellWidget(r, 1, testStates[r]);
        ui->testerTable->setCellWidget(r, 2, errorButtons[r]);
    }

}

void MainWindow::on_errorButtonPressed() {
    //qDebug()<<"Error button was pressed";
    std::vector<deviceInfo*> devices = serialHub->getDevices();

    for(int i=0; i < ui->testerTable->rowCount(); i++) {
        if(errorButtons[i]->isChecked()) {
            //qDebug()<<"And "<<i<<" is down!";
            errorButtons[i]->setChecked(false);

            if(devices[i]->error) {
                //qDebug()<<"AND ERROR BUTTON "<<i<<" HAS AN ERROR!!!";
                QMessageBox err;
                err.setWindowTitle(devices[i]->errorTitle);
                err.setText(devices[i]->errorMessage + "\nAbort test and discard data or resume test?\nPress escape or close this pop-up to ignore the error for now.");
                err.setStandardButtons(QMessageBox::Abort|QMessageBox::Retry|QMessageBox::Cancel);
                err.button(QMessageBox::Cancel)->setVisible(false);

                int decision = err.exec();

                if(decision != QMessageBox::Cancel) {
                    devices[i]->error = false;

                    ui->testerTable->cellWidget(i, 2)->setStyleSheet("QPushButton { background-color : white } QPushButton:pressed { background-color : white } ");

                    deviceInfo *device = serialHub->getDeviceConfig(ui->comboBox->currentText().toStdString().c_str());
                    if(device == devices[i]) {
                        if(devices[i]->running && (decision == QMessageBox::Retry))
                            ui->startButton->setText("Running...");
                        else
                            ui->startButton->setText("Start");
                    }

                    emit serialHub->errorDecisionMade(devices[i]->devicePort, devices[i]->testerState, decision);
                }
            }
        }
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
        if(device->error)
            ui->startButton->setText("Error");
        else if(device->running)
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

    if((ui->startButton->text() == "Start") && !devInfo->error) {
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
                std::ifstream f;
                if(batteryID.length() >= 6)
                    f = std::ifstream("../Battery Testing Data/" + batteryID.substr(batteryID.length()-6, 6) + ".csv");
                else
                    f = std::ifstream("../Battery Testing Data/" + batteryID + ".csv");

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
    std::vector<deviceInfo*> devices = serialHub->getDevices();

    for(int r=0; r < devices.size(); r++) {
        switch(devices[r]->testerState) {
            case IDLE:
                testStates[r]->setText("Idle");
                break;
            case READY:
                testStates[r]->setText("Ready");
                break;
            case CHARGE:
                testStates[r]->setText("Charging");
                break;
            case DISCHARGE:
                testStates[r]->setText("Discharging");
                break;
            default:
                testStates[r]->setText("Um");
                break;
        }

        if(devices[r]->error) {
            ui->testerTable->cellWidget(r, 2)->setStyleSheet("QPushButton { background-color : red } QPushButton:pressed { background-color : red } ");
        }
    }


    deviceInfo *device = serialHub->getDeviceConfig(ui->comboBox->currentText().toStdString().c_str());
    ui->batteryID->setText(device->batteryID.c_str());
    ui->current->setText(std::to_string(device->discharge_current).c_str());
    if(device->error)
        ui->startButton->setText("Error");
    else if(device->running)
        ui->startButton->setText("Running...");
    else
        ui->startButton->setText("Start");

    if(device->batReady) {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : green; }");
    } else {
         ui->BatteryStat->setStyleSheet("QLabel { background-color : red; }");
    }
}
