#ifndef WHOAMICOMMAND_H
#define WHOAMICOMMAND_H

#include "_AbstractCommand.hpp"
#include "../NodeMQTTIOContainer.hpp"

class WhoAmICommand : public AbstractCommand
{
public:
  WhoAmICommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline WhoAmICommand::WhoAmICommand(ApplicationContext *context) :  AbstractCommand(context) {
    setCommandWord(PSTR("WHOAMI"));
}

inline void WhoAmICommand::handle()
{
    NodeMQTTIO.printf(message_Introduction, NodeMQTTConfigManager.getStringProperty(PROP_SYS_BASETOPIC).c_str(), toDateTimeString(FIRMWARE_BUILD_TIME).c_str(), FIRMWARE_BUILD_TIME);
    NodeMQTTIO.println();
}

inline const char * WhoAmICommand::getHelpText()
{
  return PSTR("Prints node identity.");
}

#endif //WHOAMICOMMAND_H