

/********************************* Includes ***********************************/
#include "detection_interface.h"

/******************************** Constants ***********************************/
#define EPEE_TARGET_CONTACT_VOLTAGE 2.5f // 2.5 volts
#define VOLTAGE_TOLERANCE 0.4f // 0.4 volts of tolerance

#define ARDUINO_ANALOG_READ_VOLTAGE_GRANULARITY 0.0049f

#define MAKE_POSITIVE(x) ((x >= 0) ? (x) : (0))
#define CONVERT_TO_ARDUINO(voltage) ((int) ((MAKE_POSITIVE(voltage)) / ARDUINO_ANALOG_READ_VOLTAGE_GRANULARITY))

#define ARDUINO_ANALOG_READ_TOLERANCE CONVERT_TO_ARDUINO(VOLTAGE_TOLERANCE)

#define EPEE_TARGET_CONTACT_MID CONVERT_TO_ARDUINO(EPEE_TARGET_CONTACT_VOLTAGE)
#define EPEE_TARGET_CONTACT_MIN (EPEE_TARGET_CONTACT_MID - ARDUINO_ANALOG_READ_TOLERANCE)
#define EPEE_TARGET_CONTACT_MAX (EPEE_TARGET_CONTACT_MID + ARDUINO_ANALOG_READ_TOLERANCE)

/*********************************** Pins *************************************/
const uint8_t pin_AA_pull = 8;    // Pullup/pulldown for Fencer A's A line (digital)
const uint8_t pin_AB_pull = 12;   // Pullup/pulldown for Fencer A's B line (digital)
const uint8_t pin_AC_pull = 13;   // Pullup/pulldown for Fencer A's C line (digital)

const uint8_t pin_BA_pull = 1;    // Pullup/pulldown for Fencer B's A line (digital)
const uint8_t pin_BB_pull = 4;    // Pullup/pulldown for Fencer B's B line (digital)
const uint8_t pin_BC_pull = 7;    // Pullup/pulldown for Fencer B's C line (digital)

const uint8_t pin_AA_read = 0;    // Fencer A's A line (analog)
const uint8_t pin_AB_read = 1;    // Fencer A's B line (analog)
const uint8_t pin_AC_read = 2;    // Fencer A's C line (analog)

const uint8_t pin_BA_read = 3;    // Fencer B's A line (analog)
const uint8_t pin_BB_read = 4;    // Fencer B's B line (analog)
const uint8_t pin_BC_read = 5;    // Fencer B's C line (analog)

/******************************************************************************/

unsigned long get_current_time_micros()
{
  return micros();
}

void setup_detection(Weapon weapon)
{
  // set pullup/pulldown digital I/O pins to be output pins
  pinMode(pin_AA_pull, OUTPUT);
  pinMode(pin_AB_pull, OUTPUT);
  pinMode(pin_AC_pull, OUTPUT);
  pinMode(pin_BA_pull, OUTPUT);
  pinMode(pin_BB_pull, OUTPUT);
  pinMode(pin_BC_pull, OUTPUT);

  switch (weapon)
  {
    case EPEE:
      digitalWrite(pin_AA_pull, HIGH);
      digitalWrite(pin_AB_pull, LOW);
      digitalWrite(pin_AC_pull, LOW);
      digitalWrite(pin_BA_pull, HIGH);
      digitalWrite(pin_BB_pull, LOW);
      digitalWrite(pin_BC_pull, LOW);
      break;

    case FOIL:
      break;

    case SABER:
      digitalWrite(pin_AA_pull, LOW);
      digitalWrite(pin_AB_pull, HIGH);
      digitalWrite(pin_AC_pull, LOW);
      digitalWrite(pin_BA_pull, LOW);
      digitalWrite(pin_BB_pull, HIGH);
      digitalWrite(pin_BC_pull, HIGH);
      break;

    default:
      break;
  }
}

boolean in_contact_with_target(Fencer fencer, Weapon weapon)
{
  int analogReadValue = 0;
  switch (weapon)
  {
    case EPEE:
      switch (fencer)
      {
        case FENCER_A:
          analogReadValue = analogRead(pin_AB_read);
        case FENCER_B:
          analogReadValue = analogRead(pin_BB_read);
        default:
          return false;
      }
      return ((analogReadValue >= EPEE_TARGET_CONTACT_MIN)
              && (analogReadValue <= EPEE_TARGET_CONTACT_MAX));

    case FOIL:
      return false;

    case SABER:
      return false;

    default:
      return false;
  }
}

