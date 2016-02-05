

/********************************* Includes ***********************************/
#include "DisplayInterface.h"


/******************************** Constants ***********************************/
const uint16_t BUZZER_TIME_MS = 1000;
const uint16_t LIGHT_AFTER_BUZZER_TIME_MS = 2000;

/*********************************** Pins *************************************/
const uint8_t pin_on_target_A = 6;     // Fencer A on target (digital)
const uint8_t pin_off_target_A = 7;   // Fencer A off target (digital)
const uint8_t pin_self_contact_A = 8; // Fencer A self contact (digital)

const uint8_t pin_on_target_B = 9;    // Fencer B on target (digital)
const uint8_t pin_off_target_B = 10;   // Fencer B off target (digital)
const uint8_t pin_self_contact_B = 11; // Fencer B self contact (digital)

const uint8_t pin_buzzer = 12; // buzzer (digital)

const uint8_t pin_interrupt_mega = 13; // pin to signal to the mega that the other 6 pins are set

/******************************************************************************/

void setup_display()
{
  // set all pins used for display as outputs
  pinMode(pin_on_target_A, OUTPUT);
  pinMode(pin_off_target_A, OUTPUT);
  pinMode(pin_self_contact_A, OUTPUT);
  pinMode(pin_on_target_B, OUTPUT);
  pinMode(pin_off_target_B, OUTPUT);
  pinMode(pin_self_contact_B, OUTPUT);
  pinMode(pin_interrupt_mega, OUTPUT);
  pinMode(pin_buzzer, OUTPUT);

  // initialize all pins to low
  digitalWrite(pin_on_target_A, LOW);
  digitalWrite(pin_off_target_A, LOW);
  digitalWrite(pin_self_contact_A, LOW);
  digitalWrite(pin_on_target_B, LOW);
  digitalWrite(pin_off_target_B, LOW);
  digitalWrite(pin_self_contact_B, LOW);
  digitalWrite(pin_interrupt_mega, LOW);
  digitalWrite(pin_buzzer, LOW);
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

  // first set the on and off target outputs according to the parameters
  digitalWrite(pin_on_target_A, onTargetA);
  digitalWrite(pin_off_target_A, offTargetA);
  digitalWrite(pin_on_target_B, onTargetB);
  digitalWrite(pin_off_target_B, offTargetB);

  // buzzer must go off on a touch (on or off target), so set that pin high
  digitalWrite(pin_buzzer, HIGH);

  // interrupt the Arduino Mega to let it know to stop the timer and to not allow
  // any weapon changes while the touch is being displayed.  Since the interrupt
  // is caused on the rising edge, this signal can be (and is) kept high until
  // displaying this touch is completed.
  digitalWrite(pin_interrupt_mega, HIGH);

  // delay until the buzzer needs to be turned off (1 second)
  delay(BUZZER_TIME_MS);

  // then turn off the buzzer
  digitalWrite(pin_buzzer, LOW);

  // now delay until the lights need to be turned off (2 seconds)
  delay(LIGHT_AFTER_BUZZER_TIME_MS);

  // then turn off the lights
  digitalWrite(pin_on_target_A, LOW);
  digitalWrite(pin_off_target_A, LOW);
  digitalWrite(pin_on_target_B, LOW);
  digitalWrite(pin_on_target_B, LOW);

  // also lower the interrupt signal, since the displaying the touch is now finished
  digitalWrite(pin_interrupt_mega, LOW);
}

void updateSelfContact(boolean selfA, boolean selfB)
{
  digitalWrite(pin_self_contact_A, selfA);
  digitalWrite(pin_self_contact_B, selfB);
}

void updateBreakInControlCircuit(boolean breakA, boolean breakB)
{
  digitalWrite(pin_off_target_A, breakA);
  digitalWrite(pin_off_target_B, breakB);
}




