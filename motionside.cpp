#include "motionside.h"
#include <QState>



MotionSide::MotionSide(QObject *parent) : QObject(parent)
{
    //register clear
    memset(__register,0,handshakeSchema::TOTAL_COUNT);
    registerIn=reinterpret_cast<quint16*>(&__register[handshakeSchema::AOI_CONTROL_WORD]);
    registerOut=reinterpret_cast<quint16*>(&__register[handshakeSchema::MOT_CONTROL_WORD]);

    // initialize data anchor
    controlWordAoi = &registerIn[handshakeSchema::AOI_CONTROL_WORD];
    controlWordMot = &registerOut[handshakeSchema::MOT_CONTROL_WORD];
    axisData1 = reinterpret_cast<AXIS_DATA*>(&registerIn[handshakeSchema::AOI_AXIS_DATA_1]);
    axisData2 = reinterpret_cast<AXIS_DATA*>(&registerIn[handshakeSchema::AOI_AXIS_DATA_2]);

    //state machine initialize

    for (int i=states::IDLE;i<=states::WAIT_TRIG_ACK_OFF;i++)
        stateContainer.insert(static_cast<states>(i),new QState());

    //direct to central processor
    foreach (QState* __state, stateContainer.values()) {
        stateMachine.addState(__state);//added state
        connect(__state,SIGNAL(entered()),this,SLOT(stateEntered()));
    }
    //for each follwing state , jump off when online offed
    foreach (states __s, QVector<states>({states::WAIT_MOT_REQ_ON,states::WAIT_MOT_REQ_OFF,states::WAIT_TRIG_ACK_OFF}))
    {
        stateContainer[__s]->addTransition(this,SIGNAL(onlineShutted()),stateContainer[states::IDLE]);
    }

    //configure state machine
    stateContainer[states::IDLE]->addTransition(this,SIGNAL(started()),stateContainer[states::WAIT_MOT_REQ_ON]);

    stateContainer[states::WAIT_MOT_REQ_ON]->addTransition(this,SIGNAL(triggerAcknowledged()),stateContainer[states::WAIT_TRIG_ACK_OFF]); //process finished
    stateContainer[states::WAIT_MOT_REQ_ON]->addTransition(this,SIGNAL(nextTransitionSatified()),stateContainer[states::WAIT_MOT_ACTION]);

    stateContainer[states::WAIT_MOT_ACTION]->addTransition(this,SIGNAL(motionDone()),stateContainer[states::WAIT_MOT_REQ_OFF]);
    stateContainer[states::WAIT_MOT_REQ_OFF]->addTransition(this,SIGNAL(nextTransitionSatified()),stateContainer[states::WAIT_MOT_REQ_ON]);

    stateContainer[states::WAIT_TRIG_ACK_OFF]->addTransition(this,SIGNAL(nextTransitionSatified()),stateContainer[states::IDLE]); //recycle

    stateMachine.setInitialState(stateContainer[states::IDLE]);
    stateMachine.start();
}
\


void MotionSide::scanIn(const QVector<quint16> __registerIn)
{
    for(int i=0;i<__registerIn.count();i++)
        this->registerIn[i]=__registerIn[i];// operator = , assign?

    //check bits and turns into Q_SIGNAL
    if ( (*controlWordAoi & controlWordBitsAoi::AOI_ONLINE) == 0)
        emit onlineShutted();//online offed
    if (((*controlWordAoi & nextTransitionSignal) >0) == nextTransitionState)
        emit nextTransitionSatified();
    if ((*controlWordAoi & controlWordBitsAoi::TRIG_ACK) > 0)
        emit triggerAcknowledged();

    //--------------------
    //signal logger here?
    //--------------------

    QVector<quint16> __registerOut;
    for(int i=0;i<64;i++)
        __registerOut.append(registerOut[i]);
    emit scanOut(__registerOut); //populate out register
}

void MotionSide::stateEntered()
{
    QState* currentState = static_cast<QState*>(sender());

    switch (stateContainer.keys(currentState).first()) {
    case states::IDLE:
        //clear all signals , online only
        *controlWordMot = controlWordBitsMot::MOTION_ONLINE;
        break;
    case states::WAIT_MOT_REQ_ON:
        //set-up trigger/online on , and mode
        *controlWordMot &= ~controlWordBitsMot::MOV_ACK; //MOV_ACK off
        *controlWordMot |= controlWordBitsMot::MOTION_ONLINE+controlWordBitsMot::TRIG_REQUEST; // TRIG_REQ on , ONLINE on

        nextTransitionSignal = controlWordBitsAoi::MOV_REQUEST;
        nextTransitionState = true;
        break;
    case states::WAIT_MOT_ACTION:
        //activate motion
        break;
    case states::WAIT_MOT_REQ_OFF:
        *controlWordMot |= controlWordBitsMot::MOV_ACK;//MOV_ACK on

        nextTransitionSignal = controlWordBitsAoi::MOV_REQUEST;
        nextTransitionState = false;
        break;
    case states::WAIT_TRIG_ACK_OFF:
        //TODO fetch the AOI_DATAS


        *controlWordMot &= ~controlWordBitsMot::TRIG_REQUEST;//TRIG_REQ OFF

        nextTransitionSignal = controlWordBitsAoi::TRIG_ACK;
        nextTransitionState = false;
        break;
    default:
        break;
    }
}

void MotionSide::setMode(triggerMode mode)
{
    registerOut[handshakeSchema::MOT_TRIGGER_MODE] = mode;
}
const triggerMode MotionSide::getMode()
{
    return static_cast<triggerMode>(registerOut[handshakeSchema::MOT_TRIGGER_MODE]);
}

void MotionSide::onStarted()
{
    emit started();
}
