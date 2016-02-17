

/********************************* Includes ***********************************/
#include "DetectionInterface.h"

/********************************* Constants **********************************/
#define A1_index 0
#define B1_index 1
#define C1_index 2
#define A2_index 3
#define B2_index 4
#define C2_index 5

/*********************************** Pins *************************************/
const uint8_t pin_AA_pull = 9;    // Pullup/pulldown for Fencer A's A line (digital)
const uint8_t pin_AB_pull = 8;   // Pullup/pulldown for Fencer A's B line (digital)
const uint8_t pin_AC_pull = 7;   // Pullup/pulldown for Fencer A's C line (digital)

const uint8_t pin_BA_pull = 6;    // Pullup/pulldown for Fencer B's A line (digital)
const uint8_t pin_BB_pull = 5;    // Pullup/pulldown for Fencer B's B line (digital)

const uint8_t pin_time_left = 3;  // high when there is any time on the mega timer

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

void setup_detection(WeaponType weapon)
{
  // set pullup/pulldown digital I/O pins to be output pins
  pinMode(pin_AA_pull, OUTPUT);
  pinMode(pin_AB_pull, OUTPUT);
  pinMode(pin_AC_pull, OUTPUT);
  pinMode(pin_BA_pull, OUTPUT);
  pinMode(pin_BB_pull, OUTPUT);

  // set time left indicator pin to be input
  pinMode(pin_time_left, INPUT);

  setupNewWeapon(weapon);
}

void setupNewWeapon(WeaponType weapon)
{
  switch (weapon)
  {
    case EPEE:
      digitalWrite(pin_AA_pull, HIGH);
      digitalWrite(pin_AB_pull, LOW);
      digitalWrite(pin_AC_pull, LOW);
      digitalWrite(pin_BA_pull, HIGH);
      digitalWrite(pin_BB_pull, LOW);
      break;

    case FOIL:
      digitalWrite(pin_AA_pull, LOW);
      digitalWrite(pin_AB_pull, HIGH);
      digitalWrite(pin_AC_pull, LOW);
      digitalWrite(pin_BA_pull, LOW);
      digitalWrite(pin_BB_pull, HIGH);
      break;

    case SABER:
      digitalWrite(pin_AA_pull, LOW);
      digitalWrite(pin_AB_pull, HIGH);
      digitalWrite(pin_AC_pull, HIGH);
      digitalWrite(pin_BA_pull, LOW);
      digitalWrite(pin_BB_pull, HIGH);
      break;

    default:
      break;
  }
}

boolean timeIsLeft()
{
  return (digitalRead(pin_time_left) == HIGH);
}

void readLines(int * line_val_array)
{
  line_val_array[A1_index] = analogRead(pin_AA_read);
  line_val_array[B1_index] = analogRead(pin_AB_read);
  line_val_array[C1_index] = analogRead(pin_AC_read);
  line_val_array[A2_index] = analogRead(pin_BA_read);
  line_val_array[B2_index] = analogRead(pin_BB_read);
  line_val_array[C2_index] = analogRead(pin_BC_read);
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

      // This fencer is in contact if this fencer's B line is at 2.5V (+/- 0.4V)
      if (epeeLineValue > 430 && epeeLineValue < 590)
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }
      break;

    case FOIL:
    case SABER:
    default:
      readLines(line_vals);
      return get_fencer_status(fencer, weapon, line_vals);
  }

  return fStatus;
}

FencerStatus get_fencer_status(FencerSide fencer, WeaponType weapon, int * lineValArray)
{
  int epeeLineValue = 0;
  FencerStatus fStatus = NO_CONTACT;
  uint8_t fencerA, fencerB, fencerC, opponentA, opponentB;
  switch (fencer)
  {
    case FENCER_A:
      fencerA = A1_index;
      fencerB = B1_index;
      fencerC = C1_index;
      opponentA = A2_index;
      opponentB = B2_index;
      break;

    case FENCER_B:
    default:
      fencerA = A2_index;
      fencerB = B2_index;
      fencerC = C2_index;
      opponentA = A1_index;
      opponentB = B1_index;
      break;
  }

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

      // This fencer is in contact if this fencer's B line is at 2.5V (+/- 0.4V)
      if (epeeLineValue >= 430 && epeeLineValue <= 590)
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }
      break;

    case FOIL:
      /*
       * This fencer is in contact on target if this fencer's B line and the opponent's
       * A line are both at 2.5V
       */
      // Check that this fencer's B line and the opponent's A line are both
      // equal to ~2.5V (+/- 0.15V)
      if (480 <= lineValArray[fencerB] && lineValArray[fencerB] <= 540 &&
          480 <= lineValArray[opponentA] && lineValArray[opponentA] <= 540)
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }

      /*
       * This fencer is in contact off target if this fencer's B line is ~5V (+/- 0.25V).
       */
      // else if used because we cannot possible have both on and off target contact
      else if (970 <= lineValArray[fencerB] && lineValArray[fencerB] <= 1023)
      {
        fStatus |= IN_CONTACT_OFF_TARGET_FLAG;
      }

      /*
       * This fencer is in self contact if:
       * 1) this fencer's A and C line are at the same voltage
       * AND
       * 2) any of the following is true: (this is checked by looking for ~1V <= A line <= ~2V [actually 0.85V <= A <= 2.15V])
       *    a) A and C line are at 1V (both fencers in self contact + blade contact + one off target touch)
       *    b) A and C line are at 1.25V (self contact + blade contact + touch)
       *    c) A and C line are at 1.667V (plain self contact / both fencers in self contact + blade contact [but no touch])
       *    d) A and C line are at 2V (self contact + blade contact [but no touch])
       */
      // Check that this fencer's A line and C line are equal
      // (10 tolerance here equates to ~0.05V), then check for one of them being
      // one of the values that indicates self contact
      if (173 <= lineValArray[fencerA] && lineValArray[fencerA] <= 439 &&
          173 <= lineValArray[fencerC] && lineValArray[fencerC] <= 439)
      {
        fStatus |= SELF_CONTACT_FLAG;
      }

      break;

    case SABER:
      /*
       * For saber, the lines of each FENCER_A and FENCER_B are not the same.
       *    FENCER_A has line A at 0V, line B at 5V, and line C at 5V
       *    FENCER_B has line A at 0V, line B at 5V, and line C at 0V
       * There are two ambiguous cases for FENCER_A, which makes
       *   it impossible to distinguish the difference between
       *   FENCER_A touching the opponent while contacting self
       *   and FENCER_A touching the opponent while contacting
       *   self while both blades are in contact.
       * This ambiguity is dealt with by assuming in this case that
       *   no blade contact is occurring.
       */

      /*
       * This fencer's blade is in contact with the opponent's blade if this fencer's
       * B line and the opponent's B line are both equal,
       * UNLESS this fencer is FENCER_A and this fencer's B line and the opponent's
       * A line are both equal.
       */
      // Check that this fencer's B line and the opponent's B line are equal
      // (+/- 10 tolerance here equates to +/- ~0.05V),
      // then check to that this is not the ambiguous case
      if (((lineValArray[fencerB] <= lineValArray[opponentB] + 10)
           && (lineValArray[fencerB] >= lineValArray[opponentB] - 10))
          && (!(fencer == FENCER_A)
              || !((lineValArray[fencerB] <= lineValArray[opponentA] + 10)
                   && (lineValArray[fencerB] >= lineValArray[opponentA] - 10))))
      {
        fStatus |= BLADE_CONTACT_FLAG;
      }

      /*
       * This fencer is in contact on target if this fencer's B line and the opponent's
       * A line are both equal
       */
      // Check that this fencer's B line and the opponent's A line are equal
      // (+/- 10 tolerance here equates to +/- ~0.05V)
      if (((lineValArray[fencerB] <= lineValArray[opponentA] + 10)
           && (lineValArray[fencerB] >= lineValArray[opponentA] - 10)))
      {
        fStatus |= IN_CONTACT_ON_TARGET_FLAG;
      }

      /*
       * This fencer is in self contact if this fencer's A and B line are at the same
       * voltage
       */
      // Check that this fencer's A line and B line are equal
      // (+/- 10 tolerance here equates to +/- ~0.05V)
      if (((lineValArray[fencerA] <= lineValArray[fencerB] + 10)
           && (lineValArray[fencerA] >= lineValArray[fencerB] - 10)))
      {
        fStatus |= SELF_CONTACT_FLAG;
      }

      break;

    default:
      break;
  }

  return fStatus;
}




