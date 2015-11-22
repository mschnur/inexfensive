

/********************************* Includes ***********************************/
#include "display_interface.h"


/******************************** Constants ***********************************/
#define LIGHT_TIME_MS 3000
#define BUZZER_TIME_MS 1000

/*********************************** Pins *************************************/
const uint8_t pin_on_target_A = 9;     // Fencer A on target (digital)
const uint8_t pin_off_target_A = 10;   // Fencer A off target (digital)
const uint8_t pin_self_contact_A = 11; // Fencer A self contact (digital)

const uint8_t pin_on_target_B = 3;    // Fencer B on target (digital)
const uint8_t pin_off_target_B = 5;   // Fencer B off target (digital)
const uint8_t pin_self_contact_B = 6; // Fencer B self contact (digital)

const uint8_t pin_buzzer = 0;

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
  pinMode(pin_buzzer, OUTPUT);
}

void signal_touch(boolean on_target_A, boolean off_target_A,
                  boolean on_target_B, boolean off_target_B)
{
  // make sure we don't do anything if all of the parameters are false
  if (!(on_target_A || off_target_A || on_target_B || off_target_B))
  {
    return;
  }

  // first set the on and off target outputs according to the parameters
  digitalWrite(pin_on_target_A, on_target_A);
  digitalWrite(pin_off_target_A, off_target_A);
  digitalWrite(pin_on_target_B, on_target_B);
  digitalWrite(pin_off_target_B, off_target_B);

  // self contact isn't displayed when a touch or off target is being displayed,
  // so ensure that those pins are low
  digitalWrite(pin_self_contact_A, LOW);
  digitalWrite(pin_self_contact_B, LOW);

  // buzzer must go off on a touch (on or off target), so set that pin high
  digitalWrite(pin_buzzer, HIGH);

  // delay until the buzzer should stop going off
  delay(BUZZER_TIME_MS);
  // then turn off the buzzer
  digitalWrite(pin_buzzer, LOW);

  // delay until the lights should stop being on
  // (only the time remaining after the buzzer time, hence the subtraction)
  delay(LIGHT_TIME_MS - BUZZER_TIME_MS);
  // then turn off the lights
  digitalWrite(pin_on_target_A, LOW);
  digitalWrite(pin_off_target_A, LOW);
  digitalWrite(pin_on_target_B, LOW);
  digitalWrite(pin_off_target_B, LOW);
}

void signal_self_contact(Fencer fencer, boolean inContact)
{
  switch (fencer)
  {
    case FENCER_A:
      digitalWrite(pin_self_contact_A, inContact);
      break;

    case FENCER_B:
      digitalWrite(pin_self_contact_B, inContact);
      break;

    default:
      break;
  }
}

