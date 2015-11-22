

/********************************* Includes ***********************************/
#include "detection_interface.h"

/********************************** Macros ************************************/
#define CONVERT_TO_ARDUINO(voltage) ((int) (voltage / ARDUINO_ANALOG_READ_VOLTAGE_GRANULARITY))

/***************************** Common Constants *******************************/
#define ARDUINO_ANALOG_READ_GRANULARITY 0.0049f

#define A1_index 0
#define B1_index 1
#define C1_index 2
#define A2_index 3
#define B2_index 4
#define C2_index 5

/****************************** Epee Constants ********************************/
#define EPEE_TARGET_CONTACT_VOLTAGE 2.5f // 2.5 volts
#define EPEE_VOLTAGE_TOLERANCE 0.4f // 0.4 volts of tolerance

#define EPEE_ARDUINO_TOLERANCE CONVERT_TO_ARDUINO(EPEE_VOLTAGE_TOLERANCE)
#define EPEE_TARGET_CONTACT_MID CONVERT_TO_ARDUINO(EPEE_TARGET_CONTACT_VOLTAGE)
#define EPEE_TARGET_CONTACT_MIN (EPEE_TARGET_CONTACT_MID - EPEE_ARDUINO_TOLERANCE)
#define EPEE_TARGET_CONTACT_MAX (EPEE_TARGET_CONTACT_MID + EPEE_ARDUINO_TOLERANCE)

/****************************** Foil Constants ********************************/
#define FOIL_ON_TARGET_VOLTAGE 2.5f // 2.5V
#define FOIL_OFF_TARGET_VOLTAGE 5.0f // 5V
#define FOIL_SELF_CONTACT_LOWER_BOUND_VOLTAGE 1.0f // 1V
#define FOIL_SELF_CONTACT_UPPER_BOUND_VOLTAGE 2.0f // 2V
#define FOIL_VOLTAGE_TOLERANCE 0.15f // 0.15V of tolerance



#define FOIL_LINES_EQUAL_TOLERANCE 10 // 10 * 0.0049 V ~= 0.05V of tolerance





/***************************** Sabre Constants ********************************/


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

inline unsigned long get_current_time_micros()
{
  return micros();
}

void setup_detection(WeaponType weapon)
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
      digitalWrite(pin_AA_pull, LOW);
      digitalWrite(pin_AB_pull, HIGH);
      digitalWrite(pin_AC_pull, LOW);
      digitalWrite(pin_BA_pull, LOW);
      digitalWrite(pin_BB_pull, HIGH);
      digitalWrite(pin_BC_pull, LOW);
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

inline void read_lines(int * line_val_array, FencerSide fencer)
{
  switch (fencer)
  {
    case FENCER_A:
      line_val_array[A1_index] = analogRead(pin_AA_read);
      line_val_array[B1_index] = analogRead(pin_AB_read);
      line_val_array[C1_index] = analogRead(pin_AC_read);
      line_val_array[A2_index] = analogRead(pin_BA_read);
      line_val_array[B2_index] = analogRead(pin_BB_read);
      line_val_array[C2_index] = analogRead(pin_BC_read);
      break;

    case FENCER_B:
      // switch the order of the lines in the array so the first three
      // are always "this fencer's" A/B/C lines (in this case,
      // FENCER_B is "this fencer", so BA, BB, and BC are first)
      line_val_array[A1_index] = analogRead(pin_BA_read);
      line_val_array[B1_index] = analogRead(pin_BB_read);
      line_val_array[C1_index] = analogRead(pin_BC_read);
      line_val_array[A2_index] = analogRead(pin_AA_read);
      line_val_array[B2_index] = analogRead(pin_AB_read);
      line_val_array[C2_index] = analogRead(pin_AC_read);
      break;

    default:
      line_val_array[A1_index] = 0;
      line_val_array[B1_index] = 0;
      line_val_array[C1_index] = 0;
      line_val_array[A2_index] = 0;
      line_val_array[B2_index] = 0;
      line_val_array[C2_index] = 0;
      break;
  }
}

inline boolean gte_with_tolerance(int to_test, int value, int tolerance)
{
  return (to_test >= value - tolerance);
}

inline boolean lte_with_tolerance(int to_test, int value, int tolerance)
{
  return (to_test <= value + tolerance);
}

inline boolean equals_with_tolerance(int to_test, int value, int tolerance)
{
  return (lte_with_tolerance(to_test, value, tolerance)
          && gte_with_tolerance(to_test, value, tolerance));
}

inline int convertToArduino(float voltage)
{
  voltage = ((voltage >= 0.0f) ? (voltage) : (0.0f));
  return ((int) (voltage / (float) ARDUINO_ANALOG_READ_GRANULARITY));
}

inline boolean arduino_equals_voltage_with_tolerance(int to_test, float value, float tolerance)
{
  return (equals_with_tolerance(to_test,
                                convertToArduino(value),
                                convertToArduino(tolerance)));
}

inline boolean arduino_gte_voltage_with_tolerance(int to_test, float value, float tolerance)
{
  return (gte_with_tolerance(to_test,
                             convertToArduino(value),
                             convertToArduino(tolerance)));
}

inline boolean arduino_lte_voltage_with_tolerance(int to_test, float value, float tolerance)
{
  return (lte_with_tolerance(to_test,
                             convertToArduino(value),
                             convertToArduino(tolerance)));
}

FencerStatus get_fencer_status(FencerSide fencer, WeaponType weapon)
{
  int epeeLineValue = 0;
  FencerStatus fStatus = NO_CONTACT;
  int line_vals[6];

  switch (weapon)
  {
    case EPEE:
      switch (fencer)
      {
        case FENCER_A:
          epeeLineValue = analogRead(pin_AB_read);
          break;

        case FENCER_B:
          epeeLineValue = analogRead(pin_BB_read);
          break;

        default:
          epeeLineValue = 0;
          break;
      }

      if (arduino_equals_voltage_with_tolerance(epeeLineValue, 2.5f, 0.4f))
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }
      break;

    case FOIL:
      read_lines(line_vals, fencer);

      /*
       * This fencer is in contact on target is this fencer's B line and the opponent's
       * A line are both at 2.5V
       */
      // Check that this fencer's B line and the opponent's A line are equal
      // (10 tolerance here equates to ~0.05V), then check that one of them is
      // equal to ~2.5V
      if (equals_with_tolerance(line_vals[B1_index], line_vals[A2_index], 10)
          && arduino_equals_voltage_with_tolerance(line_vals[B1_index], 2.5f, 0.15f))
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }
      /*
       * This fencer is in contact off target if this fencer's B line is 5V.
       */
      // else if used because we cannot possible have both on and off target contact
      else if (arduino_equals_voltage_with_tolerance(line_vals[B1_index], 5.0f, 0.15f))
      {
        fStatus |= IN_CONTACT_OFF_TARGET_FLAG;
      }

      /*
       * This fencer is in self contact if:
       * 1) this fencer's A and C line are at the same voltage
       * AND
       * 2) any of the following is true: (this is checked by looking for 1V <= A line <= 2V)
       *    a) A and C line are at 1V (both fencers in self contact + blade contact + one off target touch)
       *    b) A and C line are at 1.25V (self contact + blade contact + touch)
       *    c) A and C line are at 1.667V (plain self contact / both fencers in self contact + blade contact [but no touch])
       *    d) A and C line are at 2V (self contact + blade contact [but no touch])
       */
      // Check that this fencer's A line and C line are equal
      // (10 tolerance here equates to ~0.05V), then check for one of them being
      // one of the values that indicates self contact
      if (equals_with_tolerance(line_vals[A1_index], line_vals[C1_index], 10)
          && arduino_gte_voltage_with_tolerance(line_vals[A1_index], 1.0f, 0.15f)
          && arduino_lte_voltage_with_tolerance(line_vals[A1_index], 2.0f, 0.15f))
      {
        fStatus |= SELF_CONTACT_FLAG;
      }

      break;

    case SABER:
      break;

    default:
      break;
  }

  return fStatus;
}

