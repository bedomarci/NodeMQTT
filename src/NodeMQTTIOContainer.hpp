#ifndef NODEMQTTIOCONTAINER_H
#define NODEMQTTIOCONTAINER_H

#include "Print.h"
#include <LinkedList.h>
#include "NodeMQTTCommandProcessor.hpp"
#include "io/_AbstractIO.hpp"
#include "io/SerialIO.hpp"
#include "io/TelnetIO.hpp"
#include "io/BluetoothIO.hpp"
#include "misc/typedef.hpp"

class NodeMQTTIOContainerClass : public Print {
    public:
        NodeMQTTIOContainerClass();
        void init(ApplicationContext *context);
        void addIO(AbstractIO * io);
        size_t write(uint8_t c);
        void printPrompt();

    protected:
        ApplicationContext * getContext();
    private:
        LinkedList<AbstractIO *> ioList;
        ApplicationContext *_context;
    
};

extern NodeMQTTIOContainerClass NodeMQTTIO;

#endif //NODEMQTTIOCONTAINER_H