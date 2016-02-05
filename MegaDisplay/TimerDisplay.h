#ifndef __INCLUDE_TIMER_DISPLAY_H__
#define __INCLUDE_TIMER_DISPLAY_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SevenSegment.h"

/*********************************** Pins *************************************/
static const uint8_t sOne_p0 = 38;
static const uint8_t sOne_p1 = 36;
static const uint8_t sOne_p2 = 34;
static const uint8_t sOne_p3 = 32;

static const uint8_t sTen_p0 = 46;
static const uint8_t sTen_p1 = 44;
static const uint8_t sTen_p2 = 42;
static const uint8_t sTen_p3 = 40;

static const uint8_t mOne_p0 = 47;
static const uint8_t mOne_p1 = 45;
static const uint8_t mOne_p2 = 43;
static const uint8_t mOne_p3 = 41;

static const uint8_t mTen_p0 = 39;
static const uint8_t mTen_p1 = 37;
static const uint8_t mTen_p2 = 35;
static const uint8_t mTen_p3 = 33;

static const uint8_t timeRemaining_pin = 50;
/******************************************************************************/

// TimerDisplay declared here so it can be used as a freind class in Time
// The full class definition is below the Time class in this file.
class TimerDisplay;

class Time
{
    friend class TimerDisplay;
  public:
    uint8_t sOnes, sTens, mOnes, mTens;
    long milliTime;

    Time(const uint8_t minTens, const uint8_t minOnes,  const uint8_t secTens, const uint8_t secOnes)
      : sOnes(secOnes % 10), sTens(secTens % 10), mOnes(minOnes % 10), mTens(minTens % 10)
    {
      recalculateMilliTime();
    }

    long inMillis() const
    {
      return milliTime;
    }

    void decreaseBySecond()
    {
      bool decreased = true;
      if (sOnes > 0)
      {
        sOnes--;
      }
      else if (sTens > 0)
      {
        sOnes = 9;
        sTens--;
      }
      else if (mOnes > 0)
      {
        sOnes = 9;
        sTens = 5;
        mOnes--;
      }
      else if (mTens > 0)
      {
        sOnes = 9;
        sTens = 5;
        mOnes = 9;
        mTens--;
      }
      else // already at 00:00
      {
        decreased = false;
      }

      if (decreased)
      {
        milliTime -= 1000;
      }
    }

    Time& operator=(Time const& other)
    {
      this->sOnes = other.sOnes;
      this->sTens = other.sTens;
      this->mOnes = other.mOnes;
      this->mTens = other.mTens;
      this->milliTime = other.milliTime;
      return *this;
    }

  private:
    void recalculateMilliTime()
    {
      milliTime = static_cast<long>(sOnes * 1000L)
                  + static_cast<long>(sTens * 10 * 1000L)
                  + static_cast<long>(mOnes * 60 * 1000L)
                  + static_cast<long>(mTens * 10 * 60 * 1000L);
    }
};

class TimerDisplay
{
  public:
    static TimerDisplay& getInstance()
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static TimerDisplay instance(sOne_p0, sOne_p1, sOne_p2, sOne_p3,
                                   sTen_p0, sTen_p1, sTen_p2, sTen_p3,
                                   mOne_p0, mOne_p1, mOne_p2, mOne_p3,
                                   mTen_p0, mTen_p1, mTen_p2, mTen_p3);
      return instance;
    }

    // toggles the state (running or stopped)
    void startOrStop();

    void start();

    void stop();

    void reset();

    void increaseResetValueAndReset();

    void decreaseResetValueAndReset();

    void updateIfNeeded();

  private:
    // prevent instances of this class from being constructed aside from the single instance
    TimerDisplay(const uint8_t sO_p0, const uint8_t sO_p1, const uint8_t sO_p2, const uint8_t sO_p3,
                 const uint8_t sT_p0, const uint8_t sT_p1, const uint8_t sT_p2, const uint8_t sT_p3,
                 const uint8_t mO_p0, const uint8_t mO_p1, const uint8_t mO_p2, const uint8_t mO_p3,
                 const uint8_t mT_p0, const uint8_t mT_p1, const uint8_t mT_p2, const uint8_t mT_p3)
      : sOneDisp(sO_p0, sO_p1, sO_p2, sO_p3),
        sTenDisp(sT_p0, sT_p1, sT_p2, sT_p3),
        mOneDisp(mO_p0, mO_p1, mO_p2, mO_p3),
        mTenDisp(mT_p0, mT_p1, mT_p2, mT_p3),
        resetValue(0, 3, 0, 0),
        currentTime(0, 3, 0, 0),
        remainingTimeMillis(3L * 60L * 1000L),
        interruptsSinceLastDispUpdate(0),
        stopped(true)
    {
      init();
    }

    void init();

    static void timerInterrupt();

    void displayTime(const Time& t);

    void decrementTime();

    // make copy constructor private
    TimerDisplay(TimerDisplay const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(TimerDisplay const&); // Don't implement

    // the seven segment displays that make up this timer display
    SevenSegmentDisplay sOneDisp, sTenDisp, mOneDisp, mTenDisp;

    Time resetValue, currentTime;
    volatile long remainingTimeMillis;
    volatile uint8_t interruptsSinceLastDispUpdate;
    bool stopped;


};

#endif // #ifndef __INCLUDE_TIMER_DISPLAY_H__

