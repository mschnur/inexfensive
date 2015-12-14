
/********************************* Includes ***********************************/
#include "DetectionInterface.h"
#include "DisplayInterface.h"
#include "WeaponChangeInterface.h"
#include "Fencer.h"
#include "WeaponMode.h"

/******************************** Constants ***********************************/

/***************************** Internal Status ********************************/
WeaponMode * currentWeaponMode;

boolean locked_out;

/******************************* Weapon Modes *********************************/
// initialized in setup()
WeaponMode * epeeMode;
WeaponMode * foilMode;
WeaponMode * saberMode;

/********************************* Fencers ************************************/
Fencer fencerA = Fencer(FENCER_A);
Fencer fencerB = Fencer(FENCER_B);

/******************************************************************************/

/******************************************************************************/


void setup()
{
  // init weapon modes
  epeeMode = new EpeeMode();
  foilMode = new FoilMode();
  saberMode = new SaberMode();

  // Box starts in epee mode
  currentWeaponMode = epeeMode;

  // sets all internal variables to their defaults
  reset(); 

  // allows detection and display implementations to do any required initialization
  setup_detection(currentWeaponMode->getType());
  setup_display();
  setupWeaponChange();
}

void loop()
{
  // Using an infinite while loop here is slightly faster than the
  // standard loop mechanism (i.e. the loop() method being called repeatedly)
  while (1)
  {
    // switchWeaponMode will be set to true in an interrupt when
    // the change weapon button is pressed on the box or remote
    if (switchWeaponMode())
    {
      switchWeapons();
    }

    // if there is time remaining in the match
    if (timeIsLeft())
    {
      // detect state changes during this iteration
      currentWeaponMode->updateStatus(fencerA, fencerB, locked_out);
    }

    // display any signals (visual or audio), as needed for the current state
    display_signals();
  }
}

void display_signals()
{
  // if we are locked out than an an on target or off target touch has occurred
  if (locked_out)
  {
    signal_touch(fencerA.touch,
                 fencerA.off_target,
                 fencerA.self_contact,
                 fencerB.touch,
                 fencerB.off_target,
                 fencerB.self_contact);

    fencerA.self_contact_changed = false;
    fencerB.self_contact_changed = false;
    reset();
  }
  else if (fencerA.self_contact_changed || fencerB.self_contact_changed)
  {
    signal_self_contact(fencerA.self_contact, fencerB.self_contact);

    fencerA.self_contact_changed = false;
    fencerB.self_contact_changed = false;
  }
}

void reset()
{
  // reset fencer statuses
  fencerA.reset();
  fencerB.reset();
  // make sure we are no longer locked out
  locked_out = false;
}

void switchWeapons()
{
  // reset the flag
  weaponModeChanged();

  // set the current weapon mode (cycle through modes)
  switch (currentWeaponMode->nextWeaponType())
  {
    case EPEE:
      currentWeaponMode = epeeMode;
      break;

    case FOIL:
      currentWeaponMode = foilMode;
      break;

    case SABER:
      currentWeaponMode = saberMode;
      break;

    default:
      // do nothing
      break;
  }

  // set up detection for the newly selected weapon mode
  setup_detection(currentWeaponMode->getType());
}



