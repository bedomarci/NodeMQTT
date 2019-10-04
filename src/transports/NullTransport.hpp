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

inline void init() {}
inline void loop() {}
inline void connectNetwork() {
    void onNetworkConnecting();
    void onNetworkConnected();
    void onBrokerConnecting();
    void onBrokerConnected();
}
inline bool isNetworkConnected() {}
inline String getNetworkAddressString() {}
inline void publish(const char *topic, const char *msg) {}
inline void subscribe(const char *topic) {}

#endif //NULLTRANSPORT_H