#ifndef RESTARTCOMMAND_H
#define RESTARTCOMMAND_H

#include "_AbstractCommand.hpp"

class RestartCommand : public AbstractCommand
{
public:
  RestartCommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline RestartCommand::RestartCommand(ApplicationContext *context) :  AbstractCommand(context) {
  setCommandWord(PSTR("RESTART"));
}

inline void RestartCommand::handle()
{
  ESP.restart();
}
inline const char * RestartCommand::getHelpText()
{
  return PSTR("Restarts node.");
}

#endif //RESTARTCOMMAND_H