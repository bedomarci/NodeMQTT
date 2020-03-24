**Library is in experimental phase. Use it on your own risk!**
# NodeMQTT
NodeMQTT is an IoT platform for ESP8266 and ESP32 devices on Arduino platform. Helps you to create IoT devices in null time. Makes your device easyily remote controllable. Communicates with JSON via MQTT.
# Features
 - Interface classes for basic components
 - Task scheduler, cron jobs
 - Settings in EEPROM: dynamic key-value pairs.
 - CLI commands
 - OTA update
# Used libraries
 - [TaskScheduler](https://github.com/arkhipenko/TaskScheduler)
 - [DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini)
 - [LinkedList](https://github.com/ivanseidel/LinkedList)
 - [PubSubClient](https://github.com/knolleary/pubsubclient)
 - [ArduinoJson](https://arduinojson.org/)
 - [JC_Button](https://github.com/JChristensen/JC_Button)
 - [ccronexpr](https://github.com/staticlibs/ccronexpr)
# Versions
### v0.5.0
 - ESP32 compatibility
 - dynamic settings for all interface and user registrable custom properties
### v0.4.0
 - Scheduler helper for delayed callbacks. Runs on internal task scheduler.
 - Finite State Machine interface for state control of the device
### v0.3.0
 - NTP time.
### v0.2.0
### v0.1.0
 - OTA update
 - multiple CLI io: serial, telner, bluetooth
 - CLI accessible via unsecured Telnet
### v0.0.0
 - Initial Commit
 - Basic interface
# TODOs
 - secured connections
