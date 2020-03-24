// [env:d1_mini]
// ;upload_port = 192.168.0.1
// ;upload_flags =
// ;    --auth=AUTH
// platform = espressif8266
// board = d1_mini
// framework = arduino
// board_build.f_cpu = 160000000L ;160MHZ cpu
// build_flags = -Wl,-Tesp8266.flash.4m.ld ;4M (3M SPIFFS)
// upload_speed = 921600
// monitor_speed = 115200
// lib_extra_dirs =
//     D:/Dropbox/!BUSINESS/escaperooms/libs
//     W:/Dropbox/!BUSINESS/escaperooms/libs
// lib_deps =
//     NodeMQTT

#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <interfaces/FiniteStateMachineInterface.hpp>
#include <interfaces/DFPlayerInterface.hpp>
#include <interfaces/BuzzerInterface.hpp>

#define DF_SERIAL_RX D7
#define DF_SERIAL_TX D8
#define PIN_BUZZER D5

void unlockGame();
void initGame();
void maintainGame();
void disableGame();
void boot();

NodeMQTT *thisNode;
FiniteStateMachineInterface *controlInterface;
DFPlayerInterface *playerInterface;
BuzzerInterface *buzzerInterface;

void setup()
{
    thisNode = new NodeMQTT();
    controlInterface = new FiniteStateMachineInterface("control");
    controlInterface->addState(0, "INIT", initGame, nullptr, nullptr, true);
    controlInterface->addState(1, "UNLOCK", unlockGame);
    controlInterface->addState(2, "MAINTAIN", maintainGame);
    controlInterface->addState(3, "DISABLE", disableGame);

    buzzerInterface = new BuzzerInterface("buz", PIN_BUZZER);
    playerInterface = new DFPlayerInterface("player", DF_SERIAL_RX, DF_SERIAL_TX);
    thisNode->addInterface(controlInterface);
    thisNode->addInterface(playerInterface);
    thisNode->addInterface(buzzerInterface);
    thisNode->setSystemBuzzer(buzzerInterface);
    boot();
    thisNode->begin();
}

void loop()
{
    thisNode->handle();
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

void maintainGame()
{
    i("Maintenance mode.");
}

void disableGame()
{
    i("Game disabled.");
}