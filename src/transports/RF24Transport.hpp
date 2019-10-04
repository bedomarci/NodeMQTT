#if !defined(RF24TRANSPORT_H) && (NODEMQTT_TRANSPORT == RF_TRANSPORT)
#define RF24TRANSPORT_H


#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include "_AbstractTransport.hpp"
#include "../misc/helpers.hpp"

class RF24Transport : public AbstractTransport
{
public:
  void init();
  void loop();
  void connectNetwork();
  bool isNetworkConnected();
  void publish(const char *topic, const char *msg);
  void subscribe(const char *topic);

  void write(const char *payload);

protected:
  RF24 *radio;
  RF24Network *network;
  RF24Mesh *mesh;
  char buffer[RF24_MAX_MESSAGE_SIZE];
};
void RF24Transport::init()
{
  radio = new RF24(RF24_CE_PIN, RF24_CS_PIN);
  network = new RF24Network(*radio);

  mesh = new RF24Mesh(*radio, *network);
  mesh->setNodeID(1);
}

void RF24Transport::connectNetwork()
{
  d(F("Connecting to the mesh..."));
  onNetworkConnecting();
  mesh->begin();
  onNetworkConnected();
  onBrokerConnecting();
  onBrokerConnected();
}

void RF24Transport::publish(const char *topic, const char *msg)
{
  strncpy(buffer, topic, RF24_MAX_MESSAGE_SIZE);
  strncat(buffer, RF24_MESSAGE_DELIMITER, RF24_MAX_MESSAGE_SIZE);
  strncat(buffer, msg, RF24_MAX_MESSAGE_SIZE);
  write(buffer);
}

void RF24Transport::subscribe(const char *topic)
{
  write(topic);
}

void RF24Transport::loop()
{
  mesh->update();
  mesh->DHCP();

  // Check for incoming data from the sensors
  // if(network->available()){
  //   RF24NetworkHeader header;
  //   network->peek(header);

  //   uint32_t dat=0;
  //   switch(header.type){
  //     // Display the incoming millis() values from the sensor nodes
  //     case 'M': network.read(header,&dat,sizeof(dat)); Serial.println(dat); break;
  //     default: network.read(header,0,0); Serial.println(header.type);break;
  //   }
  // }
}

void RF24Transport::write(const char *payload)
{
  if (!mesh->write(payload, 0, RF24_MAX_MESSAGE_SIZE))
  {
    if (!mesh->checkConnection())
    {
      i(F("Renewing Address."));
      mesh->renewAddress();
    }
    else
    {
      e(F("Send fail, Test OK."));
    }
  }
  else
  {
    onNetworkDisconnected();
  }
}

#endif //RF24TRANSPORT_H