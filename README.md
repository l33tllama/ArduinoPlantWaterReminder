# ArduinoPlantWaterReminder

Arduino code to run my Plant Water Reminder device made for my YouTube video.

It uses the Pushover service to push notifications to a device.

Replace the credentials details in the top of the .ino file with your own before use.

Requires the Pushover Arduino library - https://github.com/ArduinoHannover/Pushover

For ESP32 users - you will need to modify the Pushover library and remove the client.setInsecure() command on line 47 in the Pushover.cpp file, as the ESP32 library does not contain that command (yet - but there is a new version coming out soon that will have it).

