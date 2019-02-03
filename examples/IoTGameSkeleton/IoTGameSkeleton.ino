// [env:d1_mini]
// upload_port = 192.168.0.1
// upload_flags =
//     --auth=AUTH
// platform = espressif8266
// board = d1_mini
// framework = arduino
// board_build.f_cpu = 160000000L ;160MHZ cpu
// build_flags = -Wl,-Tesp8266.flash.4m.ld ;4M (3M SPIFFS)
// upload_speed = 921600
// monitor_speed = 115200
// lib_extra_dirs =
//     C:/Users/bedom/Dropbox/!BUSINESS/escaperooms/libs
//     D:/Dropbox/!BUSINESS/escaperooms/libs
// lib_deps =
//     NodeMQTT
//     RF24
//     RF24Network
//     RF24Mesh
//     Wire
//     SPI

#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <interfaces/StringInterface.hpp>
#include <interfaces/DFPlayerInterface.hpp>

#define DF_SERIAL_RX D7
#define DF_SERIAL_TX D8

void onControlChange(String oldValue, String newValue);
void unlockGame();
void initGame();
void maintenanceMode();
void disableGame();
void boot();

NodeMQTT thisNode;
StringInterface *controlInterface;
DFPlayerInterface *playerInterface;

void setup()
{
    controlInterface = new StringInterface("control");
    controlInterface->onChange(onControlChange);
    playerInterface = new DFPlayerInterface("player", DF_SERIAL_RX, DF_SERIAL_TX);
    thisNode.addInterface(controlInterface);
    thisNode.addInterface(playerInterface);
    thisNode.begin();
    boot();
    initGame();
}

void loop()
{
    thisNode.handle();
}

void onControlChange(String oldValue, String newValue)
{
    if (newValue == "INIT")
    {
        initGame();
    }
    if (newValue == "UNLOCK")
    {
        unlockGame();
    }
    if (newValue == "MAINTENANCE")
    {
        maintenanceMode();
    }
    if (newValue == "DISABLE")
    {
        disableGame();
    }
}
void boot()
{
}

void initGame()
{
    i("Game initialized.");
}

void unlockGame()
{
    i("Game unlocked.");
}

void maintenanceMode()
{
    i("Maintenance mode.");
}

void disableGame()
{
    i("Game disabled.");
}
