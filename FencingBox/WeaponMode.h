#ifndef __INCLUDE_WEAPON_MODE_H__
#define __INCLUDE_WEAPON_MODE_H__

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "FencingTypes.h"
#include "Fencer.h"

class WeaponMode
{
  protected:
    WeaponType type;

  public:

    WeaponMode(WeaponType myType) : type(myType) {}

    virtual void updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut) = 0;

    virtual WeaponType nextWeaponType() const = 0;

    WeaponType getType()
    {
      return type;
    }
};

class EpeeMode : public WeaponMode
{
  public:
    EpeeMode() : WeaponMode(EPEE) {}

    void updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut);

    WeaponType nextWeaponType() const
    {
      return FOIL;
    }

  private:
    void updateFencerStatus(Fencer& fencer);
};

class FoilMode : public WeaponMode
{
  public:
    FoilMode() : WeaponMode(FOIL) {}

    void updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut);

    WeaponType nextWeaponType() const
    {
      return SABER;
    }

  private:
    void updateFencerStatus(Fencer& fencer);
};

class SaberMode : public WeaponMode
{
  public:
    SaberMode() : WeaponMode(SABER) {}

    void updateStatus(Fencer& fencerA, Fencer& fencerB, boolean& out_lockedOut);

    WeaponType nextWeaponType() const
    {
      return EPEE;
    }

  private:
    void updateFencerStatus(Fencer& fencer);
};

#endif // #ifndef __INCLUDE_WEAPON_MODE_H__




