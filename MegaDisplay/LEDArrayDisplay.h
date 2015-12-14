#ifndef __INCLUDE_LED_ARRAY_DISPLAY_H__
#define __INCLUDE_LED_ARRAY_DISPLAY_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "FencingTypes.h"

/*********************************** Pins *************************************/
// LED Array A
static const uint8_t onTargetA_pin = 49;
static const uint8_t offTargetA_pin = 50;
static const uint8_t selfContactA_pin = 51;

// LED Array B
static const uint8_t onTargetB_pin = 46;
static const uint8_t offTargetB_pin = 47;
static const uint8_t selfContactB_pin = 48;

/******************************************************************************/
class LEDArrayDisplay
{
  public:

    static LEDArrayDisplay& getInstance(const FencerSide& side)
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static LEDArrayDisplay displayA(onTargetA_pin, offTargetA_pin, selfContactA_pin);
      static LEDArrayDisplay displayB(onTargetB_pin, offTargetB_pin, selfContactB_pin);

      switch (side)
      {
        default:
        case FENCER_A:
          return displayA;

        case FENCER_B:
          return displayB;
      }
    }

    inline void setOnTarget(boolean enabled) const
    {
      digitalWrite(onTarg, enabled);
    }

    inline void setOffTarget(boolean enabled) const
    {
      digitalWrite(offTarg, enabled);
    }

    inline void setSelfContact(boolean enabled) const
    {
      digitalWrite(self, enabled);
    }

  private:
    // private constructor so only the two instances can be created
    LEDArrayDisplay(const uint8_t onTarget, const uint8_t offTarget, const uint8_t selfContact)
      : onTarg(onTarget), offTarg(offTarget), self(selfContact)
    {
      pinMode(onTarg, OUTPUT);
      pinMode(offTarg, OUTPUT);
      pinMode(self, OUTPUT);
      digitalWrite(onTarg, LOW);
      digitalWrite(offTarg, LOW);
      digitalWrite(selfContact, LOW);
    }

    // make copy constructor private
    LEDArrayDisplay(LEDArrayDisplay const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(LEDArrayDisplay const&); // Don't implement

    const uint8_t onTarg, offTarg, self;
};

#endif // #ifndef __INCLUDE_LED_ARRAY_DISPLAY_H__

