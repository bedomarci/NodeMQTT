#include "_AbstractParser.hpp"
#include "../misc/typedef.hpp"

void AbstractParser::init() {
}

void AbstractParser::setContext(ApplicationContext* context) {
    _context = context;
}

void AbstractParser::setInterfaces (LinkedList<NodeInterfaceBase *> *interfaces){
    _interfaces = interfaces;
}