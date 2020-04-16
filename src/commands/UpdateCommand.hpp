#ifndef UPDATECOMMAND_H
#define UPDATECOMMAND_H

#include "_AbstractCommand.hpp"
#include "../NodeMQTTUpdateManager.hpp"
#include "../NodeMQTTScheduler.hpp"

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
    //Need to run in main loop, otherwise http client always fails.
    NodeMQTTScheduler.runDelayed([](){
        NodeMQTTUpdateManager.checkForUpdates();
        },0);
}

inline const char * UpdateCommand::getHelpText()
{
  return PSTR("Pulls update from firmware server if available.");
}

#endif //UPDATECOMMAND_H