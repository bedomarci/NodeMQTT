#ifndef ABSTRACTTRANSPORT_H
#define ABSTRACTTRANSPORT_H
#include "../constants.hpp"
#include "../misc/typedefDeclaration.hpp"
#include "Client.h"

class AbstractTransport
{
  public:
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual void connectNetwork() = 0;
    virtual bool isNetworkConnected() = 0;
    virtual String getNetworkAddressString() = 0;
    virtual void publish(const char *topic, const char *msg) = 0;
    virtual void subscribe(const char *topic) = 0;

    virtual String getState();
    
    void setContext(ApplicationContext *context);

    void setMessageCallback(NodeMQTTMessageCallback);

    void onMessage(char *, uint8_t *, unsigned int);

    Scheduler *getScheduler();
  protected:
    // AbstractTransport();

  private:
    ApplicationContext *_context;
    NodeMQTTMessageCallback messageCallback;
};
#endif //ABSTRACTTRANSPORT_H