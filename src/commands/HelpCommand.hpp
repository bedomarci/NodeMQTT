#ifndef HELPCOMMAND_H
#define HELPCOMMAND_H

#include "_AbstractCommand.hpp"
#include "misc/helpers.hpp"
#include "../NodeMQTTCommandProcessor.hpp"

class HelpCommand : public AbstractCommand
{
public:
  HelpCommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline HelpCommand::HelpCommand(ApplicationContext *context) :  AbstractCommand(context) {
  setCommandWord(PSTR("HELP"));
}

inline void HelpCommand::handle()
{
    NodeMQTTCommandProcessor.printHelp();
}

inline const char * HelpCommand::getHelpText()
{
  return PSTR("Prints this list of available commands.");
}

#endif //HELPCOMMAND_H