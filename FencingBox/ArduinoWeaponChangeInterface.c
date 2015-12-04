
/********************************* Includes ***********************************/
#include "WeaponChangeInterface.h"

/*********************************** Pins *************************************/
const uint8_t pin_changeWeapon = 2;

/***************************** Internal Status ********************************/
volatile boolean switchFlag;

/******************************************************************************/

void weaponChangeISR()
{
  switchFlag = true;
}

void setupWeaponChange()
{
  // initialize flag
  switchFlag = false;

  // setup pin on which interrupt signal will be received
  pinMode(pin_changeWeapon, INPUT);

  // set up interrupt for changing weapon mode
  attachInterrupt(digitalPinToInterrupt(pin_changeWeapon), weaponChangeISR, RISING);
}

boolean switchWeaponMode()
{
  return switchFlag;
}

void weaponModeChanged()
{
  switchFlag = false;
}




