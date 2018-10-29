
// SDL_ESP32_BC24_SOLARNL
// SwitchDoc Labs BC24 Pixel ESP32 Board plus SunControl and Solar Panels
// June 2018
//
//

#define BC24SOLARNLSOFTWAREVERSION "010"
#define BC24DEBUG

// the three channels of the INA3221 named for SunControl Solar Power Controller channels (www.switchdoc.com)
#define LIPO_BATTERY_CHANNEL 1
#define SOLAR_CELL_CHANNEL 2
#define OUTPUT_CHANNEL 3

#define DEFAULTCLOCKTIMEOFFSETTOUTC -25200

#define BUTTONPIN 17

#define BLINKPIN 13

#define BC24


#include <Wire.h>
#include "SDL_Arduino_SunControl.h"

SDL_Arduino_SunControl SunControl;

String adminPassword;

#include "utility.h"

#include <Preferences.h>


/* create an instance of Preferences library */
Preferences preferences;


#include "BC24Preferences.h"

#include "RTOSDefs.h"


#include "BigCircleFunctions.h"

#include "NeoFire.h"

#if defined(ARDUINO) && ARDUINO >= 100
// No extras
#elif defined(ARDUINO) // pre-1.0
// No extras
#elif defined(ESP_PLATFORM)
#include "arduinoish.hpp"
#endif

int oldCurrentLEDMode = -1;
bool WiFiPresent = false;

#include <time.h>


#include <WiFi.h>



#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <TimeLib.h>

#include "Clock.h"



#include "NTPClient.h"

WiFiUDP ntpUDP;


NTPClient timeClient(ntpUDP);


#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

//gets called when WiFiManager enters configuration mode


void configModeCallback (WiFiManager *myWiFiManager)
//void configModeCallback ()
{

  Serial.println("Entered config mode");

  Serial.println(WiFi.softAPIP());

}

#define WEB_SERVER_PORT 80
String APssid;

String Wssid;
String WPassword;



String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}




#include <esp_wps.h>
#include <esp_smartconfig.h>


#define ESP_WPS_MODE WPS_TYPE_PBC

// Kludge for latest ESP32 SDK - July 1, 2018

#define WPS_CONFIG_INIT_DEFAULT(type) { \
    .wps_type = type, \
                .crypto_funcs = &g_wifi_default_wps_crypto_funcs, \
  }

esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(ESP_WPS_MODE);

#include "SDL_ESP32_BC24_GETIP.h"

// aREST

#include "MaREST.h"
// Create aREST instance
aREST rest = aREST();

#include "MaRESTFunctions.h"


// Create an instance of the server
WiFiServer server(WEB_SERVER_PORT);

// Variables to be exposed to the API
float RESTsolarVoltage;
float RESTsolarCurrent;

float RESTbatteryVoltage;
float RESTbatteryCurrent;

float RESTloadVoltage;
float RESTloadCurrent;

String SWVersion = BC24SOLARNLSOFTWAREVERSION;

// Declare functions to be exposed to the API
int ledControl(String command);

NeoFire fire;

#include "RTOSTasks.h"

/* Global variables */
void setup()
{



  char rtn = 0;
  Serial.begin(115200);  // Serial is used for debugging
  delay(1000);

  // set up button Pin
  pinMode (BUTTONPIN, INPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);  // Pull up to 3.3V on input - some buttons already have this done

  Serial.print("Button=");
  Serial.println(digitalRead(BUTTONPIN));

  // Now check for clearing Preferences on hold down of Mode pin on reboot
  if (digitalRead(BUTTONPIN) == 0)
  {

    resetPreferences();
  }

  // setup preferences
  readPreferences();

  oldCurrentLEDMode = -1;  // force first LED Mode Evaluation

  ClockTimeOffsetToUTC = -25200;

  pinMode(BLINKPIN, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println("--------------------");
  Serial.println("BC24 Solar Powered Night Light Software");
  Serial.println("--------------------");
  Serial.print("Version: ");
  Serial.println(BC24SOLARNLSOFTWAREVERSION);

  Serial.print("Compiled at:");
  Serial.print (__TIME__);
  Serial.print(" ");
  Serial.println(__DATE__);

  Serial.println("\r\npower on");

  Wire.begin();


  SunControl.begin();

  /*
    Calibration values; the default values of +/-32767 for each axis
    lead to an assumed magnetometer bias of 0. Use the Calibrate example
    program to determine appropriate values for your particular unit.
  */

  // Free heap on ESP32
  Serial.print("Free Heap Space on BC24:");
  Serial.println(ESP.getFreeHeap());

#ifdef BC24DEBUG
  Serial.print("CPU0 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(0));

  Serial.print("CPU1 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(1));
#endif

  // initalize our friend the BC24!
  //BC24inititialzeCircle();

  // setup BC24

  dumpSysInfo();
  getMaxMalloc(1 * 1024, 16 * 1024 * 1024);

  if (digitalLeds_initStrands(STRANDS, STRANDCNT)) {
    Serial.println("Init FAILURE: halting");
    while (true) {};
  }
  for (int i = 0; i < STRANDCNT; i++) {
    strand_t * pStrand = &STRANDS[i];
    Serial.print("Strand ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print((uint32_t)(pStrand->pixels), HEX);
    Serial.println();
#if DEBUG_ESP32_DIGITAL_LED_LIB
    dumpDebugBuffer(-2, digitalLeds_debugBuffer);
#endif
    digitalLeds_resetPixels(pStrand);
#if DEBUG_ESP32_DIGITAL_LED_LIB
    dumpDebugBuffer(-1, digitalLeds_debugBuffer);
#endif
  }
  Serial.println("BC24 Init complete");

  // Init variables and expose them to REST API

  rest.variable("solarVoltage", &RESTsolarVoltage);
  rest.variable("batteryVoltage", &RESTbatteryVoltage);
  rest.variable("solarCurrent", &RESTsolarCurrent);
  rest.variable("batteryCurrent", &RESTbatteryCurrent);
  rest.variable("loadCurrent", &RESTloadCurrent);
  rest.variable("loadVoltage", &RESTloadVoltage);

  rest.variable("currentLEDMode", &currentLEDMode);
  rest.variable("darkLight", &darkLight);


  rest.variable("Version", &SWVersion);

  // Function to be exposed
  rest.function("led", ledControl);
  rest.function("clock", clockControl);
  rest.function("rainbow", rainbowControl);

  rest.function("fire", fireControl);

  rest.function("setDarkLight", setDarkLight);
  rest.function("setClockTimeOffsetToUTC", setClockTimeOffsetToUTC);
  rest.function("setTurnOn", setTurnOn);
  rest.function("reset", setReset);

  rest.function("setAdminPassword", setAdminPassword);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("SDL_ESP32_BC24_SOLARNL");


  // RTOS for WiFi support

  xSemaphoreSingleBlink = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreSingleBlink);   // initialize
  xSemaphoreTake( xSemaphoreSingleBlink, 10);   // start with this off

  // Semaphore to control use of IR Resource


  xSemaphoreIRResource = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreIRResource);   // initialize with it available


  // RTOS


  xSemaphoreClock = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreClock);   // initialize
  xSemaphoreTake( xSemaphoreClock, 10);   // start with this off

  xSemaphoreFire = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreFire);   // initialize
  xSemaphoreTake( xSemaphoreFire, 10);   // start with this off

  xSemaphoreRESTCommand = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreRESTCommand);   // initialize with it on!
  //xSemaphoreTake( xSemaphoreRESTCommand, 10);   // start with this off


  xSemaphoreRainbow = xSemaphoreCreateBinary();
  xSemaphoreGive( xSemaphoreRainbow);   // initialize
  xSemaphoreTake( xSemaphoreRainbow, 10);   // start with this off


  Serial.println("RTOS Tasks Starting");



  xTaskCreatePinnedToCore(
    taskSingleBlink,          /* Task function. */
    "TaskSingleBlink",        /* String with name of task. */
    1000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core

  xTaskCreatePinnedToCore(
    taskButtonRead,          /* Task function. */
    "TaskButtonRead",        /* String with name of task. */
    10000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core


  xTaskCreatePinnedToCore(
    taskClock,          /* Task function. */
    "TaskClock",        /* String with name of task. */
    10000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core

  xTaskCreatePinnedToCore(
    taskFire,          /* Task function. */
    "TaskFire",        /* String with name of task. */
    2000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core

  xTaskCreatePinnedToCore(
    taskRESTCommand,          /* Task function. */
    "TaskRESTCommand",        /* String with name of task. */
    10000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    3,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core


  xTaskCreatePinnedToCore(
    taskRainbow,          /* Task function. */
    "TaskRainbow",        /* String with name of task. */
    20000,            /* Stack size in words. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL,             /* Task handle. */
    1);               // Specific Core



  //---------------------
  // Setup WiFi Interface
  //---------------------
  WiFiPresent = false;

  WiFi.begin();
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  // check for SSID



  if (WiFi_SSID.length() != 0)
  {
    // use existing SSID
    Serial.println("Using existing SSID/psk");

    Serial.printf("SSID="); Serial.println(WiFi_SSID);
    Serial.printf("psk="); Serial.println(WiFi_psk);
    WiFi.begin(WiFi_SSID.c_str(), WiFi_psk.c_str());
    //Wait for WiFi to connect to AP
    Serial.println("Waiting for Saved WiFi");
#define WAITFORCONNECT 10
    for (int i = 0; i < WAITFORCONNECT * 2; i++)
    {
      if (WiFi.status() == WL_CONNECTED)
      {

        Serial.println("");
        Serial.println("WiFI Connected.");
        WiFiPresent = true;
#ifdef BC24
        BC24ThreeBlink(Green, 1000);
#endif

        break;
      }

      Serial.print(".");
      WiFiPresent = false;
      BC24ThreeBlink(White, 1000);
    }
    Serial.println();

  }


  if (WiFiPresent == false)
  {
    // do SmartConfig
#define WAITFORSTART 15
#define WAITFORCONNECT 20

    WiFiPresent = SmartConfigGetIP(WAITFORSTART, WAITFORCONNECT);

  } // if not already programmed before





  if (WiFiPresent != true)
  {
#define WPSTIMEOUTSECONDS 60
    // now try WPS Button

    WiFiPresent = WPSGetIP(WPSTIMEOUTSECONDS);

  }

  if (WiFiPresent != true)
  {
#define APTIMEOUTSECONDS 60
    WiFiPresent = localAPGetIP(APTIMEOUTSECONDS);
  }


  if (WiFiPresent == true)
  {


    Serial.println("-------------");
    Serial.println("WiFi Connected");
    Serial.println("-------------");
    WiFi_SSID = WiFi.SSID();
    WiFi_psk = WiFi.psk();
    Serial.print("SSID=");
    Serial.println(WiFi_SSID);

    Serial.print("psk=");
    Serial.println(WiFi_psk);
  }
  else
  {
    Serial.println("-------------");
    Serial.println("WiFi NOT Connected");
    Serial.println("-------------");
  }






  if (WiFiPresent == true)
  {

    WiFi_SSID = WiFi.SSID();
    WiFi_psk = WiFi.psk();
  }
  else
  {
    Serial.println("No Wifi Present");
    BC24ThreeBlink(Red, 1000);
  }

  // write out preferences

  writePreferences();
  if (WiFiPresent == true)
  {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("WiFi Channel= ");
    Serial.println(WiFi.channel());
  }

  //---------------------
  // End Setup WiFi Interface
  //---------------------


  if (WiFiPresent == true)
  {
    // REST server

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());

    timeClient.setTimeOffset(ClockTimeOffsetToUTC);
    timeClient.setUpdateInterval(3600000);
    timeClient.update();


    time_t t = timeClient.getEpochTime();

    setTime(t);


    digitalClockDisplay();
  }




  Serial.print("RAM left: ");
  Serial.println(system_get_free_heap_size());

  blinkLED(2, 300);  // blink twice - OK!


}



void printSemaphoreStatus(String Description)


{

#ifdef BC24DEBUG
  Serial.println("------------------------");
  Serial.println(Description);
  Serial.println("------------------------");
  Serial.print("ClockSemaphore=");
  Serial.println(uxSemaphoreGetCount(xSemaphoreClock));
  Serial.print("FireSemaphore=");
  Serial.println(uxSemaphoreGetCount(xSemaphoreFire));
  Serial.print("RainbowSemaphore=");
  Serial.println(uxSemaphoreGetCount(xSemaphoreRainbow));
  Serial.print("xSemaphoreIRResource=");
  Serial.println(uxSemaphoreGetCount(xSemaphoreIRResource));
  Serial.println("------------------------");
#endif

}



void evaluatedCurrentLEDMode()
{




  printSemaphoreStatus("Before Eval");




  if (currentLEDMode != oldCurrentLEDMode)
  {

#ifdef BC24DEBUG
    Serial.print("evaluating Mode:");
    Serial.print(oldCurrentLEDMode);
    Serial.print("/");
    Serial.println(currentLEDMode);
#endif


    oldCurrentLEDMode = currentLEDMode;
    xSemaphoreTake( xSemaphoreClock, 10);   // start with this off
    xSemaphoreTake( xSemaphoreFire, 10);   // start with this off
    xSemaphoreTake( xSemaphoreRainbow, 10);   // start with this off
    // Take all and then give back the start one

    printSemaphoreStatus("Past Takes");

    strand_t * pStrand = &STRANDS[0];
    BC24clearStrip(pStrand);

    switch (currentLEDMode)
    {
      case BC24_LED_MODE_NO_ROTATE_RAINBOW:
        xSemaphoreGive( xSemaphoreRainbow);   // turn on
        break;

      case BC24_LED_MODE_NO_ROTATE_FIRE:
        xSemaphoreGive( xSemaphoreFire);   // turn on
        break;
      case BC24_LED_MODE_NO_ROTATE_CLOCK:
        xSemaphoreGive( xSemaphoreClock);   // turn on
        break;

      case BC24_WEATHER_MODE_NO_ROTATE_BLANK:   // no display
        break;

      default:

        break;



    }

  }
  printSemaphoreStatus("At End of Eval");

}

long loopCount = 0;
bool first = true;
int previousGoodCurrentMode = currentLEDMode;

char ptrTaskList[250];

void loop()
{




  if ((loopCount % 100) == 0)
    printValues();


  // Now check the sunlight.  If solar voltage is over 2.0V, then stop the LEDs


  RESTsolarVoltage = SunControl.readChannelVoltage(SOLAR_CELL_CHANNEL);
  RESTbatteryVoltage = SunControl.readChannelVoltage(LIPO_BATTERY_CHANNEL);
  RESTsolarCurrent = SunControl.readChannelCurrent(SOLAR_CELL_CHANNEL);
  RESTbatteryCurrent = SunControl.readChannelCurrent(LIPO_BATTERY_CHANNEL);

  RESTloadVoltage = SunControl.readChannelVoltage(OUTPUT_CHANNEL);
  RESTloadCurrent = SunControl.readChannelCurrent(OUTPUT_CHANNEL);

#ifdef BC24DEBUG

  Serial.print("solarVoltage=");
  Serial.println(RESTsolarVoltage);
  Serial.print("turnOnLight=");
  Serial.println(turnOnLight);
#endif

  if ((RESTsolarVoltage < 2.0) || (turnOnLight == 1))
    //  if ((RESTsolarVoltage > 2.0) || (turnOnLight == 1))  // debug
  {
#ifdef BC24DEBUG
    Serial.println("Display On");
#endif

    if (first == false)
    {
      currentLEDMode = previousGoodCurrentMode;
    }
    first = true;
    // now display the LED Mode



    evaluatedCurrentLEDMode();

  }
  else
  {

#ifdef BC24DEBUG
    Serial.println("Display Off");
#endif
    // clear display
    if (first)
    {

      previousGoodCurrentMode = currentLEDMode;
      currentLEDMode = BC24_WEATHER_MODE_NO_ROTATE_BLANK;

      oldCurrentLEDMode = -1;
      evaluatedCurrentLEDMode();

      currentLEDMode = previousGoodCurrentMode;
      vTaskDelay(3000 / portTICK_PERIOD_MS);

      //strand_t * pStrand = &STRANDS[0];
      //BC24clearStrip(pStrand);
      first = false;

    }



    if ((loopCount % 5) == 0)
      blinkLED(2, 300);  // blink twice - I'm still here!

  }
  loopCount++;



  vTaskDelay(1000 / portTICK_PERIOD_MS);

}



void printValues()
{

  Serial.println("\r\n**************");
  Serial.println( "------------------------------");
  Serial.println( "SunControl Voltages and Currents");
  Serial.println( "------------------------------");

  float current_mA1 = 0;
  float loadvoltage1 = 0;



  current_mA1 = SunControl.readChannelCurrent(LIPO_BATTERY_CHANNEL);
  loadvoltage1 = SunControl.readChannelVoltage(LIPO_BATTERY_CHANNEL);


  Serial.print("LIPO_Battery Load Voltage:  "); Serial.print(loadvoltage1); Serial.println(" V");
  Serial.print("LIPO_Battery Current 1:       "); Serial.print(current_mA1); Serial.println(" mA");
  Serial.println("");

  float current_mA2 = 0;
  float loadvoltage2 = 0;

  current_mA2 = -SunControl.readChannelCurrent(SOLAR_CELL_CHANNEL);
  loadvoltage2 = SunControl.readChannelVoltage(SOLAR_CELL_CHANNEL);

  Serial.print("Solar Cell Load Voltage 2:  "); Serial.print(loadvoltage2); Serial.println(" V");
  Serial.print("Solar Cell Current 2:       "); Serial.print(current_mA2); Serial.println(" mA");
  Serial.println("");


  float current_mA3 = 0;
  float loadvoltage3 = 0;

  current_mA3 = SunControl.readChannelCurrent(OUTPUT_CHANNEL);
  loadvoltage3 = SunControl.readChannelVoltage(OUTPUT_CHANNEL);


  Serial.print("Output Load Voltage 3:  "); Serial.print(loadvoltage3); Serial.println(" V");
  Serial.print("Output Current 3:       "); Serial.print(current_mA3); Serial.println(" mA");
  Serial.println("");


}


