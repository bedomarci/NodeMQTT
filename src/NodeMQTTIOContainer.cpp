#include "NodeMQTTIOContainer.hpp"

NodeMQTTIOContainerClass::NodeMQTTIOContainerClass () {
    ioList = LinkedList<AbstractIO *>();
}


void NodeMQTTIOContainerClass::init(ApplicationContext * context) {
    _context = context;
    #if IO_SERIAL
    this->addIO(new SerialIO());
    #endif 
    #if IO_TELNET
    this->addIO(new TelnetIO());
    #endif 
    #if IO_BLUETOOTH
    this->addIO(new BluetoothIO());
    #endif 
    
}

ApplicationContext * NodeMQTTIOContainerClass::getContext() {
    return _context;
}


void NodeMQTTIOContainerClass::addIO(AbstractIO * io) {
    ioList.add(io);
    io->onReceive([=](const char * m){NodeMQTTCommandProcessor.execute(m);});
    io->init(getContext());
}

size_t NodeMQTTIOContainerClass::write(uint8_t c) {
    size_t sTotal = 0;
    for (int i = 0; i < ioList.size(); i++){
        size_t s = ioList.get(i)->write(c);
        sTotal = (s > sTotal) ? s : sTotal;
    }
//        yield();
    return sTotal;
}

void NodeMQTTIOContainerClass::printPrompt() {
    this->print(TERMINAL_PROMPT);
}

NodeMQTTIOContainerClass NodeMQTTIO;