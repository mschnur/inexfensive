#ifndef __INCLUDE_UNO_READER_H__
#define __INCLUDE_UNO_READER_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "FencingTypes.h"

/*********************************** Pins *************************************/
// signals a change in the status of the other 6 pins
const uint8_t unoInterrupt_pin = 3;

// fencer A
const uint8_t unoOnTargetA_pin = 11;
const uint8_t unoOffTargetA_pin = 12;
const uint8_t unoSelfContactA_pin = 13;

// fencer B
const uint8_t unoOnTargetB_pin = 7;
const uint8_t unoOffTargetB_pin = 8;
const uint8_t unoSelfContactB_pin = 9;

/******************************************************************************/

typedef uint8_t UnoStatus;

const UnoStatus NO_CONTACT = 0;
const UnoStatus A_ON_TARGET_FLAG = 1;
const UnoStatus A_OFF_TARGET_FLAG = 2;
const UnoStatus A_SELF_CONTACT_FLAG = 4;
const UnoStatus B_ON_TARGET_FLAG = 8;
const UnoStatus B_OFF_TARGET_FLAG = 16;
const UnoStatus B_SELF_CONTACT_FLAG = 32;
const UnoStatus ON_TARGET_TOUCH_FLAG = A_ON_TARGET_FLAG | B_ON_TARGET_FLAG;
const UnoStatus OFF_TARGET_TOUCH_FLAG = A_OFF_TARGET_FLAG | B_OFF_TARGET_FLAG;
const UnoStatus TOUCH_FLAG = ON_TARGET_TOUCH_FLAG | OFF_TARGET_TOUCH_FLAG;

inline boolean onTarget(UnoStatus uStatus, FencerSide fencer)
{
  UnoStatus flag = ((fencer == FENCER_A) ? (A_ON_TARGET_FLAG) : (B_ON_TARGET_FLAG));
  return ((uStatus & flag) == flag);
}

inline boolean offTarget(UnoStatus uStatus, FencerSide fencer)
{
  UnoStatus flag = ((fencer == FENCER_A) ? (A_OFF_TARGET_FLAG) : (B_OFF_TARGET_FLAG));
  return ((uStatus & flag) == flag);
}

inline boolean selfContact(UnoStatus uStatus, FencerSide fencer)
{
  UnoStatus flag = ((fencer == FENCER_A) ? (A_SELF_CONTACT_FLAG) : (B_SELF_CONTACT_FLAG));
  return ((uStatus & flag) == flag);
}

inline boolean touchOccurred(UnoStatus uStatus)
{
  return ((uStatus & TOUCH_FLAG) != NO_CONTACT);
}

class UnoReader
{

  public:

    static UnoReader& getInstance()
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static UnoReader reader(unoInterrupt_pin,
                              unoOnTargetA_pin, unoOffTargetA_pin, unoSelfContactA_pin,
                              unoOnTargetB_pin, unoOffTargetB_pin, unoSelfContactB_pin);
      return reader;
    }

    inline boolean unoUpdateAvailable()
    {
      return unoStatusAvailable;
    }

    UnoStatus getUnoStatus();

  private:
    // private constructor so only the two instances can be created
    UnoReader(const uint8_t interruptPin,
              const uint8_t onTargetA, const uint8_t offTargetA, const uint8_t selfContactA,
              const uint8_t onTargetB, const uint8_t offTargetB, const uint8_t selfContactB)
      : intPin(interruptPin),
        onTargA(onTargetA), offTargA(offTargetA), selfA(selfContactA),
        onTargB(onTargetB), offTargB(offTargetB), selfB(selfContactB)
    {
      init();
    }

    void init();

    static void unoUpdate();

    // make copy constructor private
    UnoReader(UnoReader const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(UnoReader const&); // Don't implement

    const uint8_t intPin, onTargA, offTargA, selfA, onTargB, offTargB, selfB;

    // both of the following are initialized in the .cpp file
    static volatile boolean unoStatusAvailable;
    static volatile UnoStatus mostRecentStatus;
};



#endif // #ifndef __INCLUDE_UNO_READER_H__
