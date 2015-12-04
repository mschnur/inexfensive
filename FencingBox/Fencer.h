#ifndef __INCLUDE_FENCER_H__
#define __INCLUDE_FENCER_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Fencer;

#include "FencingTypes.h"
#include "WeaponMode.h"

class Fencer
{
    // Declare the WeaponMode subclasses as friends of this class, giving these classes
    // access to the protected and private members of this class.
    // This gives these classes access to the status variables in this class
    // without needing to make them public.
    friend class EpeeMode;
    friend class FoilMode;
    friend class SaberMode;

  public:
    // Display Status
    boolean touch;
    boolean off_target;
    boolean self_contact;
    // Internal Status
    boolean self_contact_changed;

  private:
    // The side this fencer is on (A or B)
    const FencerSide side;

    // Timing
    unsigned long depress_time_micros;
    unsigned long whipover_time_micros;

    // Internal Status
    boolean depressed_on_target;
    boolean depressed_off_target;
    
    boolean touchesAllowed;
    boolean inWhipoverProtection;
    boolean bladesInContact;
    int whipoverInterruptCount;

  public:
    // constructor
    Fencer(FencerSide fencer) : side(fencer) {}

    inline const FencerSide getSide() const
    {
      return side;
    }

    inline void reset()
    {
      touch = false;
      off_target = false;
      self_contact = false;

      depress_time_micros = 0L;
      whipover_time_micros = 0L;

      depressed_on_target = false;
      depressed_off_target = false;
      self_contact_changed = false;

      touchesAllowed = false;
      inWhipoverProtection = false;
      bladesInContact = false;
      whipoverInterruptCount = 0;
    }
};


#endif // #ifndef __INCLUDE_FENCER_H__



