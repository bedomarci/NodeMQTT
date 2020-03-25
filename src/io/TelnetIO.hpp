#if !defined(TELNETIO_H) && (NODEMQTT_TRANSPORT == WIFI_TRANSPORT)
#define TELNETIO_H

#include "_AbstractIO.hpp"
#include "misc/helpers.hpp"

class TelnetIO : public AbstractIO
{
  public:
    TelnetIO();

    void boot(ApplicationContext *context) override;
    size_t write(uint8_t c) override;

  protected:
    void handle() override;
    void printHeader() override;
    void printErrorMessage();

  private:
    int i;
    int charsReceived = 0;
    char *_telnetBuffer;

    SERVER_CLASS * telnetServer;
    CLIENT_CLASS telnetClient;
    boolean connectFlag = false; //we'll use a flag separate from client.connected, so we can recognize when a new connection has been created
    unsigned long timeOfLastActivity; //time in milliseconds of last activity
    unsigned long allowedConnectTime = 300000; //five minutes
    void checkConnectionTimeout();
    void checkNewConnection();
    void getReceivedText();
    void loop();
};
inline TelnetIO::TelnetIO() {
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


inline void TelnetIO::boot(ApplicationContext *context)
{
    telnetServer = new SERVER_CLASS(TELNET_SERVER_PORT);
    //TODO: telnet begin must be called after transport setup. otherwise ESP32 fails on boot. event handler is required.
    telnetServer->begin();
    telnetServer->setNoDelay(true);
    AbstractIO::boot(context);
    loop();
}

inline void TelnetIO::checkConnectionTimeout()
{
  if(millis() - timeOfLastActivity > allowedConnectTime) {
    this->println(PSTR("Timeout disconnect."));
    telnetClient.stop();
    connectFlag = false;
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

      connectFlag = true;
      loop();
      this->printHeader();
      this->printPrompt();
    } else {
      telnetServer->available().stop();  // have client, block new conections
    }
  } else {
    if(!telnetClient.connected()) {
      if(connectFlag == true) {
        telnetClient.stop(); //client disconnects
        connectFlag = false;
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
  do {
    c = telnetClient.read();
    _telnetBuffer[charsReceived] = c;
    charsReceived++;
    charsWaiting--;
  }
  while(charsReceived <= TELNET_BUFFER_SIZE && c != '\n' && charsWaiting > 0);


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
    _telnetBuffer = new char[TELNET_BUFFER_SIZE];
    for (int i = 0; i < TELNET_BUFFER_SIZE; i++) _telnetBuffer[i] = '\0';
}

inline void TelnetIO::printErrorMessage()
{
  this->print(PSTR("Telnet buffer overload."));
}


#endif //TELNETIO_H