#ifndef MOTIONSIDE_H
#define MOTIONSIDE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QModbusReply>

enum controlWordBitsAoi : quint16
{
    AOI_ONLINE=0x01,
    TRIG_ACK=0x02,
    MOV_REQUEST=0x04
};

enum controlWordBitsMot : quint16
{
    MOTION_ONLINE=0x01,
    TRIG_REQUEST=0x02,
    MOV_ACK=0x04
};



enum triggerMode : quint16
{
    AUTO=0,
    SEMI=1,
};




struct axisData
{
    quint16 isEnabled;
    quint16 mode;
    quint16 type; // abs:0,rel:1
    quint16 bankNumber;
    qint32 coordinate;
};

typedef  struct axisData AXIS_DATA;

class MotionSide : public QObject
{
    Q_OBJECT
public:
    explicit MotionSide(QObject *parent = nullptr);
    enum handshakeSchema
    {
        MOT_CONTROL_WORD=0,
        MOT_TRIGGER_MODE=1,
        AOI_CONTROL_WORD=8,
        AOI_AXIS_DATA_1=16,
        AOI_AXIS_DATA_2=19,
        AOI_DATA_1=32,
        TOTAL_COUNT=64,
        MOT_BLOCK_SIZE = AOI_CONTROL_WORD-MOT_CONTROL_WORD,
        AOI_BLOCK_SIZE = TOTAL_COUNT-AOI_CONTROL_WORD
    };

    enum states
    {
        IDLE,
        WAIT_MOT_REQ_ON,
        WAIT_MOT_ACTION, //simulated motion action
        WAIT_MOT_REQ_OFF,
        WAIT_TRIG_ACK_OFF
    };
    Q_ENUM(states)
    Q_ENUM(handshakeSchema)

    void setMode(triggerMode mode);
    triggerMode getMode(void) const;
signals:
    void stateChanged(const QState *currentState);
    void scanOut(const QVector<quint16> registerOut);
public slots:
    void onStarted();
    void onMotionDone(); //triggered by external
    void scanIn(const QVector<quint16> registerIn); //decide whether emit transitionSatisfied()
protected slots:
    void stateEntered(); //occured on stateEntry signal emitted
protected:

    Q_SIGNAL void motionDone();
    Q_SIGNAL void started(); //by ui or some triggers

    Q_SIGNAL void nextTransitionSatified(); //the condition about handshaking satisfied , e.g MOT_REQ_ON
    Q_SIGNAL void triggerAcknowledged(); // the trigger ackowledge on (process going to finish
    Q_SIGNAL void onlineShutted(); //the online signal disapperaed

    //memory map? update by external rountine timer?
    quint16* registerIn;
    quint16* registerOut;
    QVector<quint16> ____registerOut;
    const QVector<quint16> __registerOut();

    quint16 __register[handshakeSchema::TOTAL_COUNT];

    quint16 nextTransitionSignal;
    bool nextTransitionState; //ON,OFF

    quint16* controlWordAoi;
    quint16* controlWordMot;
    AXIS_DATA* axisData1; // anchor to sub-block of handshake data block
    AXIS_DATA* axisData2;

    QStateMachine stateMachine;
    QMap<states,QState*> stateContainer;



};

#endif // MOTIONSIDE_H
