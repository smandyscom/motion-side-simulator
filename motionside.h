#ifndef MOTIONSIDE_H
#define MOTIONSIDE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QModbusReply>

enum controlWordBitsAoi
{
    AOI_ONLINE=0x01,
    TRIG_ACK=0x02,
    MOV_REQUEST=0x04
};

enum controlWordBitsMot
{
    MOTION_ONLINE=0x01,
    TRIG_REQUEST=0x02,
    MOV_ACK=0x04
};

enum states
{
    IDLE,
    WAIT_MOT_REQ_ON,
    WAIT_MOT_ACTION, //simulated motion action
    WAIT_MOT_REQ_OFF,
    WAIT_TRIG_ACK_OFF
};

enum defTriggerMode : quint16
{
    AUTO=0,
    SEMI=1,
};

enum handshakeSchema
{
    CONTROL_WORD_MOT=0,
    CONTROL_WORD_AOI=1,
    TRIGGER_MODE=2,
    AXIS_DATA_1=16,
    AXIS_DATA_2=19,
    AOI_DATA_1=32
};

class motionSide : public QObject
{
    Q_OBJECT
public:
    explicit motionSide(QObject *parent = nullptr);
signals:
    void started();
    void transitionSatified(); //the condition about handshaking satisfied , e.g MOT_REQ_ON
    void triggerAcknowledged(); // the trigger ackowledge on (process going to finish
public slots:
    void process(); //decide whether emit transitionSatisfied()
protected slots:
    void motionDone();
    void stateEntered(); //occured on stateEntry signal emitted
    void stateExited();
    void onlineShutted(); //the online signal disapperaed
protected:

    //memory map? update by external rountine timer?
    quint16 memoryIn[64];
    quint16 memoryOut[64];

    controlWordBitsAoi transitionSignal;
    bool transitionState; //ON,OFF

    QStateMachine stateMachine;
    QMap<states,QState*> stateContainer;


};

#endif // MOTIONSIDE_H
