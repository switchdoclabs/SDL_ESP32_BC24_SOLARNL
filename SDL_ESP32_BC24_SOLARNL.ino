
// SDL_ESP32_BC24_SOLARNL
// SwitchDoc Labs BC24 Pixel ESP32 Board plus SunControl and Solar Panels
// June 2018
//
//

#define BC24SOLARNLSOFTWAREVERSION "003"
#undef BC24DEBUG

// the three channels of the INA3221 named for SunControl Solar Power Controller channels (www.switchdoc.com)
#define LIPO_BATTERY_CHANNEL 1
#define SOLAR_CELL_CHANNEL 2
#define OUTPUT_CHANNEL 3


#define BUTTONPIN 17

#define BLINKPIN 13

#define BC24


#include <Wire.h>
#include "SDL_Arduino_SunControl.h"

SDL_Arduino_SunControl SunControl;

#include "utility.h"

#include "BigCircleFunctions.h"

#if defined(ARDUINO) && ARDUINO >= 100
// No extras
#elif defined(ARDUINO) // pre-1.0
// No extras
#elif defined(ESP_PLATFORM)
#include "arduinoish.hpp"
#endif


bool WiFiPresent = false;

#include <time.h>


#include <WiFi.h>



#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <TimeLib.h>

#include "Clock.h"

#include <Preferences.h>


#include "NTPClient.h"

WiFiUDP ntpUDP;


NTPClient timeClient(ntpUDP);

/* create an instance of Preferences library */
Preferences preferences;


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



#include "BC24Preferences.h"

#include <esp_wps.h>
#include <esp_smartconfig.h>


#define ESP_WPS_MODE WPS_TYPE_PBC

esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(ESP_WPS_MODE);

#include "SDL_ESP32_BC24_GETIP.h"

// aREST

#include "MaREST.h"
// Create aREST instance
aREST rest = aREST();

// Create an instance of the server
WiFiServer server(WEB_SERVER_PORT);

// Variables to be exposed to the API
float RESTsolarVoltage;
float RESTsolarCurrent;

float RESTbatteryVoltage;
float RESTbatteryCurrent;

float RESTloadVoltage;
float RESTloadCurrent;

// Declare functions to be exposed to the API
int ledControl(String command);

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

  // Function to be exposed
  rest.function("led", ledControl);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("SDL_ESP32_BC24_SOLARNL");

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

long loopCount = 0;


void handleRESTCalls()
{

  if (WiFiPresent == true)
  {

    // Handle REST calls
    WiFiClient client = server.available();

    /*
      while (!client.available()) {
      delay(1);
      }
    */
    if (client.available())
    {
      RESTsolarVoltage = SunControl.readChannelVoltage(SOLAR_CELL_CHANNEL);
      RESTbatteryVoltage = SunControl.readChannelVoltage(LIPO_BATTERY_CHANNEL);
      RESTsolarCurrent = SunControl.readChannelCurrent(SOLAR_CELL_CHANNEL);
      RESTbatteryCurrent = SunControl.readChannelCurrent(LIPO_BATTERY_CHANNEL);

      RESTloadVoltage = SunControl.readChannelVoltage(OUTPUT_CHANNEL);
      RESTloadCurrent = SunControl.readChannelCurrent(OUTPUT_CHANNEL);

      rest.handle(client);
    }
  }

}


void loop()
{

  handleRESTCalls();


  if ((loopCount % 1000) == 0)
    printValues();

  if ((loopCount % 50) == 0)
    blinkLED(2, 300);  // blink twice - I'm still here!

  // Now check the sunlight.  If solar voltage is over 2.0V, then stop the LEDs
  float solarVoltage;
  solarVoltage = SunControl.readChannelVoltage(SOLAR_CELL_CHANNEL);
  Serial.print("solarVoltage=");
  Serial.println(solarVoltage);

  if (solarVoltage < 2.0)
  {
    // now display a circle of LEDs
    // Note:   We split this up to allow checks for REST calls - This is not interrupt driven (should be in an RTOS task), but it should be.
    // BC24CircleRainbow();

    // Rainbows with REST checks inbetween specific functions
    strand_t * pStrand = &STRANDS[0];

    int latestQueueEntry;


    rainbow(pStrand, 0, 2000);

    handleRESTCalls();
    scanner(pStrand, 0, 2000);
    handleRESTCalls();
    scanner(pStrand, 1, 2000);

    handleRESTCalls();
    scanner(pStrand, 0, 2000);
    handleRESTCalls();
    rainbow(pStrand, 0, 2000);
    handleRESTCalls();

    rainbow(pStrand, 0, 2000);
    handleRESTCalls();

    scanner(pStrand, 5, 2000);
    handleRESTCalls();
    rainbow(pStrand, 0, 2000);
    handleRESTCalls();

    rainbow(pStrand, 0, 2000);

    handleRESTCalls();
    rainbow(pStrand, 0, 2000);
    digitalLeds_resetPixels(pStrand);


  }

  loopCount++;




  delay(100);  // delay for serial readability
}

void printValues()
{
  char report[80];
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


  Serial.println(report);

}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  digitalWrite(BLINKPIN, state);
  return 1;
}
