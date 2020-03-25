#include "NodeMQTTCommandProcessor.hpp"
#include "misc/typedef.hpp"

#include "commands/RestartCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/WhoAmICommand.hpp"
#include "commands/ConfigCommand.hpp"
#include "commands/PollCommand.hpp"
#include "commands/StateCommand.hpp"
#include "commands/FactoryResetCommand.hpp"
#include "commands/UpdateCommand.hpp"

void NodeMQTTCommandProcessorClass::init()
{
    loadDefaultCommands();
}

void NodeMQTTCommandProcessorClass::boot() {

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
            NodeMQTTIO.printPrompt();
            NodeMQTTIO.println(sCommandWord.c_str());
            if (parameter)
                cmd->setCommandParameter(parameter);
            cmd->handle();
            return;
        }
    }
    NodeMQTTIO.printPrompt();
    NodeMQTTIO.printf(PSTR("Unknown command: [%s]. Type HELP for available commands!\r\n"), command);
}

void NodeMQTTCommandProcessorClass::loadDefaultCommands()
{
    this->registerCommand(new HelpCommand(getContext()));
    this->registerCommand(new WhoAmICommand(getContext()));
    this->registerCommand(new RestartCommand(getContext()));
    this->registerCommand(new ConfigCommand(getContext()));
    this->registerCommand(new FactoryResetCommand(getContext()));
    this->registerCommand(new PollCommand(getContext()));
    this->registerCommand(new StateCommand(getContext()));
    this->registerCommand(new UpdateCommand(getContext()));
}

LinkedList<AbstractCommand*> * NodeMQTTCommandProcessorClass::getCommands() {
    return &commands;
}


NodeMQTTCommandProcessorClass NodeMQTTCommandProcessor;