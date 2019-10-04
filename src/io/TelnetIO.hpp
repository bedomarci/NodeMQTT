#if !defined(TELNETIO_H) && (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)
#define TELNETIO_H

#include "_AbstractIO.hpp"
#include "misc/helpers.hpp"

class TelnetIO : public AbstractIO
{
  public:
    TelnetIO();
    virtual void init(ApplicationContext *context);
    size_t write(uint8_t c);

  protected:
    void handle();
    void printHeader();
    void printErrorMessage();

  private:
    int i;
    int charsReceived = 0;
    char *_telnetBuffer;

    TELNET_SERVER_CLASS * telnetServer;
    TELNET_CLIENT_CLASS telnetClient;
    boolean connectFlag = 0; //we'll use a flag separate from client.connected, so we can recognize when a new connection has been created
    unsigned long timeOfLastActivity; //time in milliseconds of last activity
    unsigned long allowedConnectTime = 300000; //five minutes
    void checkConnectionTimeout();
    void checkNewConnection();
    void getReceivedText();
    void loop();
};
inline TelnetIO::TelnetIO() {
    telnetServer = new TELNET_SERVER_CLASS(TELNET_SERVER_PORT);
}

inline void TelnetIO::printHeader(){
  ::printHeader(telnetClient);
  this->println("\nNodeMQTT  Telnet Server");
  this->println("Type HELP for help");
}

inline size_t TelnetIO::write(uint8_t c) {
  if (telnetClient)
    return telnetClient.write(c);
  return 0;
}


inline void TelnetIO::init(ApplicationContext *context)
{
    telnetServer->begin();
    telnetServer->setNoDelay(true);
    AbstractIO::init(context);
    loop();
}

inline void TelnetIO::checkConnectionTimeout()
{
  if(millis() - timeOfLastActivity > allowedConnectTime) {
    w("TIMEOUTTT");
    this->println("Timeout disconnect.");
    telnetClient.stop();
    connectFlag = 0;
  }
}

inline void TelnetIO::handle() {
    checkNewConnection();
    if (telnetClient.connected() && telnetClient.available()) getReceivedText();
    if(connectFlag) checkConnectionTimeout(); // check to see if connection has timed out
}

inline void TelnetIO::checkNewConnection() {
  if (telnetServer->hasClient()){ // client is connected
    if (!telnetClient || !telnetClient.connected()){
      if(telnetClient) telnetClient.stop();
      telnetClient = telnetServer->available(); // ready for new client

      connectFlag = 1;
      loop();
      this->printHeader();
      this->printPrompt();
    } else {
      telnetServer->available().stop();  // have client, block new conections
    }
  } else {
    if(!telnetClient.connected()) {
      if(connectFlag == 1) {
        telnetClient.stop(); //client disconnects
        connectFlag = 0;
      }
    }
  }
}

inline void TelnetIO::getReceivedText()
{
  
  char c;
  int charsWaiting = 0;

  // copy waiting characters into _telnetBuffer until _telnetBuffer full, LF received, or no more characters
  charsWaiting = telnetClient.available();
  // Serial.println(charsWaiting);
  do {
    c = telnetClient.read();
    // Serial.print(c);
    // Serial.print('\t');
    // Serial.println((int)c);
    _telnetBuffer[charsReceived] = c;
    charsReceived++;
    charsWaiting--;
  }
  while(charsReceived <= TELNET_BUFFER_SIZE && c != '\n' && charsWaiting > 0);
  // Serial.println(_telnetBuffer);


  //if LF found go look at received text and execute command
  if(c == '\n') {
    _telnetBuffer[charsReceived-1] = '\0';
    if (_telnetBuffer[charsReceived-2]=='\r') _telnetBuffer[charsReceived-2] = '\0'; //if CR was sent, remove.
    receive(_telnetBuffer);
    loop();
    printPrompt();
  }

  // if _telnetBuffer full without reaching a CR, print an error message
  if(charsReceived >= TELNET_BUFFER_SIZE) {
    printErrorMessage();
    loop();
    printPrompt();
  }
  // if _telnetBuffer not full and no CR, do nothing else;
  // go back to loop until more characters are received
  //free(_telnetBuffer);
}

inline void TelnetIO::loop() {
    timeOfLastActivity = millis();
    telnetClient.flush();
    charsReceived = 0;
    _telnetBuffer = new char[SERIAL_BUFFER_SIZE];
    for(int i = 0; i < SERIAL_BUFFER_SIZE; i++) _telnetBuffer[i] = '\0';
}

inline void TelnetIO::printErrorMessage()
{
  this->print(PSTR("Telnet buffer overload."));
}


#endif //TELNETIO_H