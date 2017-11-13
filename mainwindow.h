#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusServer>
#include <QStateMachine>
#include <motionside.h>

#include <QStringListModel>
#include <QModbusDataUnit>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
     void deviceRegisterInternalWrite(QVector<quint16> value);
     void deviceWritten(QModbusDataUnit::RegisterType type,int address,int size); //writtend by MODBUSTCP MASTER
     void on_motionDoneButton_clicked();

private:
    QStringList l1;
    Ui::MainWindow *ui;
    QModbusServer* device;
    MotionSide* __motionSide;

    QStringListModel* modelRegister;

    QModbusDataUnit* dataIn; //have to destrct or leak
    QModbusDataUnit* dataOut;
};

#endif // MAINWINDOW_H
