#ifndef HEARTBEATINTERFACE_H
#define HEARTBEATINTERFACE_H

#include "_NodeInterface.hpp"

struct NodeStatus {
  unsigned long uptime;
  const char * nodeID;
  uint32_t freeHeap = 0;
};

class HeartbeatInterface : public NodeInterface<NodeStatus> {
    public:
        HeartbeatInterface(int beatIntervalSecond);
    private:
        NodeStatus sample();
        void updatePhisicalInterface(NodeStatus newValue);
        NodeStatus fromJSON (JsonObject& rootObject);
        JsonObject& toJSON (NodeStatus value);
        int cmp(NodeStatus oldValue, NodeStatus newValue);
};

#endif //HEARTBEATINTERFACE_H