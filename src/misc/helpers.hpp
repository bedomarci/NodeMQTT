#ifndef HELPERS_H
#define HELPERS_H
#include "../NodeMQTTLogger.hpp"


#define d(x)        Logger.log(x, DEBUG)
#define debug(x)    Logger.log(x, DEBUG)

#define i(x)        Logger.log(x, INFO)
#define info(x)     Logger.log(x, INFO)

#define w(x)        Logger.log(x, WARNING)
#define warning(x)  Logger.log(x, WARNING)

#define e(x)        Logger.log(x, ERROR)
#define error(x)    Logger.log(x, ERROR)

#endif