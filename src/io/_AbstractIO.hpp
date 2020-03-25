#ifndef ABSTRACTIO_H
#define ABSTRACTIO_H

#include <Arduino.h>
#include <Print.h>
#include "../misc/config.hpp"
#include "../misc/typedefDeclaration.hpp"
#include <TaskSchedulerDeclarations.h>

class AbstractIO : public Print
{
public:
    void onReceive(const NodeMQTTStringCallback& cb);
    size_t write(uint8_t c) override = 0;

    virtual void boot(ApplicationContext *context);
protected:
    virtual void handle() = 0;
    virtual void printHeader() = 0;

    void printPrompt();
    ApplicationContext * getContext();
    void receive(const char * message);
    Task _task;
private:
    ApplicationContext *_context;
    NodeMQTTStringCallback _cb = nullptr;
};
#endif //ABSTRACTIO_H