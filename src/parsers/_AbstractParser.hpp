#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H
#include "../misc/helpers.hpp"
// #include "../misc/typedef.hpp"

//// class NodeInterfaceBase;

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

// inline void AbstractParser::init() {
// }

// inline void AbstractParser::setContext(ApplicationContext* context) {
//     _context = context;
// }

// inline void AbstractParser::setInterfaces (LinkedList<NodeInterfaceBase *> *interfaces){
//     _interfaces = interfaces;
// }

#endif //ABSTRACTPARSER_H