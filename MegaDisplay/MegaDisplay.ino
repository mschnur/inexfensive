/* The Mega handles:
 *  - the LED arrays (touch displays)
 *  - the score displays
 *  - the timer
 *  - the weapon mode LEDs
 *  - the IR receiver
 *
 * Inputs to the Mega are:
 *  - Weapon mode change button (pin: X)
 *  - Input lines from the Arduino Uno (pins: [2 sets of 3 pins])
 *  - data from the IR receiver (pin: Y)
 *
 * Outputs of the Mega are:
 *  - BCD score display values (pins: [4 sets of 4 pins])
 *  - BCD timer display values (pins: [4 sets of 4 pins])
 *  - 3 signals to each LED array (pins: [2 sets of 3 pins])
 *  - 3 signals to turn on one of the weapon mode LEDs (pins: [3 pins])
 *  - 1 signal to the Arduino Uno for when time is out (pin: Z)
 */

/********************************* Includes ***********************************/
/*
 * This library needs to be included here because for a library's header to be
 * included in the header search path in other files, it must be included in the
 * main sketch file (this file).
 */
#include <TimerOne.h>

#include <IRremote.h>
#include "TimerDisplay.h"
#include "ScoreDisplay.h"
#include "FencingTypes.h"
#include "WeaponModeDisplay.h"
#include "IRreceiver.h"
#include "FencingTypes.h"

/********************************** Pins **************************************/
const uint8_t weaponSelectPin = 20;
const uint8_t unoInterruptPin = 2;
const uint8_t unoWeaponChangeNotificationPin = 52;
const uint8_t unoWeaponChangeConfirmationPin = A0;

/******************************** Volatiles ***********************************/
volatile boolean needToChangeWeaponMode = false;
volatile boolean unoInterruptOccurred = false;

/********************************** State *************************************/
boolean weaponModeChangesAllowed = true;

/******************************* I/O Objects **********************************/
TimerDisplay * timerDisp;
ScoreDisplay * scoreDispA, * scoreDispB;
WeaponModeDisplay * modeDisp;
IRrecv irrecv(irReceiverPin);

/********************************* Constants **********************************/
// only process one weapon change every 1000 ms
const uint16_t MIN_WEAPON_CHANGE_PERIOD_MS = 1000;

/******************************************************************************/

void setup()
{
  // initialize score displays
  scoreDispA = &ScoreDisplay::getInstance(FENCER_A);
  scoreDispB = &ScoreDisplay::getInstance(FENCER_B);

  // initialize timer
  timerDisp = &TimerDisplay::getInstance();

  // initialize weapon mode display
  modeDisp = &WeaponModeDisplay::getInstance();

  // set up interrupt for changing weapon mode
  pinMode(weaponSelectPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(weaponSelectPin), changeWeaponModeISR, RISING);

  // set up pin for notifying Arduino Uno of weapon change (as well as the confirmation pin,
  // which is used to try and prevent any unexpected interrupts from being registered)
  pinMode(unoWeaponChangeNotificationPin, OUTPUT);
  pinMode(unoWeaponChangeConfirmationPin, OUTPUT);
  digitalWrite(unoWeaponChangeNotificationPin, LOW);
  digitalWrite(unoWeaponChangeConfirmationPin, LOW);

  // set up interrupt from Arduino Uno
  pinMode(unoInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(unoInterruptPin), unoInterruptISR, RISING);

  // initialize IR receiver
  irrecv.enableIRIn();  // Start the receiver

  // DEBUG
  Serial.begin(9600);
}

void changeWeaponModeISR()
{
  needToChangeWeaponMode = true;
}

void unoInterruptISR()
{
  unoInterruptOccurred = true;
}

void loop()
{
  while (1)
  {
    // make any updates to timer display as needed (based on whether the timer interupt has triggered or not)
    timerDisp->updateIfNeeded();

    // check if an interrupt from the Uno occcurred
    if (unoInterruptOccurred)
    {
      handleUnoInterrupt();
    }

    decode_results  results;        // Somewhere to store the results of reading from the IR receiver
    static unsigned long timeOfLastHandledMessage = 0L;
    static unsigned long timeOfLastHandledWeaponChange = 0L;

    unsigned long now = millis();

    if (irrecv.decode(&results))  // Grab an IR code
    {
      if (now - timeOfLastHandledMessage >= MIN_REMOTE_MESSAGE_PERIOD_MS)
      {
        timeOfLastHandledMessage = now;
        switch (results.value)
        {
          case FENCER_A_SCORE_INCREASE:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then increase fencer A's score
            scoreDispA->increaseScore();
            break;

          case FENCER_A_SCORE_DECREASE:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then decrease fencer A's score
            scoreDispA->decreaseScore();
            break;

          case FENCER_B_SCORE_INCREASE:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then increase fencer B's score
            scoreDispB->increaseScore();
            break;

          case FENCER_B_SCORE_DECREASE:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then decrease fencer B's score
            scoreDispB->decreaseScore();
            break;

          case DOUBLE_TOUCH:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then increase both scores by 1
            scoreDispA->increaseScore();
            scoreDispB->increaseScore();
            break;

          case ZERO_SCORES:
            // stop the timer if it is not already stopped
            timerDisp->stop();
            // then set both scores to zero
            scoreDispA->zeroScore();
            scoreDispB->zeroScore();
            break;

          case TIMER_GO_STOP:
            timerDisp->startOrStop();
            break;

          case RESET_TIMER:
            timerDisp->reset();
            break;

          case TIMER_RESET_AND_INCREASE:
            timerDisp->increaseResetValueAndReset();
            break;

          case TIMER_RESET_AND_DECREASE:
            timerDisp->decreaseResetValueAndReset();
            break;

          case CHANGE_WEAPON_MODE:
            needToChangeWeaponMode = true;
            break;

          default:
            // unrecognized value, do nothing
            break;
        }

        // prevents accidentally processing the same command twice
        results.value = 0;
      }

      // Prepare for the next value
      irrecv.resume();
    }

    // change weapon mode if need be
    if (needToChangeWeaponMode)
    {
      // Don't allow weapon mode to be changed within 3 seconds of starting up.
      // This is done because the weapon mode was randomly changing during startup
      // some, but not all, of the time.  This solution seems to fix that problem.
      // Also don't allow weapon mode to be changed more often than once every
      // MIN_WEAPON_CHANGE_PERIOD_MS milliseconds
      if (millis() < 3000 || now - timeOfLastHandledWeaponChange < MIN_WEAPON_CHANGE_PERIOD_MS)
      {
        needToChangeWeaponMode = false;
      }
      else
      {
        timeOfLastHandledWeaponChange = now;
        digitalWrite(unoWeaponChangeConfirmationPin, HIGH);
        digitalWrite(unoWeaponChangeNotificationPin, HIGH);
        delayMicroseconds(75); // time for the interrupt to be registered
        digitalWrite(unoWeaponChangeNotificationPin, LOW);
        delayMicroseconds(75); // time for the confirmation pin to be read on the Uno
        digitalWrite(unoWeaponChangeConfirmationPin, LOW);
        changeWeaponMode();
      }
    }
  }
}

void changeWeaponMode()
{
  modeDisp->switchMode();
  needToChangeWeaponMode = false;
}

void handleUnoInterrupt()
{
  timerDisp->stop();
  unoInterruptOccurred = false;
}
