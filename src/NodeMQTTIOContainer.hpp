#ifndef NODEMQTTIOCONTAINER_H
#define NODEMQTTIOCONTAINER_H

#include "Print.h"
#include <LinkedList.h>
#include "NodeMQTTCommandProcessor.hpp"
#include "misc/NodeMQTTComponent.hpp"
#include "misc/typedef.hpp"
#include "io/SerialIO.hpp"
#include "io/TelnetIO.hpp"
#include "io/BluetoothIO.hpp"
#include "io/_AbstractIO.hpp"

class NodeMQTTIOContainerClass : public Print, public NodeMQTTComponent {
public:
    NodeMQTTIOContainerClass();

    void init() override;

    void boot() override;

    void addIO(AbstractIO *io);

    size_t write(uint8_t c);

    void printPrompt();

private:
    LinkedList<AbstractIO *> ioList;

};

extern NodeMQTTIOContainerClass NodeMQTTIO;

#endif //NODEMQTTIOCONTAINER_H