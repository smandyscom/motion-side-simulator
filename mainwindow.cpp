#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QModbusServer>
#include <QModbusTcpServer>

#include <QStringList>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    device = new QModbusTcpServer(this);

    __motionSide = new MotionSide(this);

    connect(ui->buttonStart,SIGNAL(clicked()),__motionSide,SLOT(onStarted())); //button trigger onStarted

    QStringList l1,l2;
    for(int i=0;i<64;i++){
        l1.append(QString::number(0));
        l2.append(QString::number(0));
    }

    modelIn = new QStringListModel(l1);
    modelOut = new QStringListModel(l2);

    //modelIn->setStringList(l1);
    //modelOut->setStringList(l2);

    ui->listViewModelIn->setModel(modelIn);
    ui->listViewModelOut->setModel(modelOut);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonSend_clicked()
{
    QVector<quint16> send;

    foreach (QString __s, modelIn->stringList()) {
        send.append(__s.toUShort());
    }

    __motionSide->scanIn(send);//drive state machine
}

void MainWindow::renderOut(QVector<quint16> values)
{
    for(int i=0;i<values.count();i++)
        modelOut->stringList().replace(i,QString::number(values[i]));

}
