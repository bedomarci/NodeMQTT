#pragma once
#ifndef INTEGERINTERFACE_H
#define INTEGERINTERFACE_H
#include "_NodeInterface.hpp"


class IntegerInterface : public NodeInterface<int> {
public:
    IntegerInterface(String publishTopic, String subscribeTopic);
protected:
    
    int fromJSON (JsonObject& rootObject);
    JsonObject& toJSON (int value);
    int cmp(int oldValue, int newValue);
    virtual int sample()=0;
    virtual void updatePhisicalInterface(int newValue)=0;
};
#endif //INTEGERINTERFACE_H