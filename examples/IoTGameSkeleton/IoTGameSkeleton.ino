#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <interfaces/StringInterface.hpp>

void onControlChange(String oldValue, String newValue);
void unlockGame();
void initGame();

NodeMQTT thisNode;
StringInterface *controlInterface;

void setup()
{
    controlInterface = new StringInterface("control");
    controlInterface->onChange(onControlChange);
    thisNode.addInterface(controlInterface);
    thisNode.begin();
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
}

void unlockGame()
{
    i("Game unlocked.");
}

void initGame()
{
    i("Game initialized.");
}