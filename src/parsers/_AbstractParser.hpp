#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H
#include "../misc/helpers.hpp"

#include "../misc/typedefDeclaration.hpp"

class AbstractParser {
    public:
        void init();
        virtual void parse(const char* topic, const char* payload) = 0;
        void setContext(ApplicationContext* context);
        void setInterfaces (LinkedList<NodeInterfaceBase *> *interfaces);
    protected:
        LinkedList<NodeInterfaceBase *> *_interfaces;
        ApplicationContext * _context;
};

#endif //ABSTRACTPARSER_H