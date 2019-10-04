#include "_AbstractParser.hpp"
#include "../misc/typedef.hpp"
#include "TaskSchedulerDeclarations.h"

void AbstractParser::init() {
}

void AbstractParser::setContext(ApplicationContext* context) {
    _context = context;
}

void AbstractParser::setInterfaces (LinkedList<NodeInterfaceBase *> *interfaces){
    _interfaces = interfaces;
}