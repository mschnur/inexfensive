
/********************************* Includes ***********************************/
#include "detection_interface.h"
#include "display_interface.h"

/******************************** Constants ***********************************/
#define EPEE_DEPRESS_US 2000
#define EPEE_LOCKOUT_US 45000

#define FOIL_DEPRESS_US 14000
#define FOIL_LOCKOUT_US 300000

#define SABER_DEPRESS_US
#define SABER_LOCKOUT_US

/****************************** Display Status ********************************/
boolean touch_A, touch_B;
boolean off_target_A, off_target_B;
boolean self_contact_A, self_contact_B;

/********************************** Timing ************************************/
unsigned long depress_time_micros_A, depress_time_micros_B;

/***************************** Internal Status ********************************/
Weapon currentWeaponMode;
boolean switchWeaponMode;
boolean depressed_A, depressed_B;
boolean locked_out;
boolean self_contact_A_changed, self_contact_B_changed;

/******************************************************************************/

/******************************************************************************/


void setup() {
  // put your setup code here, to run once:
  currentWeaponMode = EPEE;
  switchWeaponMode = false;
  setup_detection(EPEE);
  setup_display();
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (1)
  {
    if (switchWeaponMode)
    {
      switchWeapons();
    }
    
    // detect state changes during this iteration
    switch (currentWeaponMode)
    {
      case EPEE:
        epee();
        break;

      case FOIL:
        break;

      case SABER:
        break;
      
      default:
        break;
    }

    // display any signals (visual or audio), if needed
    display_signals();
  }
}

void display_signals()
{
  if (locked_out)
  {
    signal_touch(touch_A, off_target_A, touch_B, off_target_B);
    reset();
  }
  // this else is here because self contact is not signaled when a valid touch
  // or off target touch is being signaled
  else
  {
    if (self_contact_A_changed)
    {
      signal_self_contact(FENCER_A, self_contact_A);
      self_contact_A_changed = false;
    }

    if (self_contact_B_changed)
    {
      signal_self_contact(FENCER_B, self_contact_B);
      self_contact_B_changed = false;
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

void switchWeapons()
{
  // reset the flag
  switchWeaponMode = false;
  
  // set the current weapon mode (cycle through modes)
  switch (currentWeaponMode)
  {
    case EPEE:
      currentWeaponMode = FOIL;
      break;

    case FOIL:
      currentWeaponMode = SABER;
      break;

    case SABER:
    default:
      currentWeaponMode = EPEE;
      break;
  }

  // set up detection for the newly selected weapon mode
  setup_detection(currentWeaponMode);  
}

void epee()
{
  unsigned long now = get_current_time_micros();
  FencerStatus status_A, status_B;

  // A or B has had a touch register and enough time has passed to now lock out the opponent,
  // or both A and B have registered a touch, so the remainder of the lockout can be ignored.
  if ((touch_A && (now - depress_time_micros_A >= EPEE_LOCKOUT_US))
      || (touch_B && (now - depress_time_micros_B >= EPEE_LOCKOUT_US))
      || (touch_A && touch_B))
  {
    locked_out = true;
  }

  // first check for the status of fencer A
  // don't check A if they have already registered a touch
  if (touch_A == false)
  {
    // get fencer A's status
    status_A = get_fencer_status(FENCER_A, EPEE);
    
    // if fencer A is in contact with valid target area
    if (in_contact_on_target(status_A))
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
  // don't check B if they have already registered a touch
  if (touch_B == false)
  {
    // get fencer A's status
    status_B = get_fencer_status(FENCER_B, EPEE);
    
    if (in_contact_on_target(status_B))
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
