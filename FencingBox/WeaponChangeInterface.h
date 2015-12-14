#ifndef __INCLUDE_WEAPON_CHANGE_INTERFACE_H__
#define __INCLUDE_WEAPON_CHANGE_INTERFACE_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void setupWeaponChange();

boolean switchWeaponMode();

void weaponModeChanged();

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDE_WEAPON_CHANGE_INTERFACE_H__
