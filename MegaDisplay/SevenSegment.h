#ifndef __INCLUDE_SEVEN_SEGMENT_H__
#define __INCLUDE_SEVEN_SEGMENT_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CHECK_BIT_TO_HIGH_LOW(value, mask) ((((value) & (mask)) == (mask)) ? (HIGH) : (LOW))

class SevenSegmentDisplay
{
  public:
    SevenSegmentDisplay(const uint8_t p0, const uint8_t p1, const uint8_t p2, const uint8_t p3)
      : pin0(p0), pin1(p1), pin2(p2), pin3(p3)
    {
      pinMode(pin0, OUTPUT);
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(pin3, OUTPUT);
      setValue(0);
    }

    void setValue(const uint8_t value)
    {
      digitalWrite(pin0, CHECK_BIT_TO_HIGH_LOW(value, 1));
      digitalWrite(pin1, CHECK_BIT_TO_HIGH_LOW(value, 2));
      digitalWrite(pin2, CHECK_BIT_TO_HIGH_LOW(value, 4));
      digitalWrite(pin3, CHECK_BIT_TO_HIGH_LOW(value, 8));
    }

  private:
    const uint8_t pin0, pin1, pin2, pin3;
};

#endif // #ifndef __INCLUDE_SEVEN_SEGMENT_H__

