#ifndef NODECONFIGINTERFACE_H
#define NODECONFIGINTERFACE_H

#include "_NodeInterface.hpp"
#include "NodeMQTTConfigManager.hpp"



class NodeConfigInterface : public NodeInterface<NodeMQTTConfig> {
    public:
        NodeConfigInterface();
    private:
        NodeMQTTConfig sample();
        void updatePhisicalInterface(NodeMQTTConfig newValue);
        NodeMQTTConfig fromJSON (JsonObject& rootObject);
        JsonObject& toJSON (NodeMQTTConfig value);
        int cmp(NodeMQTTConfig oldValue, NodeMQTTConfig newValue);
};

#endif //NODECONFIGINTERFACE_H