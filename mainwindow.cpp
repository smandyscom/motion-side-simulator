#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QModbusServer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    device = new QModbusServer(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}
