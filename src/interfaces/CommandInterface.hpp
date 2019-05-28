#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include "StringInterface.hpp"
#include "../NodeMQTTCommandProcessor.hpp"

#define COMMAND_NUM 2

class CommandInterface : public StringInterface
{
  public:
    CommandInterface();
    void init();
    String getSubscribeTopic() override;
    String getPublishTopic() override;

  protected:
    void writeRaw(String newValue, bool publishable) override;
};

inline CommandInterface::CommandInterface() : StringInterface(COMMAND_TOPIC)
{
}

inline void CommandInterface::init() {
    this->setSamplingEnabled(false);
    this->setMQTTPublish(false);
}

inline void CommandInterface::writeRaw(String newValue, bool publishable) {
  NodeMQTTCommandProcessor.execute(newValue.c_str());
}
    
inline String CommandInterface::getSubscribeTopic() {
    return this->getBaseTopic();
}
inline String CommandInterface::getPublishTopic() {
    return this->getBaseTopic();
}

#endif //COMMANDINTERFACE_H