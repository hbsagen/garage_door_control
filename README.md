Garage door control

An Arduino project for detecting if the garage door is open or closed, and displaying this with a webserver and a button for open/close

Parts:
Arduino Mega 2560
5 VDC relay
Rangefinder, sonic
ESP8266

Mounting:
Above the door when it's open. But with free sight of the floor when it's closed

Working principle:
It creates a webserver at ip:8888, stauts is displayed, and a button presented. Clicking the button wil activate the relay for 1 sec. Connect thte relay to your garage openers proper point. Page refresh every 5 sec, adjsut IP at line 156 to the current one.

Insert your SSID and password at line 28.

Currently installed with a Liftmaster.
