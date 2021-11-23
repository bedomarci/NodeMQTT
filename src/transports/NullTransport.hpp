#if !defined(NULLTRANSPORT_H) && (NODEMQTT_TRANSPORT == NULL_TRANSPORT)
#define NULLTRANSPORT_H

#include "_AbstractTransport.hpp"

class NullTransport : public AbstractTransport
{
public:
    void init();
    void loop();
    void connectNetwork();
    bool isNetworkConnected();
    String getNetworkAddressString();
    void publish(const char *topic, const char *msg);
    void subscribe(const char *topic);
};

inline void NullTransport::init() {}

inline void NullTransport::loop() {}

inline void NullTransport::connectNetwork() {}

inline bool NullTransport::isNetworkConnected() { return false; }

inline String NullTransport::getNetworkAddressString() { return "NULL TRANSPORT"; }

inline void NullTransport::publish(const char *topic, const char *msg) {}

inline void NullTransport::subscribe(const char *topic) {}

#endif //NULLTRANSPORT_H