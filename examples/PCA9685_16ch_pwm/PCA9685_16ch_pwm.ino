
#include <Arduino.h>
#include <NodeMQTT.hpp>
#include <interfaces/PCA9685Interface.hpp>

void boot();

void pwmTest();

NodeMQTT *thisNode;
PCA9685Interface *pwmInterface;

uint16_t pwmValue = 0;

Task tPWMTest(TASK_SECOND / 100, TASK_FOREVER, &pwmTest);

void setup() {
    thisNode = new NodeMQTT();
    Wire.begin();

    pwmInterface = new PCA9685Interface("pwm", &Wire);

    thisNode->addInterface(pwmInterface);
    thisNode->addTask(tPWMTest);
    boot();
    thisNode->begin();
}

void pwmTest() {
    pwmValue += 32;
    Array<uint16_t, 16> arr;
    for (uint8_t pin = 0; pin < 16; pin++) {
        uint16_t val = pwmValue + (pin * 512);
        arr[pin] = (val & 0b0000111111111111);
    }
    pwmInterface->write(arr);
}

void loop() {
    thisNode->handle();
}

void boot() {
    tPWMTest.enable();
    pwmInterface->invert(true);
}