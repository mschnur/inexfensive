
/********************************* Includes ***********************************/
#include "WeaponChangeInterface.h"

/*********************************** Pins *************************************/
const uint8_t pin_changeWeapon = 2;

/***************************** Internal Status ********************************/
volatile uint8_t switchCount;

/******************************************************************************/

void weaponChangeISR()
{
  ++switchCount;
}

void setupWeaponChange()
{
  // initialize flag
  switchCount = 0;

  // setup pin on which interrupt signal will be received
  pinMode(pin_changeWeapon, INPUT);

  // set up interrupt for changing weapon mode
  attachInterrupt(digitalPinToInterrupt(pin_changeWeapon), weaponChangeISR, RISING);
}

uint8_t getSwitchWeaponModeCount()
{
  return switchCount;
}

void weaponModeChanged()
{
  --switchCount;
}




