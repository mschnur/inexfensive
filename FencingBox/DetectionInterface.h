#ifndef __INCLUDE_DETECTION_INTERFACE_H__
#define __INCLUDE_DETECTION_INTERFACE_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "FencingTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t FencerStatus;

const FencerStatus NO_CONTACT = 0;
const FencerStatus IN_CONTACT_ON_TARGET_FLAG = 1;
const FencerStatus IN_CONTACT_OFF_TARGET_FLAG = 2;
const FencerStatus SELF_CONTACT_FLAG = 4;
const FencerStatus BLADE_CONTACT_FLAG = 8;

inline unsigned long get_current_time_micros();

FencerStatus get_fencer_status(FencerSide fencer, WeaponType weapon);

inline boolean in_contact_on_target(FencerStatus fStatus)
{
  return (fStatus & IN_CONTACT_ON_TARGET_FLAG == IN_CONTACT_ON_TARGET_FLAG);
}

inline boolean in_contact_off_target(FencerStatus fStatus)
{
  return (fStatus & IN_CONTACT_OFF_TARGET_FLAG == IN_CONTACT_OFF_TARGET_FLAG);
}

inline boolean in_self_contact(FencerStatus fStatus)
{
  return (fStatus & SELF_CONTACT_FLAG == SELF_CONTACT_FLAG);
}

inline boolean in_blade_contact(FencerStatus fStatus)
{
  return (fStatus & BLADE_CONTACT_FLAG == BLADE_CONTACT_FLAG);
}

void setup_detection(WeaponType weapon);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDE_DETECTION_INTERFACE_H__



