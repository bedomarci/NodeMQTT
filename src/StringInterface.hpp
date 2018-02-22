#pragma once
#ifndef StringInterface_H
#define StringInterface_H
#include "_NodeInterface.hpp"


class StringInterface : public NodeInterface<String> {
public:
    StringInterface(String publishTopic, String subscribeTopic);
protected:
    
    String fromJSON (JsonObject& rootObject);
    JsonObject& toJSON (String value);
    int cmp(String oldValue, String newValue);
    virtual String sample()=0;
    virtual void updatePhisicalInterface(String newValue)=0;
};
#endif //StringInterface_H