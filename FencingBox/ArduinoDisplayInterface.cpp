


/********************************* Includes ***********************************/
#include <SPI.h>
#include "DisplayInterface.h"


/******************************** Constants ***********************************/
const uint16_t BUZZER_TIME_MS = 1000;
const uint16_t LIGHT_AFTER_BUZZER_TIME_MS = 2000;

/*********************************** Pins *************************************/
const uint8_t pinSRData = 11; // shift register data pin (SPI MOSI pin)
const uint8_t pinSRClock = 13; // shift register clock pin (SPI SCK pin)
const uint8_t pinSRLatch = 10; // shift register latch pin (SPI SS pin)
const uint8_t pinSRNotClear = 12; // shift register !clear (clear, but active low) pin (digital)

const uint8_t pinInterruptMega = 13; // pin to signal to the mega that the other 6 pins are set

/****************************** Shift Register ********************************/
typedef uint8_t SRData;
const SRData CLEAR = 0;

const SRData OFF_TARGET_A_BIT = 0;
const SRData ON_TARGET_A_BIT = 1;
const SRData SELF_CONTACT_A_BIT = 2;

const SRData OFF_TARGET_B_BIT = 3;
const SRData ON_TARGET_B_BIT = 4;
const SRData SELF_CONTACT_B_BIT = 5;

const SRData BUZZER_BIT = 6;

SRData currentState = CLEAR;

/******************************************************************************/

void setTouchLEDStateInSR(boolean onTargetA, boolean offTargetA,
                          boolean onTargetB, boolean offTargetB);

void setBuzzerStateInSR(boolean buzzerOn);

void setSRState(SRData newState);

void setup_display()
{
  // set all pins used for display as outputs
  pinMode(pinSRData, OUTPUT);
  pinMode(pinSRLatch, OUTPUT);
  pinMode(pinSRClock, OUTPUT);
  pinMode(pinSRNotClear, OUTPUT);
  pinMode(pinInterruptMega, OUTPUT);

  // initialize all pins to low
  digitalWrite(pinSRData, LOW);
  digitalWrite(pinSRLatch, LOW);
  digitalWrite(pinSRClock, LOW);
  digitalWrite(pinSRNotClear, HIGH);
  digitalWrite(pinInterruptMega, LOW);

  // settings are based on SN74HC595N shift register specs
  SPI.beginTransaction(SPISettings(25000000, LSBFIRST, SPI_MODE0));
  setSRState(currentState);
}

void signalTouch(boolean onTargetA, boolean offTargetA,
                 boolean onTargetB, boolean offTargetB)
{
  // make sure we don't do anything if all of the parameters are false
  if (!(onTargetA || offTargetA || onTargetB || offTargetB)
      // make sure we don't have an invalid combination of parameters
      || (onTargetA && offTargetA)
      || (onTargetB && offTargetB))
  {
    return;
  }

  // interrupt the Arduino Mega to let it know to stop the timer and to not allow
  // any weapon changes while the touch is being displayed.  Since the interrupt
  // is caused on the rising edge, this signal can be (and is) kept high until
  // displaying this touch is completed.
  digitalWrite(pinInterruptMega, HIGH);

  SRData newState = currentState;
  // set the on and off target outputs according to the parameters
  bitWrite(newState, ON_TARGET_A_BIT, onTargetA);
  bitWrite(newState, OFF_TARGET_A_BIT, offTargetA);
  bitWrite(newState, ON_TARGET_B_BIT, onTargetB);
  bitWrite(newState, OFF_TARGET_B_BIT, offTargetB);
  
  // buzzer must also go off on a touch (on or off target), so set bit high
  bitWrite(newState, BUZZER_BIT, HIGH);
  setSRState(newState);
  
  // delay until the buzzer needs to be turned off (1 second)
  delay(BUZZER_TIME_MS);

  // then turn off the buzzer
  setBuzzerStateInSR(LOW);

  // now delay until the lights need to be turned off (2 seconds)
  delay(LIGHT_AFTER_BUZZER_TIME_MS);

  // then turn off the lights
  setTouchLEDStateInSR(LOW, LOW, LOW, LOW);

  // also lower the interrupt signal, since the displaying the touch is now finished
  digitalWrite(pinInterruptMega, LOW);
}

void updateSelfContact(boolean selfA, boolean selfB)
{
  SRData newState = currentState;
  bitWrite(newState, SELF_CONTACT_A_BIT, selfA);
  bitWrite(newState, SELF_CONTACT_B_BIT, selfB);
  setSRState(newState);
}

void updateBreakInControlCircuit(boolean breakA, boolean breakB)
{
  SRData newState = currentState;
  bitWrite(newState, OFF_TARGET_A_BIT, breakA);
  bitWrite(newState, OFF_TARGET_B_BIT, breakB);
  setSRState(newState);
}

void setTouchLEDStateInSR(boolean onTargetA, boolean offTargetA,
                          boolean onTargetB, boolean offTargetB)
{
  SRData newState = currentState;
  bitWrite(newState, ON_TARGET_A_BIT, onTargetA);
  bitWrite(newState, OFF_TARGET_A_BIT, offTargetA);
  bitWrite(newState, ON_TARGET_B_BIT, onTargetB);
  bitWrite(newState, OFF_TARGET_B_BIT, offTargetB);
  setSRState(newState);
}

void setBuzzerStateInSR(boolean buzzerOn)
{
  SRData newState = currentState;
  bitWrite(newState, BUZZER_BIT, buzzerOn);
  setSRState(newState);
}

void setSRState(SRData newState)
{
  digitalWrite(pinSRLatch, LOW);
  SPI.transfer(newState);
  digitalWrite(pinSRLatch, HIGH);
  currentState = newState;
}




