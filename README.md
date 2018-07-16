SDL_ESP32_BC24_SOLARNL<BR>
BC24 With the SunControl Solar Power <BR>
SwitchDoc Labs June 2018 <BR>
For the BC24 24 Pixel plus ESP32 Dual Core Processor with Grove Connectors

June 21, 2018:  V003:  Improved REST response times.  Added I2C reliability improvement<BR>
June 19, 2018:  V002:   Initial Release June 19, 2018<BR>

Features:

- Uses the RMT peripherial on the ESP32 - No real CPU load for the Pixels

- Turns lights off if Solar Panels are above 2.0 V

REST interface included:

http://192.168.1.139/

(replace IP with the IP from your device).

Uses the SDL_ESP32_BC24_GETIP library (github.com/switchdoclabs)

In order to install the Arduino IDE with support for the ESP32 on the BC24, please follow the tutorial here:

http://www.switchdoc.com/2018/07/tutorial-arduino-ide-esp32-bc24/

Select the Adafruit ESP32 Feather under Tools

If you get a "\SDL_ESP32_BC24DEMO\SDL_ESP32_BC24DEMO.ino:69:21: fatal error: TimeLib.h: No such file or directory"

Go to this link github.com/PaulStoffregen/Time and download the .zip file. Then, in the IDE, go to Sketch>Include Library and click on Add .ZIP Library... In your file download area, you should be able to find the Time-Master.zip. Click on it and Open. It will install the Time.h required for the compilation of the sketch. Try to compile. If you get a repeat error, ,then close the IDE and restart it. Then re-compiling should work.

Plug a USB Micro to USB connector into your computer (the one with the power supply will work) to establish the connection.

You can buy the BC24 here:<BR>
https://shop.switchdoc.com/products/the-bc24-esp32-based-24-rgbw-pixels-with-grove-connectors



All Libraries included in main directory

<img class="aligncenter size-large wp-image-29202" src="http://www.switchdoc.com/wp-content/uploads/2018/05/IMG_5852-copy.jpg" alt="" width="930" height="922" />



