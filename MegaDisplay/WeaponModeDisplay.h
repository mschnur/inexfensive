#ifndef __INCLUDE_WEAPON_MODE_DISPLAY_H__
#define __INCLUDE_WEAPON_MODE_DISPLAY_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SevenSegment.h"
#include "FencingTypes.h"

/*********************************** Pins *************************************/
static const uint8_t epee_pin = A8;
static const uint8_t foil_pin = A9;
static const uint8_t saber_pin = A10;

/******************************************************************************/

class WeaponModeDisplay
{
  public:
    static WeaponModeDisplay& getInstance()
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static WeaponModeDisplay instance(epee_pin, foil_pin, saber_pin);
      return instance;
    }
  
    void switchMode()
    {
      switch (currentMode)
      {
        case EPEE:
          currentMode = FOIL;
          digitalWrite(ePin, LOW);
          digitalWrite(fPin, HIGH);
          digitalWrite(sPin, LOW);
          break;

        case FOIL:
          currentMode = SABER;
          digitalWrite(ePin, LOW);
          digitalWrite(fPin, LOW);
          digitalWrite(sPin, HIGH);
          break;

        case SABER:
        default:
          currentMode = EPEE;
          digitalWrite(ePin, HIGH);
          digitalWrite(fPin, LOW);
          digitalWrite(sPin, LOW);
          break;
      }
    }

    inline WeaponType getCurrentMode()
    {
      return currentMode;
    }

  private:
    // private constructor so that only the single instance can be constructed
    WeaponModeDisplay(const uint8_t epeePin, const uint8_t foilPin, const uint8_t saberPin)
      : ePin(epeePin), fPin(foilPin), sPin(saberPin), currentMode(EPEE)
    {
      // set up pins (output mode)
      pinMode(ePin, OUTPUT);
      pinMode(fPin, OUTPUT);
      pinMode(sPin, OUTPUT);

      // start in epee mode
      digitalWrite(ePin, HIGH);
      digitalWrite(fPin, LOW);
      digitalWrite(sPin, LOW);
    }

    // make copy constructor private
    WeaponModeDisplay(WeaponModeDisplay const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(WeaponModeDisplay const&); // Don't implement

    const uint8_t ePin, fPin, sPin;
    WeaponType currentMode;

};



#endif // #ifndef __INCLUDE_WEAPON_MODE_DISPLAY_H__

