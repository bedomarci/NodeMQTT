#ifndef ABSTRACTCOMMAND_CPP
#define ABSTRACTCOMMAND_CPP

#include "_AbstractCommand.hpp"
#include "../misc/typedef.hpp"


AbstractCommand::AbstractCommand(ApplicationContext *context) {
    _context = context;
}

void AbstractCommand::setCommandWord(const char *commandWord) {
    _commandWord = commandWord;
}

const char *AbstractCommand::getCommandWord() {
    return _commandWord;
}

void AbstractCommand::setCommandParameter(const char *commandParameter) {
    _commandParameter = commandParameter;
}

const char *AbstractCommand::getCommandParameter() {
    return _commandParameter;
}

ApplicationContext *AbstractCommand::getContext() {
    return _context;
}

#endif //ABSTRACTCOMMAND_CPP