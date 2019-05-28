#ifndef POLLCOMMAND_H
#define POLLCOMMAND_H

#include "_AbstractCommand.hpp"


class PollCommand : public AbstractCommand
{
public:
    PollCommand(ApplicationContext *context);
    const char * getHelpText();
    void handle();

protected:

};

inline PollCommand::PollCommand(ApplicationContext *context) :  AbstractCommand(context) {
  setCommandWord(PSTR("POLL"));
}

inline void PollCommand::handle()
{
    for (int i = 0; i < getContext()->interfaces->size(); i++) {
        NodeInterfaceBase *interface = getContext()->interfaces->get(i);
        interface->republish();
    }
}

inline const char * PollCommand::getHelpText()
{
  return PSTR("Publishes current states of all publishable interfaces.");
}

#endif //POLLCOMMAND_H