#include "NodeMQTTCommandProcessor.hpp"
#include "misc/helpers.hpp"

#include "commands/RestartCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ConfigCommand.hpp"
#include "commands/PollCommand.hpp"
#include "commands/FactoryResetCommand.hpp"

void NodeMQTTCommandProcessorClass::init(ApplicationContext *context)
{
    _context = context;
    loadDefaultCommands();
}
void NodeMQTTCommandProcessorClass::registerCommand(AbstractCommand *command)
{
    commands.add(command);
}

void NodeMQTTCommandProcessorClass::execute(const char *command)
{
    //SEPARATE PARAMETER FROM COMMANDWORD
    char *parameter = strchr(command, ':');
    if (parameter)
    {
        parameter[0] = '\0';
        parameter++;
    }

    //CASE INSENSITIVITY
    String sCommandWord = String(command);
    sCommandWord.toUpperCase();

    for (int i = 0; i < commands.size(); i++)
    {
        AbstractCommand *cmd = commands.get(i);
        if (strcmp_P(sCommandWord.c_str(), cmd->getCommandWord()) == 0)
        {
            if (parameter)
                cmd->setCommandParameter(parameter);
            cmd->handle();
            return;
        }
    }
    Logger.logf(WARNING, F("Unknown command! [%s]"), command);
    printHelp();
}

void NodeMQTTCommandProcessorClass::loadDefaultCommands()
{
    this->registerCommand(new HelpCommand(_context));
    this->registerCommand(new RestartCommand(_context));
    this->registerCommand(new ConfigCommand(_context));
    this->registerCommand(new FactoryResetCommand(_context));
    this->registerCommand(new PollCommand(_context));
}

void NodeMQTTCommandProcessorClass::printHelp()
{
    Serial.println(TERMINAL_HR);
    for (int i = 0; i < commands.size(); i++)
    {
        AbstractCommand *cmd = commands.get(i);
        String commandWord = FPSTR(cmd->getCommandWord()); 
        Serial.print(commandWord);
        if (commandWord.length()<8)
            Serial.print(TERMINAL_TAB);
        Serial.print(TERMINAL_TAB);
        Serial.println(FPSTR(cmd->getHelpText()));
    }
    Serial.println(TERMINAL_HR);
}

NodeMQTTCommandProcessorClass NodeMQTTCommandProcessor;