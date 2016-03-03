#include "WeaponMode.h"
#include "FencingConstants.h"
#include "DetectionInterface.h"

void FoilMode::updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut)
{
  unsigned long now = get_current_time_micros();
  
  // Either A or B has had a touch (on or off target) register and enough time has passed to now
  // lock out the opponent, or both A and B have registered a touch (on or off target), so
  // the remainder of the lockout can be ignored.
  if (((fencerA.touch || fencerA.off_target) && (now - fencerA.depress_time_micros >= FOIL_LOCKOUT_US))
      || ((fencerB.touch || fencerB.off_target) && (now - fencerB.depress_time_micros >= FOIL_LOCKOUT_US))
      || ((fencerA.touch || fencerA.off_target) && (fencerB.touch || fencerB.off_target)))
  {
    out_lockedOut = true;
  }

  int lineValArray[6];
  readLines(lineValArray);
  updateFencerStatus(fencerA, now, lineValArray);
  updateFencerStatus(fencerB, now, lineValArray);
}

void FoilMode::updateFencerStatus(Fencer& fencer, unsigned long now, int * lineValArray)
{
  FencerStatus fStatus;
  
  // get fencer's status
  // This is gotten here because even if fencer has already registered a touch I need to update
  //  whether or not fencer is touching self, so I need this status every iteration.
  fStatus = get_fencer_status(fencer.side, FOIL, lineValArray);

  // first check for the status of fencer
  // don't check fencer if they have already registered a touch (on or off target)
  if (!fencer.touch && !fencer.off_target)
  {
    // if fencer is in contact with valid target area
    if (in_contact_on_target(fStatus))
    {
      // if fencer's tip was depressed during the last iteration
      if (fencer.depressed_on_target)
      {
        // fencer has had their tip depressed for long enough, register fencer's touch as valid.
        if (now - fencer.depress_time_micros >= FOIL_DEPRESS_US)
        {
          fencer.touch = true;
        }
      }
      // fencer just landed a touch (i.e. just depressed tip), so record the time so
      // waiting for the depress time minimum can start
      else
      {
        fencer.depress_time_micros = now;
        fencer.depressed_on_target = true;

        // if fencer was previously waiting for an off target touch to register, reset
        // that status flag
        fencer.depressed_off_target = false;
      }
    }
    // if fencer is in contact with invalid target area
    else if (in_contact_off_target(fStatus))
    {
      // if fencer's tip was depressed during the last iteration
      if (fencer.depressed_off_target)
      {
        // fencer has had their tip depressed for long enough, register fencer's touch as valid.
        if (now - fencer.depress_time_micros >= FOIL_DEPRESS_US)
        {
          fencer.off_target = true;
        }
      }
      // fencer just landed a touch (i.e. just depressed tip), so record the time so
      // waiting for the depress time minimum can start
      else
      {
        fencer.depress_time_micros = now;
        fencer.depressed_off_target = true;

        // if fencer was previously waiting for an on target touch to register, reset
        // that status flag
        fencer.depressed_on_target = false;
      }
    }
    // fencer has lost contact before the touch was depressed long enough to register as a point
    else if (fencer.depressed_on_target || fencer.depressed_off_target)
    {
      fencer.depress_time_micros = 0;
      fencer.depressed_on_target = false;
      fencer.depressed_off_target = false;
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



