#ifndef NTPTIME_H
#define NTPTIME_H

#include "TaskSchedulerDeclarations.h"
#include "../typedef.hpp"
#include "../config.hpp"
#include "../helpers.hpp"
#include "time_macros.hpp"


#define NTP_PACKET_SIZE 48

class NTPTime {
    public:
        NTPTime();
        void init(ApplicationContext * context);
        void setTimeSyncedCallback(NodeMQTTCallback);
        void setTimeReceivedCallback(NodeMQTTCallback);

    protected:
        byte packetBuffer[NTP_PACKET_SIZE];
        const int timeZone = 0;
        // IPAddress timeServer;
        UDP Udp;
        Task _tTick;
        Task _tGetNtpTime;
        void tick();
        ApplicationContext * _context;
        void getNtpTime();
        void sendNTPpacket();
        void onTimeSynced();
        void onTimeReceived();

    NodeMQTTCallback timeSyncedCallback;
    NodeMQTTCallback timeReceivedCallback;
    private:
        uint32_t beginWait;
};

inline NTPTime::NTPTime() {
    _tTick.set(TASK_SECOND, TASK_FOREVER, [=](){this->tick();});
    _tGetNtpTime.set(TASK_SECOND/5, 50, [=](){this->getNtpTime();});
    Udp.begin(NTP_TIME_SERVER_PORT);
}

inline void NTPTime::init(ApplicationContext * context) {
    _context = context;
    // timeZone = _context->configuration->timezone;
    context->scheduler->addTask(_tTick);
    context->scheduler->addTask(_tGetNtpTime);
    _tGetNtpTime.restartDelayed(TASK_SECOND*5);
}

inline void NTPTime::getNtpTime()
{
    if (_tGetNtpTime.isFirstIteration()) {
        while (Udp.parsePacket() > 0) ; // discard any previously received packets
        sendNTPpacket(); //"Transmit NTP Request"
        // beginWait = millis();
    }
    // while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
        bool isFirstTime = !_context->currentTime;
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        time_t time  = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        _context->currentTime = time;
        Logger.logf(INFO, F("Time server synced. Current time is %s."), toTimeString(time).c_str());

        //EVENTS
        onTimeSynced();
        if (isFirstTime) //If time was 0, we have NTP for first time.
            onTimeReceived();

        _tTick.enable();
        _tGetNtpTime.disable();
        _tGetNtpTime.restartDelayed(NTP_UPDATE_INTERVAL);
    }
    // }

    if (_tGetNtpTime.isLastIteration()) {
        e(F("No NTP Response."));
        _tGetNtpTime.restartDelayed(NTP_UPDATE_INTERVAL/4);
    }
    // return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
inline void NTPTime::sendNTPpacket()
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  d(F("Requesting NTP time."));
  Udp.beginPacket(NTP_TIME_SERVER_ADDRESS, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

inline void NTPTime::tick() {
    _context->currentTime++;
}

inline void NTPTime::setTimeSyncedCallback(NodeMQTTCallback cb){
    timeSyncedCallback = cb;
}
inline void NTPTime::setTimeReceivedCallback(NodeMQTTCallback cb){
    timeReceivedCallback = cb;
}
inline void NTPTime::onTimeSynced(){
    if (timeSyncedCallback != nullptr)
        timeSyncedCallback();
}
inline void NTPTime::onTimeReceived(){
    if (timeReceivedCallback != nullptr)
        timeReceivedCallback();
}


#endif //NTPTIME_H