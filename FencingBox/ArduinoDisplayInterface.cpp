


/********************************* Includes ***********************************/
#include <SPI.h>
#include <TimerOne.h>
#include "DisplayInterface.h"


/******************************** Constants ***********************************/
const unsigned long LIGHT_AFTER_BUZZER_TIME_US = 2000000;
const unsigned long BUZZER_TIME_US = 1000000;

/*********************************** Pins *************************************/
const uint8_t pinSRData = 11; // shift register data pin (SPI MOSI pin)
const uint8_t pinSRClock = 13; // shift register clock pin (SPI SCK pin)
const uint8_t pinSRLatch = 10; // shift register latch pin (SPI SS pin)
const uint8_t pinSRNotClear = 12; // shift register !clear (clear, but active low) pin (digital)

const uint8_t pinInterruptMega = 1; // pin to signal to the mega that the other 6 pins are set

/****************************** Shift Register ********************************/
typedef uint8_t SRData;
const SRData CLEAR = 0;

const SRData BUZZER_BIT = 0;

const SRData SELF_CONTACT_B_BIT = 1;
const SRData ON_TARGET_B_BIT = 2;
const SRData OFF_TARGET_B_BIT = 3;

const SRData SELF_CONTACT_A_BIT = 4;
const SRData ON_TARGET_A_BIT = 5;
const SRData OFF_TARGET_A_BIT = 6;

SRData currentState = CLEAR;

/******************************************************************************/

volatile boolean displayingTouch = false;
volatile boolean signalingTimeExpired = false;

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

  // setup TimerOne
  Timer1.initialize();
  Timer1.stop();

  // settings are based on SN74HC595N shift register specs
  SPI.beginTransaction(SPISettings(25000000, LSBFIRST, SPI_MODE0));
  setSRState(currentState);
}

// FOR DEBUG PURPOSES ONLY! Tests the functionality and order of the LED arrays and speaker
//  via the shift register.
void lightTest()
{
  while (true)
  {
    digitalWrite(pinSRLatch, LOW);
    SPI.transfer(0);
    digitalWrite(pinSRLatch, HIGH);
    delay(1000);
    for (uint8_t i = 0; i < 8; i++)
    {
      uint8_t shiftVal = 0;
      bitSet(shiftVal, i);
      digitalWrite(pinSRLatch, LOW);
      SPI.transfer(shiftVal);
      digitalWrite(pinSRLatch, HIGH);
      delay(1000);
    }
  }
}

void disableTouchLightsISR()
{
  // turn off the lights
  setTouchLEDStateInSR(LOW, LOW, LOW, LOW);

  // also lower the interrupt signal, since the displaying the touch is now finished
  digitalWrite(pinInterruptMega, LOW);

  // indicate that we are done displaying a touch internally (so FencingBox.ino can see it)
  displayingTouch = false;

  Timer1.stop();
  Timer1.detachInterrupt();
}

void disableBuzzerISR()
{
  // turn off the buzzer
  setBuzzerStateInSR(LOW);

  signalingTimeExpired = false;
}

void disableBuzzerThenLightsISR()
{
  // turn off the buzzer
  setBuzzerStateInSR(LOW);

  // set up an interrupt to call disableTouchLightsISR() in LIGHT_AFTER_BUZZER_TIME_US
  // microseconds (2 sec)
  Timer1.attachInterrupt(disableTouchLightsISR,
                         LIGHT_AFTER_BUZZER_TIME_US);
  Timer1.resume();
}

void signalTimeExpired()
{
  // make sure we are not already displaying a touch (this shouldn't happen, but just in case)
  if (!displayingTouch && !signalingTimeExpired)
  {
    /*
     * I don't need to signal the Arduino Mega to stop the timer, because this call will have
     * been prompted by the timer running out, at which point it will automatically stop.
     */

    signalingTimeExpired = true;

    // only buzzer needs to go off, so set buzzer bit high
    setBuzzerStateInSR(HIGH);

    // set up an interrupt to call disableBuzzerISR() in BUZZER_TIME_US microseconds (1 sec)
    Timer1.attachInterrupt(disableBuzzerISR,
                           BUZZER_TIME_US);
    Timer1.resume();
  }
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

  // interrupt the Arduino Mega to let it know to stop the timer.  This
  // signal is kept high until displaying this touch is completed, because
  // the way it is handled in the Mega allows this, and it guarantees the Mega
  // will not miss this signal.
  digitalWrite(pinInterruptMega, HIGH);

  // indicate that we are displaying a touch internally (so FencingBox.ino can see it)
  displayingTouch = true;

  SRData newState = currentState;
  // set the on and off target outputs according to the parameters
  bitWrite(newState, ON_TARGET_A_BIT, onTargetA);
  bitWrite(newState, OFF_TARGET_A_BIT, offTargetA);
  bitWrite(newState, ON_TARGET_B_BIT, onTargetB);
  bitWrite(newState, OFF_TARGET_B_BIT, offTargetB);

  // buzzer must also go off on a touch (on or off target), so set bit high
  bitWrite(newState, BUZZER_BIT, HIGH);
  setSRState(newState);

  // set up an interrupt to call disableBuzzerThenLightsISR() in BUZZER_TIME_US microseconds (1 sec)
  Timer1.attachInterrupt(disableBuzzerThenLightsISR,
                         BUZZER_TIME_US);
  Timer1.resume();
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
