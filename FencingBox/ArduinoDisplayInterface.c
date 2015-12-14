

/********************************* Includes ***********************************/
#include "DisplayInterface.h"


/******************************** Constants ***********************************/
#define LIGHT_TIME_MS 3000
#define BUZZER_TIME_MS 1000

/*********************************** Pins *************************************/
const uint8_t pin_on_target_A = 6;     // Fencer A on target (digital)
const uint8_t pin_off_target_A = 7;   // Fencer A off target (digital)
const uint8_t pin_self_contact_A = 8; // Fencer A self contact (digital)

const uint8_t pin_on_target_B = 9;    // Fencer B on target (digital)
const uint8_t pin_off_target_B = 10;   // Fencer B off target (digital)
const uint8_t pin_self_contact_B = 11; // Fencer B self contact (digital)

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

  // initialize all pins to low
  digitalWrite(pin_on_target_A, LOW);
  digitalWrite(pin_off_target_A, LOW);
  digitalWrite(pin_self_contact_A, LOW);
  digitalWrite(pin_on_target_B, LOW);
  digitalWrite(pin_off_target_B, LOW);
  digitalWrite(pin_self_contact_B, LOW);
  digitalWrite(pin_interrupt_mega, LOW);

}

void signal_touch(boolean on_target_A, boolean off_target_A, boolean self_contact_A,
                  boolean on_target_B, boolean off_target_B, boolean self_contact_B)
{
  // make sure we don't do anything if all of the parameters are false
  if (!(on_target_A || off_target_A || on_target_B || off_target_B)
      // make sure we don't have an invalid combination of parameters
      || (on_target_A && off_target_A)
      || (on_target_B && off_target_B))
  {
    return;
  }

  // first set all output lines
  digitalWrite(pin_on_target_A, on_target_A);
  digitalWrite(pin_off_target_A, off_target_A);
  digitalWrite(pin_self_contact_A, self_contact_A);
  digitalWrite(pin_on_target_B, on_target_B);
  digitalWrite(pin_off_target_B, off_target_B);
  digitalWrite(pin_self_contact_B, self_contact_B);

  // then put the signal line high for a pulse
  digitalWrite(pin_interrupt_mega, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin_interrupt_mega, LOW);

  delay(3000);
  digitalWrite(pin_on_target_A, LOW);
  digitalWrite(pin_off_target_A, LOW);
  digitalWrite(pin_self_contact_A, LOW);
  digitalWrite(pin_on_target_B, LOW);
}

void signal_self_contact(boolean selfA, boolean selfB)
{
  // first set self contact lines appropriately
  digitalWrite(pin_self_contact_A, selfA);
  digitalWrite(pin_self_contact_B, selfB);

  // then put the signal line high for a pulse
  digitalWrite(pin_interrupt_mega, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin_interrupt_mega, LOW);
}




