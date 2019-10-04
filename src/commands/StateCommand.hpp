#ifndef StateCommand_H
#define StateCommand_H

#include "_AbstractCommand.hpp"
// #include "../transports/_AbstractTransport.hpp"

class StateCommand : public AbstractCommand
{
public:
  StateCommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline StateCommand::StateCommand(ApplicationContext *context) :  AbstractCommand(context) {
    setCommandWord(PSTR("STATE"));
}

inline void StateCommand::handle()
{
  NodeMQTTIO.println(TERMINAL_HR);
  NodeMQTTIO.println(this->getContext()->transport->getState());



  NodeMQTTIO.println(TERMINAL_HR);
  NodeInterfaceBase *interface;
  String ifState = "";
  for (int i = 0; i < this->getContext()->interfaces->size(); i++)
    {
    char buf[128];
    interface = this->getContext()->interfaces->get(i);
    sprintf(buf, "[pub]%s/[sub]%s - [%s]", interface->getSubscribeTopic().c_str(), interface->getSubscribeTopic().c_str(), interface->valueToString().c_str());
    NodeMQTTIO.println(buf);
    }
  NodeMQTTIO.println(TERMINAL_HR);
}

inline const char * StateCommand::getHelpText()
{
  return PSTR("Prints current state of the node.");
}

#endif //StateCommand_H