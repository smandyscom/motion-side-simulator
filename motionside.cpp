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
    }

    //configure state machine
    stateContainer[states::IDLE]->addTransition(this,SIGNAL(started()),stateContainer[states::WAIT_MOT_REQ_ON]);

    stateContainer[states::WAIT_MOT_REQ_ON]->addTransition(this,SIGNAL(transitionSatified()),stateContainer[states::WAIT_MOT_ACTION]);

    stateContainer[states::WAIT_MOT_REQ_OFF]->addTransition(this,SIGNAL(transitionSatified()),stateContainer[states::WAIT_MOT_REQ_ON]);


    stateMachine.setInitialState(stateContainer[states::IDLE]);

    s1->addTransition(timer,SIGNAL(timeout()),s2);
    s1->setObjectName(tr("S1"));
    connect(s1,SIGNAL(entered()),this,SLOT(showMessage()));

    s2->addTransition(timer,SIGNAL(timeout()),s1);
    s2->setObjectName(tr("S2"));
    connect(s2,SIGNAL(entered()),this,SLOT(showMessage()));

    fsm->addState(s1);
    fsm->addState(s2);
    fsm->setInitialState(s1);
    fsm->start();
}
\
void motionSide::processingSignals()
{
    if (((memory[handshakeSchema::CONTROL_WORD_AOI] & transitionSignal) >0) == transitionState)
        emit transitionSatified();
}

void motionSide::stateEntered()
{
    QState* currentState = static_cast<QState*>(sender());

}
