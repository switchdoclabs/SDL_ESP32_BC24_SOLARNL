//
// Non volalile preferences functions
//

// Global preference variables

unsigned int currentLEDMode;


unsigned int darkLight;

unsigned int turnOnLight = 0;

String WiFi_SSID = "";
String WiFi_psk = "";

int ClockTimeOffsetToUTC = 0;



void resetPreferences()
{
  preferences.begin("BC24init", false);
  preferences.clear();
  preferences.end();

  ClockTimeOffsetToUTC = DEFAULTCLOCKTIMEOFFSETTOUTC;
  Serial.println("----Clearing Preferences---");


}

void writePreferences()
{
  preferences.begin("BC24init", false);


  preferences.putInt("currentLEDMode", currentLEDMode);

  preferences.putInt("darkLight", darkLight);

  preferences.putString("WiFi_SSID", WiFi_SSID);
  preferences.putString("WiFi_psk", WiFi_psk);

  Serial.print("ClockTimeOffsetToUTC=");
  Serial.println(ClockTimeOffsetToUTC);
  preferences.putInt("COffsetToUTC", ClockTimeOffsetToUTC);

   preferences.putString("adminPassword", adminPassword);


  preferences.end();

#ifdef BC24DEBUG
  Serial.println("----Writing Preferences---");
  Serial.print("currentLEDMode="); Serial.println(currentLEDMode);
  Serial.print("OLEDOn="); Serial.println(OLEDOn);
  Serial.print("darkLight="); Serial.println(darkLight);



  Serial.printf("SSID="); Serial.println(WiFi_SSID);
  Serial.printf("psk="); Serial.println(WiFi_psk);
  Serial.printf("COffsetToUTC="); Serial.println(ClockTimeOffsetToUTC);

      Serial.print("Admin Password:");
  Serial.println(adminPassword.substring(0, 2) + "******");

  Serial.println("--------------------------");

#endif


}

void readPreferences()
{
  preferences.begin("BC24init", false);

  currentLEDMode = preferences.getInt("currentLEDMode", 0);

  darkLight = preferences.getInt("darkLight", 1);

  WiFi_SSID = preferences.getString("WiFi_SSID", "");
  WiFi_psk = preferences.getString("WiFi_psk", "");
  ClockTimeOffsetToUTC = preferences.getInt("COffsetToUTC", DEFAULTCLOCKTIMEOFFSETTOUTC);


  adminPassword = preferences.getString("adminPassword", "admin");


  preferences.end();

#ifdef BC24DEBUG
  Serial.println("----Reading Preferences---");
  Serial.print("currentLEDMode="); Serial.println(currentLEDMode);
  Serial.print("OLEDOn="); Serial.println(OLEDOn);
  Serial.print("darkLight="); Serial.println(darkLight);

  Serial.printf("SSID="); Serial.println(WiFi_SSID);
  Serial.printf("psk="); Serial.println(WiFi_psk);
  Serial.printf("COffsetToUTC="); Serial.println(ClockTimeOffsetToUTC);


  Serial.print("Admin Password:");
  Serial.println(adminPassword.substring(0, 2) + "******");
 
  Serial.println("--------------------------");
#endif
}





