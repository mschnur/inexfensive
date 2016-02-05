#ifndef __INCLUDE_SCORE_DISPLAY_H__
#define __INCLUDE_SCORE_DISPLAY_H__

/********************************* Includes ***********************************/
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SevenSegment.h"
#include "FencingTypes.h"

/*********************************** Pins *************************************/
// Score Display A
static const uint8_t aOne_p0 = 18;
static const uint8_t aOne_p1 = 17;
static const uint8_t aOne_p2 = 16;
static const uint8_t aOne_p3 = 15;

static const uint8_t aTen_p0 = 5;
static const uint8_t aTen_p1 = 4;
static const uint8_t aTen_p2 = 3;
static const uint8_t aTen_p3 = 14;

// Score Display B
static const uint8_t bOne_p0 = 10;
static const uint8_t bOne_p1 = 11;
static const uint8_t bOne_p2 = 12;
static const uint8_t bOne_p3 = 13;

static const uint8_t bTen_p0 = 9;
static const uint8_t bTen_p1 = 8;
static const uint8_t bTen_p2 = 7;
static const uint8_t bTen_p3 = 6;

/******************************************************************************/

class ScoreDisplay
{
  public:

    static ScoreDisplay& getInstance(const FencerSide& side)
    {
      // instantiated on first use, guaranteed to be destroyed on program exit
      static ScoreDisplay displayA(aOne_p0, aOne_p1, aOne_p2, aOne_p3,
                                   aTen_p0, aTen_p1, aTen_p2, aTen_p3);
      static ScoreDisplay displayB(bOne_p0, bOne_p1, bOne_p2, bOne_p3,
                                   bTen_p0, bTen_p1, bTen_p2, bTen_p3);

      switch (side)
      {
        default:
        case FENCER_A:
          return displayA;

        case FENCER_B:
          return displayB;
      }
    }

    void increaseScore()
    {
      if (score < 99)
      {
        ++score;
        updateScore();
      }
    }

    void decreaseScore()
    {
      if (score > 0)
      {
        --score;
        updateScore();
      }
    }

    void zeroScore()
    {
      score = 0;
      updateScore();
    }


  private:
    // private constructor so only the two instances can be created
    ScoreDisplay(const uint8_t ones_p0, const uint8_t ones_p1, const uint8_t ones_p2, const uint8_t ones_p3,
                 const uint8_t tens_p0, const uint8_t tens_p1, const uint8_t tens_p2, const uint8_t tens_p3)
      : ones(ones_p0, ones_p1, ones_p2, ones_p3),
        tens(tens_p0, tens_p1, tens_p2, tens_p3),
        score(0)
    {
      updateScore();
    }

    // make copy constructor private
    ScoreDisplay(ScoreDisplay const&); // Don't Implement
    // don't allow this class to be assigned
    void operator=(ScoreDisplay const&); // Don't implement

    SevenSegmentDisplay ones, tens;
    uint8_t score;

    void updateScore()
    {
      // if the tens column is zero, don't display it (setting a value of 10 results in the 7-segment being blank
      tens.setValue(static_cast<uint8_t>(((score / 10) == 0) ? (10) : (score / 10)));
      ones.setValue(static_cast<uint8_t>(score % 10));
    }
};



#endif // #ifndef __INCLUDE_SCORE_DISPLAY_H__

