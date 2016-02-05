#ifndef __INCLUDE_DISPLAY_INTERFACE_H__
#define __INCLUDE_DISPLAY_INTERFACE_H__

#include "FencingTypes.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void setup_display();

void signalTouch(boolean onTargetA, boolean offTargetA,
                 boolean onTargetB, boolean offTargetB);
                  
void updateSelfContact(boolean selfA, boolean selfB);

void updateBreakInControlCircuit(boolean breakA, boolean breakB);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __INCLUDE_DISPLAY_INTERFACE_H__



