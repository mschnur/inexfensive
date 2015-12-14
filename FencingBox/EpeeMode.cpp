#include "WeaponMode.h"
#include "FencingConstants.h"
#include "DetectionInterface.h"

void EpeeMode::updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut)
{
  unsigned long now = get_current_time_micros();

  // A or B has had a touch register and enough time has passed to now lock out the opponent,
  // or both A and B have registered a touch, so the remainder of the lockout can be ignored.
  if ((fencerA.touch && (now - fencerA.depress_time_micros >= EPEE_LOCKOUT_US))
      || (fencerB.touch && (now - fencerB.depress_time_micros >= EPEE_LOCKOUT_US))
      || (fencerA.touch && fencerB.touch))
  {
    out_lockedOut = true;
    return; // return early, because there is no self contact to detect in epee
  }

  updateFencerStatus(fencerA);
  updateFencerStatus(fencerB);
}

void EpeeMode::updateFencerStatus(Fencer& fencer)
{
  FencerStatus fStatus;

  if (fencer.touch == false)
  {
    // get fencer's status
    fStatus = get_fencer_status(fencer.side, EPEE);

    // if fencer is in contact with valid target area
    if (in_contact_on_target(fStatus))
    {
      // if fencer's tip was depressed during the last iteration
      if (fencer.depressed_on_target)
      {
        // fencer has had their tip depressed for long enough, register fencer's touch as valid.
        if (get_current_time_micros() - fencer.depress_time_micros >= EPEE_DEPRESS_US)
        {
          fencer.touch = true;
        }
      }
      // fencer just landed a touch (i.e. just depressed tip), so record the time so
      // waiting for the depress time minimum can start
      else
      {
        fencer.depress_time_micros = get_current_time_micros();
        fencer.depressed_on_target = true;
      }
    }
    // fencer has lost contact with opponent before the touch was depressed long enough to register as a point
    else if (fencer.depressed_on_target)
    {
      fencer.depress_time_micros = 0;
      fencer.depressed_on_target = false;
    }
  }
}




