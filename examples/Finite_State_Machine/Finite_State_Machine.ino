#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <interfaces/FiniteStateMachineInterface.hpp>

NodeMQTT *thisNode;
FiniteStateMachineInterface *fsm;

void init();
void unlock();
void maintain();
void disable();
void boot();

void setup()
{
    thisNode = new NodeMQTT();
    fsm = new FiniteStateMachineInterface("control");
    fsm->addState("INIT", init, nullptr, nullptr, true);
    fsm->addState("UNLOCK", unlock);
    fsm->addState("DISABLE", disable);
    fsm->addState("MAINTAIN", maintain);
    thisNode->begin();
    boot();
}

void loop()
{
    thisNode->handle();
}
void boot()
{
}

void init()
{
}
void unlock()
{
}
void maintain()
{
}
void disable()
{
}