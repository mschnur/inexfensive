

/********************************* Includes ***********************************/
#include <TimerOne.h>
#include "TimerDisplay.h"
/******************************************************************************/

const int INTERRUPT_PERIOD_MS = 200;
const int INTERRUPTS_PER_SEC = (1000 / INTERRUPT_PERIOD_MS);
volatile bool interruptHandled = true;

void TimerDisplay::init()
{
  Timer1.initialize(INTERRUPT_PERIOD_MS * 1000L);
  Timer1.stop();
  Timer1.attachInterrupt(&TimerDisplay::timerInterrupt);
  displayTime(currentTime);
  pinMode(timeRemaining_pin, OUTPUT);
  digitalWrite(timeRemaining_pin, HIGH);
}

void TimerDisplay::start()
{
  // receive an interrupt every 100 ms
  //  Timer1.attachInterrupt(&TimerDisplay::timerInterrupt);
  Timer1.start();

  stopped = false;
}

void TimerDisplay::stop()
{
  if (!stopped)
  {
    Timer1.stop();
    //    Timer1.detachInterrupt();
    stopped = true;
  }
}

void TimerDisplay::startOrStop()
{
  if (stopped)
  {
    start();
  }
  else
  {
    stop();
  }
}

void TimerDisplay::reset()
{
  if (!stopped)
  {
    stop();
  }

  remainingTimeMillis = resetValue.inMillis();
  interruptsSinceLastDispUpdate = 0;
  interruptHandled = true;
  currentTime = resetValue;
  displayTime(currentTime);

  if (remainingTimeMillis == 0)
  {
    digitalWrite(timeRemaining_pin, LOW);
  }
  else
  {
    digitalWrite(timeRemaining_pin, HIGH);
  }
}

void TimerDisplay::increaseResetValueAndReset()
{
  // increase the reset value by one minute
  bool timeIncreased = true;
  if (resetValue.mOnes < 9)
  {
    resetValue.mOnes++;
  }
  else if (resetValue.mTens < 9)
  {
    resetValue.mOnes = 0;
    resetValue.mTens++;
  }
  else // don't increase minutes over 99, so don't actually increment any values in this case
  {
    timeIncreased = false;
  }

  if (timeIncreased)
  {
    resetValue.recalculateMilliTime();
  }

  reset();
}

void TimerDisplay::decreaseResetValueAndReset()
{
  // decrease the reset value by one minute
  bool timeDecreased = true;
  if (resetValue.mOnes > 0)
  {
    resetValue.mOnes--;
  }
  else if (resetValue.mTens > 0)
  {
    resetValue.mTens--;
    resetValue.mOnes = 9;
  }
  else  // can't decrease minutes below 0, so don't actually decrement any values in this case
  {
    timeDecreased = false;
  }

  if (timeDecreased)
  {
    resetValue.recalculateMilliTime();
  }

  reset();
}

void TimerDisplay::timerInterrupt()
{
  TimerDisplay::getInstance().decrementTime();
}

void TimerDisplay::displayTime(const Time& t)
{
  sOneDisp.setValue(t.sOnes);
  sTenDisp.setValue(t.sTens);
  mOneDisp.setValue(t.mOnes);
  // Make the 10s column of the minutes be blank if it is zero
  mTenDisp.setValue((t.mTens == 0) ? (10) : (t.mTens));
}

void TimerDisplay::updateIfNeeded()
{
  if (!interruptHandled)
  {
    interruptHandled = true;
    unsigned long localRemainingTime;

    // get the remainingTimeMillis (must be accessed with interrupts disabled since it is a multibyte value
    noInterrupts();
    localRemainingTime = remainingTimeMillis;
    interrupts();

    if (localRemainingTime == 0L && !stopped)
    {
      digitalWrite(timeRemaining_pin, LOW);
      stop();
      // in case remainingTimeMillis is somehow less than 0, set it to 0
      // it is ok to set this value here without disabling interrupts because the interrupt that
      // uses this value will have been turned off by the previous call to stop()
      remainingTimeMillis = 0L;
    }

    if (interruptsSinceLastDispUpdate >= INTERRUPTS_PER_SEC)
    {
      interruptsSinceLastDispUpdate -= INTERRUPTS_PER_SEC;
      currentTime.decreaseBySecond();
      displayTime(currentTime);
    }
  }
}

void TimerDisplay::decrementTime()
{
  if (remainingTimeMillis > 0)
  {
    remainingTimeMillis -= INTERRUPT_PERIOD_MS;
  }
  ++interruptsSinceLastDispUpdate;
  interruptHandled = false;
}


