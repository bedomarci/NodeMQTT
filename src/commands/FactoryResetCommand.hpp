#ifndef FACTORYRESETCOMMAND_H
#define FACTORYRESETCOMMAND_H

#include "_AbstractCommand.hpp"

class FactoryResetCommand : public AbstractCommand
{
  public:
    FactoryResetCommand(ApplicationContext *context);
    const char *getHelpText();
    void handle();

  protected:
};

inline FactoryResetCommand::FactoryResetCommand(ApplicationContext *context) : AbstractCommand(context)
{
    setCommandWord(PSTR("FACTORYRESET"));
}

inline void FactoryResetCommand::handle()
{
    NodeMQTTConfigManager.factoryReset();
}
inline const char *FactoryResetCommand::getHelpText()
{
    return PSTR("Erases configuration from eeprom.");
}

#endif //FACTORYRESETCOMMAND_H