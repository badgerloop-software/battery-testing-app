#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <string>
#include <serialhub.h>
#include <QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void on_startButton_pressed();
    void indexChanged(int index);
    void on_statusChange();
    void on_errorButtonPressed();
    void on_voltageChange(std::string port, QString voltage); // TODO
    void on_movedToReady(std::string port); // TODO  Tester Ready Mouse
private:
    void init_combobox();
    void init_table();
    Ui::MainWindow *ui;

    bool connected=0;
    SerialHub *serialHub;
    //QList<QString*> voltages; // TODO
    QList<QLabel*> testStates;
    QList<QLabel*> voltageLabels; // TODO
    QList<QPushButton*> errorButtons;
};


#endif // MAINWINDOW_H

