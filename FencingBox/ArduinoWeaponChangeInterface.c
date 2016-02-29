
/********************************* Includes ***********************************/
#include "WeaponChangeInterface.h"
#include <digitalWriteFast.h>

/*********************************** Pins *************************************/
const uint8_t pin_changeWeaponInterrupt = 2;
// this is signed just so I don't get a warning caused by digitalReadFast
const int8_t pin_changeWeaponConfirmation = 4;

/***************************** Internal Status ********************************/
volatile uint8_t switchCount;

/******************************************************************************/

void weaponChangeISR()
{
  if (digitalReadFast(pin_changeWeaponConfirmation))
  {
    ++switchCount;
  }
}

void setupWeaponChange()
{
  // initialize flag
  switchCount = 0;

  // setup pin on which interrupt signal will be received as well as the confirmation pin (used
  // to try and rule out any unexpected interrupts).
  pinMode(pin_changeWeaponInterrupt, INPUT);
  pinMode(pin_changeWeaponConfirmation, INPUT);

  // set up interrupt for changing weapon mode
  attachInterrupt(digitalPinToInterrupt(pin_changeWeaponInterrupt), weaponChangeISR, RISING);
}

uint8_t getSwitchWeaponModeCount()
{
  return switchCount;
}

void weaponModeChanged()
{
  --switchCount;
}




