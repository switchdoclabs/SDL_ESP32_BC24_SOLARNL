//
//
//
// aRest Command functions
//
//


//
// BC24 Control functions
//


int clockControl(String command)
{
  Serial.println("clockControl");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);

  if (sentPassword == adminPassword)
  {

    currentLEDMode = BC24_LED_MODE_NO_ROTATE_CLOCK;

    writePreferences();
    //BC24CheckForClear();
    return 1;
  }
  return 0;

}


int rainbowControl(String command)
{
  Serial.println("rainbowControl");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);

  if (sentPassword == adminPassword)
  {

    currentLEDMode = BC24_LED_MODE_NO_ROTATE_RAINBOW;

    writePreferences();
    //BC24CheckForClear();
    return 1;
  }
  return 0;

}


int fireControl(String command)
{
  Serial.println("fireControl");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);

  if (sentPassword == adminPassword)
  {

    currentLEDMode = BC24_LED_MODE_NO_ROTATE_FIRE;

    writePreferences();
    //BC24CheckForClear();
    return 1;
  }
  return 0;

}


int setTurnOn(String command)
{
  Serial.println("setTurnOnl");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);
  value = getValue(command, ',', 1);
  if (sentPassword == adminPassword)
  {


    int myValue = value.toInt();

    if (myValue == 0)
    {

      turnOnLight = 0;
      return 1;
    }
    else if (myValue == 1)
    {
      turnOnLight = 1;
      return 1;
    }
    return 0;
  }
  return 0;

}


int setClockTimeOffsetToUTC(String command)
{
  Serial.println("ClockTimeOffsetToUTC");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);
  value = getValue(command, ',', 1);

  if (sentPassword == adminPassword)
  {

    int myValue = value.toInt();

    ClockTimeOffsetToUTC = myValue;

    timeClient.setTimeOffset(ClockTimeOffsetToUTC);
    timeClient.forceUpdate();
    time_t t;
    t = timeClient.getEpochTime();
    setTime(t);


    writePreferences();
    return 1;
  }
  return 0;
}


int setDarkLight(String command)
{
  Serial.println("setDarkLight");
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String value;

  sentPassword = getValue(command, ',', 0);
  value = getValue(command, ',', 1);

  if (sentPassword == adminPassword)
  {

    int myValue = value.toInt();


    // validate, set to On if it doesn't make sense
    if (myValue == 0)
      darkLight = myValue;
    else if (myValue == 1)
      darkLight = myValue;
    else
      myValue = 1;

    setUpDarkLight();
    writePreferences();
    return 1;
  }
  return 0;
}

// other functions
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  if (state == 0)  // ESP8266 inverts sense (HIGH = off, LOW = ON)
    state = 1;
  else
    state = 0;

  digitalWrite(BLINKPIN, state);
  return 1;
}

int setWeatherPlusIDControl(String command) {



  rest.set_id(command);
  return 1;
}

int resetWiFiAccessPoint(String command)
{
  Serial.print("Command =");
  Serial.println(command);




  if (command == adminPassword)
  {
    WiFi_psk = "XXX";
    WiFi_SSID = "XXX";
    writePreferences();
    return 1;
  }
  else
    return 0;


  //http://192.168.1.134/resetWiFiAccessPoint?params=adminpassword
  return 1;


}

int resetBC24Weather(String command) {

  Serial.println("resetBC24Weather - settings invalidated");
  Serial.print("Command =");
  Serial.println(command);
  if (command == adminPassword)
  {

    resetPreferences();

    system_restart();

    // qdelay(10000);

    return 1;
  }
  return 0;
}

int setAdminPassword(String command)
{
  Serial.print("Command =");
  Serial.println(command);


  String oldPassword;
  String neWiFi_psk;

  oldPassword = getValue(command, ',', 0);
  neWiFi_psk = getValue(command, ',', 1);

  if (oldPassword == adminPassword)
  {
    adminPassword = neWiFi_psk;
    writePreferences();
    return 1;
  }
  else
    return 0;


  //http://192.168.1.134/setAdminPassword?params=oldpassword,neWiFi_psk
  return 1;

}

int setReset(String command) {
  Serial.println("setReset");
 Serial.print("Command =");
  Serial.println(command);




  if (command == adminPassword)
  {
    ESP.restart();
    return 1;
  }
  else
    return 0;


}

int rebootBC24Weather(String command) {



  system_restart();
  // ESP.reset();
  delay(10000);

  return 1;
}

