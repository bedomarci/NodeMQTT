#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <Arduino.h>
#include "../misc/typedef.hpp"
#include "parsers/_AbstractParser.hpp"

struct ApplicationContext;
class AbstractCommand
{
public:
  explicit AbstractCommand(ApplicationContext *context);
  virtual void handle() = 0;
  virtual const char * getHelpText() = 0;
  void setCommandParameter(const char* parameter);
  const char * getCommandWord();
  const char * getCommandParameter();

protected:
  void setCommandWord(const char * commandWord);
  ApplicationContext * getContext();

private:
  const char * _commandWord;
  const char * _commandParameter;
  ApplicationContext *_context;
};

inline AbstractCommand::AbstractCommand(ApplicationContext *context)
{
  _context = context;
}

inline void AbstractCommand::setCommandWord( const char * commandWord) {
    _commandWord = commandWord;
}
inline const char * AbstractCommand::getCommandWord() {
    return  _commandWord;
}

inline void AbstractCommand::setCommandParameter(const char * commandParameter) {
    _commandParameter = commandParameter;
}
inline const char * AbstractCommand::getCommandParameter() {
    return  _commandParameter;
}

inline ApplicationContext * AbstractCommand::getContext() {
    return _context;
}

#endif //ABSTRACTCOMMAND_H