#include "NodeMQTTCommandProcessor.hpp"
#include "NodeMQTTIOContainer.hpp"

#include "commands/RestartCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/WhoAmICommand.hpp"
#include "commands/ConfigCommand.hpp"
#include "commands/PollCommand.hpp"
#include "commands/StateCommand.hpp"
#include "commands/FactoryResetCommand.hpp"
#include "commands/UpdateCommand.hpp"
#include "misc/helpers.hpp"

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
    this->registerCommand(new HelpCommand(_context));
    this->registerCommand(new WhoAmICommand(_context));
    this->registerCommand(new RestartCommand(_context));
    this->registerCommand(new ConfigCommand(_context));
    this->registerCommand(new FactoryResetCommand(_context));
    this->registerCommand(new PollCommand(_context));
    this->registerCommand(new StateCommand(_context));
    this->registerCommand(new UpdateCommand(_context));
}

LinkedList<AbstractCommand*> * NodeMQTTCommandProcessorClass::getCommands() {
    return &commands;
}

NodeMQTTCommandProcessorClass NodeMQTTCommandProcessor;