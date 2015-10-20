
/********************************* Includes ***********************************/
#include "detection_interface.h"
#include "display_interface.h"

/******************************** Constants ***********************************/
#define EPEE_DEPRESS_US 2000
#define EPEE_LOCKOUT_US 45000

#define FOIL_DEPRESS_US
#define FOIL_LOCKOUT_US

#define SABER_DEPRESS_US
#define SABER_LOCKOUT_US

/****************************** Display Status ********************************/
boolean touch_A, touch_B;
boolean off_target_A, off_target_B;
boolean self_contact_A, self_contact_B;

/********************************** Timing ************************************/
unsigned long depress_time_micros_A = 0, depress_time_micros_B = 0;

/***************************** Internal Status ********************************/
boolean depressed_A = false, depressed_B = false;
boolean locked_out = false;
boolean self_contact_A_changed, self_contact_B_changed;

/******************************************************************************/

/******************************************************************************/


void setup() {
  // put your setup code here, to run once:
  setup_detection(EPEE);
  setup_display();
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (1)
  {
    epee();
    display_signals();
  }
}

void display_signals()
{
  if (locked_out)
  {
    if (touch_A)
    {

    }
    if (touch_B)
    {

    }
    if (off_target_A)
    {

    }
    if (off_target_B)
    {

    }
    reset();
  }
  //
  else
  {
    if (self_contact_A_changed)
    {
      
    }
    if (self_contact_B_changed)
    {
  
    }
  }
}

void reset()
{
  // reset display booleans
  touch_A = false;
  touch_B = false;
  off_target_A = false;
  off_target_B = false;
  self_contact_A = false;
  self_contact_B = false;
  // reset all time values
  depress_time_micros_A = 0;
  depress_time_micros_B = 0;
  // reset all status booleans
  depressed_A = false;
  depressed_B = false;
  locked_out = false;
}

void epee()
{
  unsigned long now = get_current_time_micros();

  // A or B has had a touch register and enough time has passed to now lock out the opponent,
  // or both A and B have registered a touch, so the remainder of the lockout can be ignored.
  if ((touch_A && (now - depress_time_micros_A >= EPEE_LOCKOUT_US))
      || (touch_B && (now - depress_time_micros_B >= EPEE_LOCKOUT_US))
      || (touch_A && touch_B))
  {
    locked_out = true;
  }

  // first check for the status of fencer A
  if (touch_A == false)
  {
    // if fencer A is in contact with valid target area
    if (in_contact_with_target(FENCER_A, EPEE))
    {
      // if fencer A's tip was depressed during the last iteration
      if (depressed_A)
      {
        // A has had their tip depressed for long enough, register A's touch as valid.
        if (get_current_time_micros() - depress_time_micros_A >= EPEE_DEPRESS_US)
        {
          touch_A = true;
        }
      }
      // A just landed a touch (i.e. just depressed tip), so record the time so 
      // waiting for the depress time minimum can start
      else
      {
        depress_time_micros_A = get_current_time_micros();
        depressed_A = true;
      }
    }
    // A has lost contact with B before the touch was depressed long enough to register as a point
    else if (depressed_A)
    {
      depress_time_micros_A = 0;
      depressed_A = false;
    }
  }

  // second check for fencer B
  if (touch_B == false)
  {
    if (in_contact_with_target(FENCER_B, EPEE))
    {
      if (depressed_B)
      {
        // B has had their tip depressed for long enough, register B's touch as valid.
        if (get_current_time_micros() - depress_time_micros_B >= EPEE_DEPRESS_US)
        {
          touch_B = true;
        }
      }
      // B just landed a touch (i.e. just depressed tip), so record the time so 
      // waiting for the depress time minimum can start
      else
      {
        depress_time_micros_B = get_current_time_micros();
        depressed_B = true;
      }
    }
    // B has lost contact with A before the touch was depressed long enough to register as a point
    else if (depressed_B)
    {
      depress_time_micros_B = 0;
      depressed_B = false;
    }
  }
}
