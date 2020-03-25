#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <Arduino.h>
#include "../misc/typedefDeclaration.hpp"
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

#endif //ABSTRACTCOMMAND_H