#ifndef __INCLUDE_DISPLAY_INTERFACE_H__
#define __INCLUDE_DISPLAY_INTERFACE_H__

#include "fencing_types.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void setup_display();

void signal_touch(boolean on_target_A, boolean off_target_A,
                  boolean on_target_B, boolean off_target_B);

void signal_self_contact(Fencer fencer, boolean inContact);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDE_DISPLAY_INTERFACE_H__
