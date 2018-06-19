//
//   SDL_Arduino_SunControl Library
//   Version 1.1
//   SwitchDoc Labs   July 2017
//
//


#include "SDL_Arduino_SunControl.h"

void SDL_Arduino_SunControl::begin() {

  _ina3221.begin();
}

SDL_Arduino_SunControl::SDL_Arduino_SunControl(
  uint8_t addr /*= SDL_Arduino_SunControl_INA3221Address*/,
  uint8_t USBControlEnable /*= SDL_Arduino_SunControl_USBControlEnable*/,
  uint8_t USBControlControl /*= SDL_Arduino_SunControl_USBControlControl*/,
  uint8_t WatchDog_Done /*= SDL_Arduino_SunControl_WatchDog_Done*/,
  uint8_t WatchDog_Wake /*= SDL_Arduino_SunControl_WatchDog_Wake*/,
  uint8_t WatchDog_Use /* = SDL_Arduino_SunControl_WatchDog_Use*/)
{



  _addr = addr;
  _USBControlEnable = USBControlEnable;
  _USBControlControl = USBControlControl;
  _WatchDog_Done = WatchDog_Done;
  _WatchDog_Wake = WatchDog_Wake;
  _WatchDog_Use = WatchDog_Use;

  pinMode(_USBControlEnable, OUTPUT);
  pinMode(_USBControlControl, OUTPUT);

  if ( _WatchDog_Use == true)
  {

    pinMode(_WatchDog_Done, OUTPUT);
    pinMode( _WatchDog_Wake, OUTPUT);
  }

  _ina3221 = SDL_Arduino_INA3221(_addr);


}


float SDL_Arduino_SunControl::readChannelVoltage(uint8_t channel)
{

  float busvoltage = _ina3221.getBusVoltage_V(channel);
  float shuntvoltage = _ina3221.getShuntVoltage_mV(channel);
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  return loadvoltage;
};


float SDL_Arduino_SunControl::readChannelCurrent(uint8_t channel)
{


  float current_mA = _ina3221.getCurrent_mA(channel);
  return current_mA;

};


uint8_t SDL_Arduino_SunControl::setUSBEnable(uint8_t value)
{

  digitalWrite(_USBControlEnable, value);
  return value;
};


uint8_t SDL_Arduino_SunControl::setUSBControl(uint8_t value)
{
  digitalWrite(_USBControlControl, value);
  return value;
};



void SDL_Arduino_SunControl::useWatchDog()
{

  _WatchDog_Use = true;

  pinMode(_WatchDog_Done, OUTPUT);
  pinMode( _WatchDog_Wake, OUTPUT);

};


void SDL_Arduino_SunControl::patTheWatchDog()
{
  digitalWrite(_WatchDog_Done, true);
  delay(100);
  digitalWrite(_WatchDog_Done, false);

};



