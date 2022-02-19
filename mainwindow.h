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
private:
    void init_combobox();
    Ui::MainWindow *ui;

    bool connected=0;
    SerialHub *serial;
};
#endif // MAINWINDOW_H

