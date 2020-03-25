#ifndef NODECOMMANDPROCESSOR
#define NODECOMMANDPROCESSOR

#include "commands/_AbstractCommand.hpp"

#include <LinkedList.h>
#include <misc/NodeMQTTComponent.hpp>

class NodeMQTTCommandProcessorClass : public NodeMQTTComponent {
    public:
    void init();

    void boot();
        void execute(const char * command);
        void registerCommand(AbstractCommand* command);
        LinkedList<AbstractCommand*> * getCommands();
    protected:
        void loadDefaultCommands();
        LinkedList<AbstractCommand*> commands;
};

extern NodeMQTTCommandProcessorClass NodeMQTTCommandProcessor;

#endif //NODECOMMANDPROCESSOR