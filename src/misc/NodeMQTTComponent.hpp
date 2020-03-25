#ifndef NODEMQTT_NODEMQTTCOMPONENT_HPP
#define NODEMQTT_NODEMQTTCOMPONENT_HPP

#include "typedefDeclaration.hpp"

// An abstract class
class NodeMQTTComponent {
protected:
    ApplicationContext *getContext();

private:
    ApplicationContext *_context;
public:
    virtual void init() = 0;

    virtual void boot() = 0;

    void setContext(ApplicationContext *context);
};

inline void NodeMQTTComponent::setContext(ApplicationContext *context) {
    _context = context;
}

inline ApplicationContext *NodeMQTTComponent::getContext() {
    return _context;
}


#endif //NODEMQTT_NODEMQTTCOMPONENT_HPP
