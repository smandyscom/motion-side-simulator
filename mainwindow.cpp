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
    connect(device,SIGNAL(dataWritten(QModbusDataUnit::RegisterType,int,int)),this,SLOT(deviceWritten(QModbusDataUnit::RegisterType,int,int)));

    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 64 });
    reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 64 });
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 64 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 64 });

    device->setMap(reg);
    //const QUrl url = QUrl::fromUserInput("127.0.0.1:502");
    const QUrl url = QUrl::fromUserInput("192.168.1.4:502");
    device->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
    device->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
    device->setServerAddress(1);
    device->connectDevice();

    //
    __motionSide = new MotionSide(this);

    connect(ui->buttonStart,SIGNAL(clicked()),__motionSide,SLOT(onStarted())); //button trigger onStarted
    connect(__motionSide,SIGNAL(scanOut(QVector<quint16>)),this,SLOT(renderOut(QVector<quint16>)));


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

    __qtimer = new QTimer(this);
    __qtimer->setInterval(100);
    connect(__qtimer,SIGNAL(timeout()),this,SLOT(deviceWritten()));
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
    {
        modelOut->setData(modelOut->index(i,0),QString::number(values[i]));
        //modelOut->stringList().replace(i,QString::number(values[i]))
        device->setData(QModbusDataUnit::InputRegisters,i,values[i]);
    }

}

void MainWindow::on_pushButton_clicked()
{
    __motionSide->motionDone();
}

void MainWindow::on_textEdit_textChanged()
{
    //modelOut->stringList().replace(0,ui->textEdit->toPlainText());
    QString qs = ui->textEdit->toPlainText();
    l2.replace(0,ui->textEdit->toPlainText());
    l2[0] = ui->textEdit->toPlainText();
    modelOut->setData(modelOut->index(0,0),ui->textEdit->toPlainText());
}

void MainWindow::deviceWritten(QModbusDataUnit::RegisterType type,int address,int size)
{
    switch (type) {
    case QModbusDataUnit::Coils:
        quint16 cache;
        for(int i=0;i<size;i++)
        {
            device->data(type,address+i,&cache);
            modelIn->setData(modelIn->index(address+i,0),cache);
        }
        on_buttonSend_clicked();
        break;
    default:
        break;
    }
}
