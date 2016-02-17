
/********************************* Includes ***********************************/
/*
 * These libraries need to be included here because for a library's header to be
 * included in the header search path in *other* files, it must be included in the
 * main sketch file (this file).
 */
#include <SPI.h>
#include <TimerOne.h>

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

/********************************* Defines ************************************/
#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

/******************************************************************************/

void setup()
{
  /*
   * Based on discussion here: http://forum.arduino.cc/index.php?topic=6549.0,
   * as well as data here: https://github.com/greiman/SdFat-beta/blob/master/AnalogBinLoggerExtras/AdcErrorStudy.txt,
   * and this graph: https://github.com/greiman/SdFat-beta/blob/master/AnalogBinLoggerExtras/ADC_ENOB.PNG,
   * It seems that the ADC prescaler can be changed to decrease the time required for each analogRead, at
   * the cost of some accuracy on the reads.  It appears from the above sources that this loss of accuracy
   * is very minor if the prescaler is not decreased too much.  As such, I am decreasing the prescaler
   * by two levels, from the default 128 to 32.  I referenced Table 24-5 from page 250 of the ATmega328P 
   * (the chip in the Arduino Uno) datasheet, which can be found here:
   * http://www.atmel.com/images/atmel-8271-8-bit-avr-microcontroller-atmega48a-48pa-88a-88pa-168a-168pa-328-328p_datasheet_complete.pdf
   */
#if FASTADC
  // set prescale to 32
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  sbi(ADCSRA, ADPS0) ;
#endif

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

//  lightTest();
}

void loop()
{
  // Using an infinite while loop here is slightly faster than the
  // standard loop mechanism (i.e. the loop() method being called repeatedly)
  while (1)
  {
    // The switchWeaponModeCount will be incremented in an interrupt when
    // the change weapon button is pressed on the box or remote
    if (getSwitchWeaponModeCount() > 0)
    {
      switchWeapons();
    }

    // if there is time remaining in the match and we are not in the process of diplaying a touch
    if (timeIsLeft() && !displayingTouch)
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
  if (fencerA.self_contact_changed || fencerB.self_contact_changed)
  {
    updateSelfContact(fencerA.self_contact, fencerB.self_contact);

    fencerA.self_contact_changed = false;
    fencerB.self_contact_changed = false;
  }
  
  // if we are locked out than an an on target or off target touch has occurred
  if (locked_out)
  {
    signalTouch(fencerA.touch,
                fencerA.off_target,
                fencerB.touch,
                fencerB.off_target);
                
    reset();
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
  while (getSwitchWeaponModeCount() > 0)
  {
    // set the current weapon mode (cycle through modes)
    WeaponType newType = currentWeaponMode->nextWeaponType();
    switch (newType)
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
    setupNewWeapon(newType);

    // reset the flag
    weaponModeChanged();
  }
}



