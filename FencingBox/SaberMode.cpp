#include "WeaponMode.h"
#include "FencingConstants.h"
#include "DetectionInterface.h"

void SaberMode::updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut)
{
  unsigned long now = get_current_time_micros();

  // Either A or B has had a touch register and enough time has passed to now
  // lock out the opponent, or both A and B have registered a touch, so
  // the remainder of the lockout can be ignored.
  if ((fencerA.touch && (now - fencerA.depress_time_micros >= SABER_LOCKOUT_US))
      || (fencerB.touch && (now - fencerB.depress_time_micros >= SABER_LOCKOUT_US))
      || (fencerA.touch && fencerB.touch))
  {
    out_lockedOut = true;
  }

  updateFencerStatus(fencerA);
  updateFencerStatus(fencerB);
}

unsigned long whipover_time_micros;

boolean touchesAllowed;
boolean inWhipoverProtection;
boolean bladesInContact;
int whipoverInterruptCount;

void SaberMode::updateFencerStatus(Fencer& fencer)
{
  FencerStatus fStatus;

  // get fencer's status
  // This is gotten here because even if fencer has already registered a touch I need to update
  //  whether or not fencer is touching self, so I need this status every iteration.
  fStatus = get_fencer_status(fencer.side, SABER);

  /* Start of whipover protection handling */
  // if already in whipover protection state
  if (fencer.inWhipoverProtection)
  {
    // if fencer's blade is in contact with opponent's blade
    if (in_blade_contact(fStatus))
    {
      // simply store that blades are in contact
      fencer.bladesInContact = true;
    }
    else // fencer's blade is not in contact with opponent's blade
    {
      // fencer's blade was in contact with opponent's blade on the last iteration
      if (fencer.bladesInContact)
      {
        /* an interrupt in blade contact occurred during whipover state */
        // increment by 1 the number of times blade contact has been interrupted during
        // whipover protection state
        fencer.whipoverInterruptCount++;
      }

      // store that blades are not in contact
      fencer.bladesInContact = false;
    }

    // if whipover protection state has entered period of time during which touches are
    // disallowed
    if (get_current_time_micros() - fencer.whipover_time_micros >= WHIPOVER_LOCKOUT_US)
    {
      // store that touches are not allowed
      fencer.touchesAllowed = false;
    }

    /*
    Whipover protection state should end if 15 ms passes or if more than 10
    interrupts in the blade contact occurs
    */
    // if whipover protection should end
    if ((get_current_time_micros() - fencer.whipover_time_micros >= WHIPOVER_TIMEOUT_US)
        || (fencer.whipoverInterruptCount > MAX_WHIPOVER_INTERRUPT_COUNT))
    {
      // end whipover protection and allow touches
      fencer.inWhipoverProtection = false;
      fencer.touchesAllowed = true;
    }
  }
  else // not in whipover protections state at this time
  {
    // if fencer's blade is in contact with opponent's blade
    if (in_blade_contact(fStatus))
    {
      /* entering whipover protection state */
      // store that whipover protection state has started
      fencer.inWhipoverProtection = true;

      // store whipover protection start time
      fencer.whipover_time_micros = get_current_time_micros();

      // store 0 as number of times blade contact has been interrupted during whipover
      // protection state
      fencer.whipoverInterruptCount = 0;

      // store that blades are in contact
      fencer.bladesInContact = true;
    }

    // store that touches are allowed
    fencer.touchesAllowed = true;
  }

  // first check for the status of fencer
  // don't check fencer if they have already registered a touch
  if (!fencer.touch)
  {
    // if fencer is in contact with valid target area
    if (in_contact_on_target(fStatus))
    {
      // if fencer's weapon was in contact during the last iteration
      if (fencer.depressed_on_target)
      {
        // fencer has had their weapon in contact for long enough, register fencer's touch as valid.
        if (get_current_time_micros() - fencer.depress_time_micros >= SABER_DEPRESS_US)
        {
          // if touches are allowed, record the touch; otherwise, reset the time of contact
          if (fencer.touchesAllowed)
          {
            fencer.touch = true;
          }
          else
          {
            // reset time of contact to now
            fencer.depress_time_micros = get_current_time_micros();
          }
        }
      }
      // fencer just landed a touch (i.e. just made contact), so record the time so
      // waiting for the contact time minimum can start
      else
      {
        fencer.depress_time_micros = get_current_time_micros();
        fencer.depressed_on_target = true;
      }
    }
    // fencer has lost contact before the touch was in contact long enough to register as a point
    else if (fencer.depressed_on_target)
    {
      fencer.depress_time_micros = 0;
      fencer.depressed_on_target = false;
    }
  }

  // check whether fencer's self touch state has changed
  // (i.e. from self touching to not self touching or vice versa)
  if (in_self_contact(fStatus))
  {
    if (!fencer.self_contact)
    {
      fencer.self_contact = true;
      fencer.self_contact_changed = true;
    }
  }
  else // not in self contact
  {
    if (fencer.self_contact)
    {
      fencer.self_contact = false;
      fencer.self_contact_changed = true;
    }
  }
}



