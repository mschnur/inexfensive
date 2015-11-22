#ifndef __INCLUDE_DETECTION_INTERFACE_H__
#define __INCLUDE_DETECTION_INTERFACE_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "fencing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long get_current_time_micros();

boolean in_contact_with_target(Fencer fencer, Weapon weapon);

void setup_detection(Weapon weapon);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDE_DETECTION_INTERFACE_H__
