#ifndef UPDATECOMMAND_H
#define UPDATECOMMAND_H

#include "_AbstractCommand.hpp"
#include "../NodeMQTTUpdateManager.hpp"

class UpdateCommand : public AbstractCommand
{
public:
  UpdateCommand(ApplicationContext *context);
  const char * getHelpText();
  void handle();

protected:
};

inline UpdateCommand::UpdateCommand(ApplicationContext *context) :  AbstractCommand(context) {
    setCommandWord(PSTR("UPDATE"));
}

inline void UpdateCommand::handle()
{
    // if (getContext() && this->getContext()->transport->isNetworkConnected())
      NodeMQTTUpdateManager.checkForUpdates();
    // else
      // e(F("Node is offline."));
}

inline const char * UpdateCommand::getHelpText()
{
  return PSTR("Pulls update from firmware server if available.");
}

#endif //UPDATECOMMAND_H