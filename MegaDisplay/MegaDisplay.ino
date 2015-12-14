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
 * The two Timer library includes are needed because for a library's header to be
 * included in the header search path in other files, it must be included in the
 * main sketch file (this file).
 */
#include <TimerOne.h>
#include <TimerThree.h>

#include <IRremote.h>
#include "TimerDisplay.h"
#include "ScoreDisplay.h"
#include "FencingTypes.h"
#include "WeaponModeDisplay.h"
#include "IRreceiver.h"
#include "AudioVisualController.h"
#include "UnoReader.h"
#include "FencingTypes.h"

/********************************** Pins **************************************/
const uint8_t weaponSelectPin = 2;

/******************************** Volatiles ***********************************/
volatile boolean needToChangeWeaponMode = false;

/******************************* I/O Objects **********************************/
TimerDisplay * timerDisp;
ScoreDisplay * scoreDispA, * scoreDispB;
WeaponModeDisplay * modeDisp;
AudioVisualController * avController;
UnoReader * unoReader;
IRrecv irrecv(irReceiverPin);


/******************************************************************************/

void setup()
{
  // set up interrupt for input from the Arduino Uno
  unoReader = &UnoReader::getInstance();

  // set up controller for displaying a touch
  avController = &AudioVisualController::getInstance();

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

  // initialize IR receiver
  irrecv.enableIRIn();  // Start the receiver

  // DEBUG
  Serial.begin(9600);
}

void changeWeaponModeISR()
{
  needToChangeWeaponMode = true;
}

void loop()
{
  while (1)
  {
    // make any updates to timer display as needed (based on whether the timer interupt has triggered or not)
    timerDisp->updateIfNeeded();

    // check if the uno has an update for us
    if (unoReader->unoUpdateAvailable())
    {
      handleUnoUpdate();
    }

    decode_results  results;        // Somewhere to store the results
    static unsigned long timeOfLastHandledMessage = 0L;
    boolean remoteSignaledWeaponChange = false;
    if (irrecv.decode(&results))  // Grab an IR code
    {
      unsigned long now = millis();
      if (now - timeOfLastHandledMessage >= MIN_REMOTE_MESSAGE_PERIOD_MS)
      {
        timeOfLastHandledMessage = now;
        switch (results.value)
        {
          case FENCER_A_SCORE_INCREASE:
            scoreDispA->increaseScore();
            break;

          case FENCER_A_SCORE_DECREASE:
            scoreDispA->decreaseScore();
            break;

          case FENCER_B_SCORE_INCREASE:
            scoreDispB->increaseScore();
            break;

          case FENCER_B_SCORE_DECREASE:
            scoreDispB->decreaseScore();
            break;

          case DOUBLE_TOUCH:
            scoreDispA->increaseScore();
            scoreDispB->increaseScore();
            break;

          case ZERO_SCORES:
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
            remoteSignaledWeaponChange = true;
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
    if (needToChangeWeaponMode == true)
    {
      if (remoteSignaledWeaponChange)
      {
        remoteSignaledWeaponChange = false;
        detachInterrupt(digitalPinToInterrupt(weaponSelectPin));
        pinMode(weaponSelectPin, OUTPUT);
        digitalWrite(weaponSelectPin, HIGH);
        delayMicroseconds(5);
        digitalWrite(weaponSelectPin, LOW);
        pinMode(weaponSelectPin, INPUT);
        attachInterrupt(digitalPinToInterrupt(weaponSelectPin), changeWeaponModeISR, RISING);
      }
      changeWeaponMode();
    }
  }
}

void changeWeaponMode()
{
  modeDisp->switchMode();
  needToChangeWeaponMode = false;
}

void handleUnoUpdate()
{
  UnoStatus uStatus = unoReader->getUnoStatus();
  WeaponType currentType = modeDisp->getCurrentMode();
  boolean aTouch, bTouch;
  Serial.print("Uno Update: ");
  Serial.print("   onTargetA[");
  Serial.print(onTarget(uStatus, FENCER_A));
  Serial.print("], offTargetA[");
  Serial.print(offTarget(uStatus, FENCER_A));
  Serial.print("], selfContactA[");
  Serial.print(selfContact(uStatus, FENCER_A));
  Serial.print("]\n");
  Serial.print("   onTargetB[");
  Serial.print(onTarget(uStatus, FENCER_B));
  Serial.print("], offTargetB[");
  Serial.print(offTarget(uStatus, FENCER_B));
  Serial.print("], selfContactB[");
  Serial.print(selfContact(uStatus, FENCER_B));
  Serial.print("]\n");
  Serial.print("   mode[");
  Serial.print(((currentType == EPEE) ? ("EPEE") : ((currentType == FOIL) ? ("FOIL") : ("SABER"))));
  Serial.print("]\n");
  switch (currentType)
  {
    case EPEE:
      aTouch = onTarget(uStatus, FENCER_A);
      bTouch = onTarget(uStatus, FENCER_B);
      if (aTouch || bTouch)
      {
        timerDisp->stop();
        avController->signalTouch(aTouch, false, bTouch, false);
      }
      break;

    case FOIL:
      if (touchOccurred(uStatus))
      {
        timerDisp->stop();
        avController->signalTouch(onTarget(uStatus, FENCER_A),
                                  offTarget(uStatus, FENCER_A),
                                  onTarget(uStatus, FENCER_B),
                                  offTarget(uStatus, FENCER_B));
      }
      avController->updateSelfContact(selfContact(uStatus, FENCER_A),
                                      selfContact(uStatus, FENCER_B));
      break;

    case SABER:
      aTouch = onTarget(uStatus, FENCER_A);
      bTouch = onTarget(uStatus, FENCER_B);
      if (aTouch || bTouch)
      {
        timerDisp->stop();
        avController->signalTouch(aTouch, false, bTouch, false);
      }
      avController->updateSelfContact(selfContact(uStatus, FENCER_A),
                                      selfContact(uStatus, FENCER_B));
      break;

    default:
      break;
  }
}


