#include "AudioVisualController.h"
#include <TimerThree.h>

const LEDArrayDisplay * AudioVisualController::ledDispA = &LEDArrayDisplay::getInstance(FENCER_A);
const LEDArrayDisplay * AudioVisualController::ledDispB = &LEDArrayDisplay::getInstance(FENCER_B);

void AudioVisualController::init()
{
  // setup buzzer pin
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  // setup TimerThree
  Timer3.initialize();
  Timer3.stop();
}

void AudioVisualController::signalTouch(boolean onTargetA, boolean offTargetA,
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
  ledDispA->setOnTarget(onTargetA);
  ledDispA->setOffTarget(offTargetA);
  ledDispB->setOnTarget(onTargetB);
  ledDispB->setOffTarget(offTargetB);

  // buzzer must go off on a touch (on or off target), so set that pin high
  digitalWrite(buzzer, HIGH);

  // set up an interrupt to call disableBuzzer() in BUZZER_TIME_US microseconds (1 sec)
  Timer3.attachInterrupt(&AudioVisualController::disableBuzzer,
                         BUZZER_TIME_US);
  Timer3.resume();
}

void AudioVisualController::disableBuzzer()
{
  // turn off the buzzer
  digitalWrite(buzzer_pin, LOW);

  // set up an interrupt to call disableTouchLights() in LIGHT_AFTER_BUZZER_TIME_US
  // microseconds (2 sec)
  Timer3.attachInterrupt(&AudioVisualController::disableTouchLights,
                         LIGHT_AFTER_BUZZER_TIME_US);
  Timer3.resume();
}


void AudioVisualController::disableTouchLights()
{
  // then turn off the lights
  ledDispA->setOnTarget(false);
  ledDispA->setOffTarget(false);
  ledDispB->setOnTarget(false);
  ledDispB->setOffTarget(false);
  Timer3.stop();
  Timer3.detachInterrupt();
}

void AudioVisualController::updateSelfContact(boolean selfA, boolean selfB)
{
  ledDispA->setSelfContact(selfA);
  ledDispB->setSelfContact(selfB);
}

void AudioVisualController::updateBreakInControlCircuit(boolean breakA, boolean breakB)
{
  ledDispA->setOffTarget(breakA);
  ledDispB->setOffTarget(breakB);
}

