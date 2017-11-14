#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QModbusServer>
#include <QModbusTcpServer>

#include <QStringList>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    device = new QModbusTcpServer(this);
    connect(device,SIGNAL(dataWritten(QModbusDataUnit::RegisterType,int,int)),this,SLOT(deviceWritten(QModbusDataUnit::RegisterType,int,int)));

    QModbusDataUnitMap reg;
    //reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 64 });
    //reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 64 });
    //reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 64 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 64 });

    device->setMap(reg);
    const QUrl url = QUrl::fromUserInput("127.0.0.1:502");
    //const QUrl url = QUrl::fromUserInput("192.168.1.4:502");
    device->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
    device->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
    device->setServerAddress(1);
    device->connectDevice();

    //prepare manipulating unit
    dataIn = new QModbusDataUnit(QModbusDataUnit::HoldingRegisters,MotionSide::AOI_CONTROL_WORD,MotionSide::AOI_BLOCK_SIZE);
    dataOut=new QModbusDataUnit(QModbusDataUnit::HoldingRegisters,MotionSide::MOT_CONTROL_WORD,MotionSide::MOT_BLOCK_SIZE);

    //
    __motionSide = new MotionSide(this);

    connect(ui->buttonStart,SIGNAL(clicked()),__motionSide,SLOT(onStarted())); //button trigger onStarted
    connect(ui->buttonMotionDone,SIGNAL(clicked()),__motionSide,SLOT(onMotionDone()));

    connect(__motionSide,SIGNAL(scanOut(QVector<quint16>)),this,SLOT(deviceRegisterInternalWrite(QVector<quint16>)));
    connect(__motionSide,SIGNAL(stateChanged(const QState*)),this,SLOT(stateChangedHandler(const QState*)));

    modelRegister = new QStandardItemModel(this);

    modelRegister->insertColumns(0,2);
    modelRegister->insertRows(0,MotionSide::TOTAL_COUNT);

    modelRegister->setHeaderData(0,Qt::Horizontal,QString("Name"));
    modelRegister->setHeaderData(1,Qt::Horizontal,QString("Value"));

    QMetaEnum schemaMetaEnum = QMetaEnum::fromType<MotionSide::handshakeSchema>();
    for(int i=0;i<schemaMetaEnum.keyCount();i++){
        modelRegister->setData(modelRegister->index(schemaMetaEnum.keyToValue(schemaMetaEnum.key(i)),0)
                               ,QString(schemaMetaEnum.key(i)));
    }

    ui->registerTableView->setModel(modelRegister);
}

MainWindow::~MainWindow()
{
    //delete dataIn;
    //delete dataOut;
}


void MainWindow::deviceRegisterInternalWrite(QVector<quint16> outValues)
{
    dataOut->setValues(outValues);
    device->setData(*dataOut);

    for(int i=0;i<outValues.count();i++)
        modelRegister->setData(modelRegister->index(MotionSide::MOT_CONTROL_WORD+i,1),tr("0x%1").arg(outValues[i],4,16,QChar('0')));
}
void MainWindow::deviceWritten(QModbusDataUnit::RegisterType type,int address,int size)
{
    switch (type) {
    case QModbusDataUnit::HoldingRegisters:

        device->data(dataIn);

        QVector<quint16> temp = dataIn->values();
        __motionSide->scanIn(temp); // redirect to handshake controller

        //sync with model
        for (int i=0;i<temp.count();i++)
            modelRegister->setData(modelRegister->index(MotionSide::AOI_CONTROL_WORD+i,1),tr("0x%1").arg(temp[i],4,16,QChar('0')));
        break;
    }
}

void MainWindow::stateChangedHandler(const QState* currentState)
{
    ui->statusBar->showMessage(tr("Current State: %1").arg(currentState->objectName()));
}
