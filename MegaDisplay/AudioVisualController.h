/*
 * This class controls the LEDArrayDisplays and the speaker.
 */
#ifndef __INCLUDE_AUDIO_VISUAL_CONTROLLER_H__
#define __INCLUDE_AUDIO_VISUAL_CONTROLLER_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "FencingTypes.h"
#include "LEDArrayDisplay.h"


/*********************************** Pins *************************************/
// LED Array A
static const uint8_t buzzer_pin = 5;

/******************************** Constants ***********************************/
const unsigned long LIGHT_AFTER_BUZZER_TIME_US = 2000000;
const unsigned long BUZZER_TIME_US = 1000000;

/******************************************************************************/

class AudioVisualController
{
  public:

    static AudioVisualController& getInstance()
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static AudioVisualController controller(buzzer_pin);
      return controller;
    }

    void signalTouch(boolean onTargetA, boolean offTargetA,
                     boolean onTargetB, boolean offTargetB);

    void updateBreakInControlCircuit(boolean breakA, boolean breakB);

    void updateSelfContact(boolean selfA, boolean selfB);

  private:
    // private constructor so only the two instances can be created
    AudioVisualController(const uint8_t buzzerPin)
      : buzzer(buzzerPin)
    {
      init();
    }

    void init();
    
    static void disableBuzzer();
    
    static void disableTouchLights();

    // make copy constructor private
    AudioVisualController(AudioVisualController const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(AudioVisualController const&); // Don't implement

    const uint8_t buzzer;

    static const LEDArrayDisplay * ledDispA;
    static const LEDArrayDisplay * ledDispB;
};

#endif // #ifndef __INCLUDE_AUDIO_VISUAL_CONTROLLER_H__
