#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <string>
#include <serial.h>

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
private:
    Ui::MainWindow *ui;
    bool connected=0;
    Serial *serial;
};
#endif // MAINWINDOW_H

