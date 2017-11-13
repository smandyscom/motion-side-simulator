#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusServer>
#include <QStateMachine>
#include <motionside.h>

#include <QStringListModel>
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


    void on_buttonSend_clicked();
    void renderOut(QVector<quint16> value);

    void on_pushButton_clicked();

    void on_textEdit_textChanged();

private:
    QStringList l1,l2;
    Ui::MainWindow *ui;
    QModbusServer* device;
    MotionSide* __motionSide;

    QStringListModel* modelIn;
    QStringListModel* modelOut;
};

#endif // MAINWINDOW_H
