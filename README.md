SDL_ESP32_BC24_SOLARNL<BR>
BC24 With the SunControl Solar Power <BR>
SwitchDoc Labs June 2018 <BR>
For the BC24 24 Pixel plus ESP32 Dual Core Processor with Grove Connectors

August 15, 2018:  V006:  Bug fixes<BR>
August 13, 2018:  V005:  Bug fixes, additional REST variables - Full Release<BR> 
August 12, 2018:  V004:  Additional REST Commands, fix to ESP32 Library Problem<BR>
June 21, 2018:  V003:  Improved REST response times.  Added I2C reliability improvement<BR>
June 19, 2018:  V002:   Initial Release June 19, 2018<BR>

Features:

- Uses the RMT peripherial on the ESP32 - No real CPU load for the Pixels

- Turns lights off if Solar Panels are above 2.0 V

REST interface included:

http://192.168.1.104/

(replace IP with the IP from your device).

Uses the SDL_ESP32_BC24_GETIP library (github.com/switchdoclabs)

<<<<<<< HEAD

<H2>Description</H2>

The Demo program has four different modes:<BR>
We have four goodies for you right out of the box.

- Rainbow Display Demo (Mode 0)
- Blue Theater Chasing Circle (Mode 1)
- Digital LED Clock (synced with NIST clocks for high accuracy) (Mode 2)

The program remmebers which selected via REST last.   Mode 2 requires a WiFi connection.  FIVE red lights will flash if you go to 2 without a WiFi Connection.

<H2>REST Commands</H2>

More documentation shortly on the REST commands

Try this:

http://192.168.1.104/<BR>

(replace IP with the IP from your device).

You should get something like this:

<pre>
{"variables": {"solarVoltage": 0.00, "batteryVoltage": 0.00, "solarCurrent": 0.00, "batteryCurrent": 0.00, "loadCurrent": 0.00, "loadVoltage": 0.00}, "id": "Â", "name": "SDL_ESP32_BC24_SOLARNL", "hardware": "esp32", "connected": true}
</pre>

And this to change the mode:

http://192.168.1.104/rainbow?parsms=admin<BR>

<pre>
{"return_value": 1, "id": "", "name": "SDL_ESP32_BC24_SOLARNL", "hardware": "esp32", "connected": true}
</pre>

http://192.168.1.104/clock?parsms=admin<BR>
<pre>
{"return_value": 1, "id": "", "name": "SDL_ESP32_BC24_SOLARNL", "hardware": "esp32", "connected": true}
</pre>
http://192.168.1.104/fire?parsms=admin<BR>
<pre>
{"return_value": 1, "id": "", "name": "SDL_ESP32_BC24_SOLARNL", "hardware": "esp32", "connected": true}
</pre>

(replace IP with the IP from your device).

<H2>Startup</H2>

On Startup the software looks for a previously set WiFI connection.  If it finds it then, you will see three green lights flash.  If it doesn't find the WiFi or has never been connected to a WiFi, then it starts though the WiFi detection sequence.

<H2>WiFi Connection Sequence - Provisioning</H2>

- SmartConfig<BR>
- WPS Button on AP<BR>
- Local AP on ESP32 (192.168.4.1)<BR>

Note:  This was developed for the BC24 ESP32 Based 24 RGBW Pixel LED Board <BR>

<H2>
SmartConfig<BR>
</H2>
This method uses the TI method of provisiong
"SmartConfig" was invented by TI. You can refer to it here:<BR>
https://community.particle.io/t/smart-config-the-missing-manual-now-available/442<BR>
In order to do SmartConfig, you need a smartphone or tablet (Android or iOS) that connected to WiFi network (which you want ESP32 to connect to) and installed a special application. 
On this application, you just supply the ssid and password of WiFi network so that the application can use, encode them and then broadcast (via UDP) encoded ssid and password (under packet format) over the air. 
When this software is being run, the ESP32 (with the SmartConfig software loaded) will capture these packets, decode back ssid and password and use them to connect to Wifi network. 

After connecting to WiFi, the  ESP32 will use mDNS to multicast a message to the application to notify that it has successfully connected to WiFi.

The source code of special application is supplied by Espressif. You can download at:<BR>
https://github.com/EspressifApp/EsptouchForAndroid<BR>
https://github.com/EspressifApp/EsptouchForIOS<BR>
This application is also available on App Store. You can use it to test SmartConfig feature.<BR>
- For Android, this application is available under name "IOT_Espressif" or another application "ESP8266 SmartConfig" (this is for ESP8266 but you can use it for ESP32):<BR>

https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch<BR>
https://play.google.com/store/apps/details?id=com.espressif.iot<BR>
<P>
- For iOS, this application is available under name "Espressif Esptouch":<BR>
https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8  <BR>

There is also another app on the iOS Appstore, search on "SmartConfig"<BR>
<h3> Color sequence </h3>
Default Time:  15 seconds to hit Smart Config on app, 30 seconds timeout on response<BR>
- Three White Lights<BR>
- Three Red on failure<BR>
- Three Green on success<BR>
<BR>
<H2>
WPS Button on AP<BR>
</H2>
Wi-Fi Protected Setup (WPS; originally, Wi-Fi Simple Config) is a network security standard to create a secure wireless home network.

Introduced in 2006, the goal of the protocol is to allow home users who know little of wireless security and may be intimidated by the available security options to set up Wi-Fi Protected Access, as well as making it easy to add new devices to an existing network without entering long passphrases.

This library will wait 60 seconds (in the example) for the WPS packets to be recieved by the ESP32.  

A major security flaw was revealed in December 2011 that affects wireless routers with the WPS PIN feature, which most recent models have enabled by default.

This software does not use the PIN feature.

<h3> Color sequence </h3>
Default Time:  30 seconds timeout if no WPS button sent from AP <BR>
- Three White Lights<BR>
- Two White Lights during search<BR>
- Two Red Lights on failure<BR>
- Two Green on success<BR>

<H2>
Local AP (192.168.4.1)<BR>
</H2>


For the third provisioning method, the ESP32 is set up as an access point (192.168.4.1) - look at your list of WiFi APs on your computer when it is running.   A small web server is started that will allow you to select the AP that you want the ESP32 to connect to and then you can enter the password for the access point.
It runs for 60 seconds by default.
<h3> Color sequence </h3>
Default Time:  60 seconds timeout if No completed Webform returned<BR>
- One Blink of single White Light <BR>
- One Blinking White Light during AP page active <BR>
- One Red Light on failure<BR>
- One Green on success<BR>

<B>Remember that all of these features may fail and have to be repeated.  Nature of the beast.</B>

<H2>
Software Notes<BR>
</H2>

<H3>Reset to Default state</H3>
Hold the Mode button down while you press and release Reset - erases stored information
=======
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



