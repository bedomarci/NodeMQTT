#ifndef HELPCOMMAND_H
#define HELPCOMMAND_H

#include "_AbstractCommand.hpp"
#include "misc/helpers.hpp"
#include "../NodeMQTTCommandProcessor.hpp"
#include "../NodeMQTTIOContainer.hpp"

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
    LinkedList<AbstractCommand*> * commands = NodeMQTTCommandProcessor.getCommands();

    NodeMQTTIO.println(TERMINAL_HR);
    for (int i = 0; i < commands->size(); i++)
    {
        AbstractCommand *cmd = commands->get(i);
        String commandWord = FPSTR(cmd->getCommandWord()); 
        NodeMQTTIO.print(commandWord);
        if (commandWord.length()<8)
            NodeMQTTIO.print(TERMINAL_TAB);
        NodeMQTTIO.print(TERMINAL_TAB);
        NodeMQTTIO.println(FPSTR(cmd->getHelpText()));
    }
    NodeMQTTIO.println(TERMINAL_HR);

}

inline const char * HelpCommand::getHelpText()
{
  return PSTR("Prints this list of available commands.");
}

#endif //HELPCOMMAND_H