#ifndef NODECOMMANDPROCESSOR
#define NODECOMMANDPROCESSOR

#include "commands/_AbstractCommand.hpp"

#include <LinkedList.h>

class NodeMQTTCommandProcessorClass {
    public:
        void init(ApplicationContext* context);
        void execute(const char * command);
        void registerCommand(AbstractCommand* command);
        LinkedList<AbstractCommand*> * getCommands();
    protected:
        void loadDefaultCommands();
        LinkedList<AbstractCommand*> commands;
        ApplicationContext *_context;
};

extern NodeMQTTCommandProcessorClass NodeMQTTCommandProcessor;

#endif //NODECOMMANDPROCESSOR