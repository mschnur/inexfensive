#include "UnoReader.h"

// initialize static members
volatile boolean UnoReader::unoStatusAvailable = false;
volatile UnoStatus UnoReader::mostRecentStatus = NO_CONTACT;

void UnoReader::init()
{
  pinMode(intPin, INPUT);
  pinMode(onTargA, INPUT);
  pinMode(offTargA, INPUT);
  pinMode(selfA, INPUT);
  pinMode(onTargA, INPUT);
  pinMode(offTargA, INPUT);
  pinMode(selfB, INPUT);
  attachInterrupt(digitalPinToInterrupt(intPin), &unoUpdate, RISING);
}

UnoStatus UnoReader::getUnoStatus()
{
  unoStatusAvailable = false;
  return mostRecentStatus;
}

void UnoReader::unoUpdate()
{
  // read the new state
  UnoStatus newStatus = NO_CONTACT;
  if (digitalRead(unoOnTargetA_pin) == HIGH)
    newStatus |= A_ON_TARGET_FLAG;
  if (digitalRead(unoOffTargetA_pin) == HIGH)
    newStatus |= A_OFF_TARGET_FLAG;
  if (digitalRead(unoSelfContactA_pin) == HIGH)
    newStatus |= A_SELF_CONTACT_FLAG;
  if (digitalRead(unoOnTargetB_pin) == HIGH)
    newStatus |= B_ON_TARGET_FLAG;
  if (digitalRead(unoOffTargetB_pin) == HIGH)
    newStatus |= B_OFF_TARGET_FLAG;
  if (digitalRead(unoSelfContactB_pin) == HIGH)
    newStatus |= B_SELF_CONTACT_FLAG;

  // store the new state and a boolean flag in volatile variables
  mostRecentStatus = newStatus;
  unoStatusAvailable = true;
}




