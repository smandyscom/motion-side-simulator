#include "motionside.h"
#include <QState>



motionSide::motionSide(QObject *parent) : QObject(parent)
{
    //state machine initialize

    for (int i=states::IDLE;i<=states::WAIT_TRIG_ACK_OFF;i++)
        stateContainer.insert(static_cast<states>(i),new QState(stateMachine));

    //direct to central processor
    QState* __state;
    foreach (__state, stateContainer.values()) {
        stateMachine.addState(__state);//added state
        connect(__state,SIGNAL(entered()),this,SLOT(stateEntered());
        connect(__state,SIGNAL(exited()),this,SLOT(stateExited()));
        __state->
    }

    //configure state machine
    stateContainer[states::IDLE]->addTransition(this,SIGNAL(started()),stateContainer[states::WAIT_MOT_REQ_ON]);

    stateContainer[states::WAIT_MOT_REQ_ON]->addTransition(this,SIGNAL(triggerAcknowledged()),stateContainer[states::WAIT_TRIG_ACK_OFF]); //process finished
    stateContainer[states::WAIT_MOT_REQ_ON]->addTransition(this,SIGNAL(transitionSatified()),stateContainer[states::WAIT_MOT_ACTION]);

    stateContainer[states::WAIT_MOT_ACTION]->addTransition(this,SIGNAL(motionDone()),stateContainer[states::WAIT_MOT_REQ_OFF]);
    stateContainer[states::WAIT_MOT_REQ_OFF]->addTransition(this,SIGNAL(transitionSatified()),stateContainer[states::WAIT_MOT_REQ_ON]);

    stateContainer[states::WAIT_TRIG_ACK_OFF]->addTransition(this,SIGNAL(transitionSatified()),stateContainer[states::IDLE]); //recycle

    stateMachine.setInitialState(stateContainer[states::IDLE]);

}
\
void motionSide::process()
{
    if (((memoryIn[handshakeSchema::CONTROL_WORD_AOI] & transitionSignal) >0) == transitionState)
        emit transitionSatified();
}

void motionSide::stateEntered()
{
    QState* currentState = static_cast<QState*>(sender());

    switch (stateContainer.keys(currentState).first()) {
    case states::IDLE:
        //clear all signals
        break;
    case states::WAIT_MOT_REQ_ON:
        //set-up trigger on , and mode
        //MOT_ACK OFF
        break;
    case states::WAIT_MOT_ACTION:
        //activate motion
        break;
    case states::WAIT_MOT_REQ_OFF:
        //set-up MOT_ACK ON
        break;
    case states::WAIT_TRIG_ACK_OFF:
        //set-up TRIG_REQ OFF
        break;
    default:
        break;
    }
}
