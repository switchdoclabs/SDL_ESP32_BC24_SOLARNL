//
//   SDL_Arduino_SunControl Library
//   Version 1.1
//   SwitchDoc Labs   July 2017
//
//


#include "Arduino.h"

#include "SDL_Arduino_INA3221.h"

#define SDL_Arduino_SunControl_INA3221Address  (0x40)

// USB Control GPIO Pins

#define SDL_Arduino_SunControl_USBControlEnable (9)
#define SDL_Arduino_SunControl_USBControlControl (8)

// the three channels of the INA3221 named for SunControl Solar Power Controller channels (www.switchdoc.com)
#define SDL_Arduino_SunControl_LIPO_BATTERY_CHANNEL (1)
#define SDL_Arduino_SunControl_SOLAR_CELL_CHANNEL   (2)
#define SDL_Arduino_SunControl_OUTPUT_CHANNEL       (3)
// WatchDog Values

#define SDL_Arduino_SunControl_WatchDog_Done  (7)
#define SDL_Arduino_SunControl_WatchDog_Wake  (6)

#define SDL_Arduino_SunControl_WatchDog_Use  false

class SDL_Arduino_SunControl {

  public:
    SDL_Arduino_SunControl(uint8_t addr = SDL_Arduino_SunControl_INA3221Address,
                           uint8_t USBControlEnable = SDL_Arduino_SunControl_USBControlEnable,
                           uint8_t USBControlControl = SDL_Arduino_SunControl_USBControlControl,
                           uint8_t WatchDog_Done = SDL_Arduino_SunControl_WatchDog_Done,
                           uint8_t WatchDog_Wake = SDL_Arduino_SunControl_WatchDog_Wake,
                           uint8_t WatchDog_Use  = SDL_Arduino_SunControl_WatchDog_Use);

    void begin(void);
    float readChannelVoltage(uint8_t channel);
    float readChannelCurrent(uint8_t channel);

    uint8_t setUSBEnable(uint8_t value);
    uint8_t setUSBControl(uint8_t value);

    void useWatchDog();
    void patTheWatchDog();




  private:

    uint8_t _addr;
    uint8_t _USBControlEnable;
    uint8_t _USBControlControl;
    uint8_t _WatchDog_Done;
    uint8_t _WatchDog_Wake;
    uint8_t _WatchDog_Use;

   SDL_Arduino_INA3221 _ina3221;



};

