#ifndef CONFIGCOMMAND_H
#define CONFIGCOMMAND_H

#include "_AbstractCommand.hpp"
#include "misc/helpers.hpp"

class ConfigCommand : public AbstractCommand
{
public:
  ConfigCommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline ConfigCommand::ConfigCommand(ApplicationContext *context) :  AbstractCommand(context) {
    setCommandWord(PSTR("CONFIG"));
}

inline void ConfigCommand::handle()
{
    String baseTopic = String(getContext()->configuration->baseTopic);
    String topic = baseTopic + "/" + CONFIG_SUB_TOPIC;
    getContext()->parser->parse(&topic[0], getCommandParameter());
}

inline const char * ConfigCommand::getHelpText()
{
  return PSTR("Processes JSON formatted configuration message.");
}

#endif //CONFIGCOMMAND_H