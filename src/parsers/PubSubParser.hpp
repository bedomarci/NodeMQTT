#ifndef PUBSUBPARSER_H
#define PUBSUBPARSER_H
#include "_AbstractParser.hpp"

class PubSubParser : public AbstractParser {
    public:
        void parse(const char* topic, const char* payload);
    protected:
};

inline void PubSubParser::parse(const char* topic, const char* payload) {
    NodeInterfaceBase *interface;
    String s_payload = String(payload);
    for (int i = 0; i < _interfaces->size(); i++)
    {
        interface = _interfaces->get(i);
        if (strcmp(interface->getSubscribeTopic().c_str(), topic) == 0)
        {
            interface->writeRaw(s_payload, false);
        }
    }
}


#endif //ABSTRACTPARSER_H