#include "application.h"

SYSTEM_MODE(MANUAL);

#include "RotaryEncoderWithPush.h"

RotaryEncoderWithPush myKnob(D2, D3, D4, 50, 4, 2, 1000);

void setup()
{
  Serial.begin(9600);
  delay(2000);

  Serial.println("OO RotaryEncoderWithPush");

  myKnob.setup();
}

static int knobPosition = 0;

void loop()
{
  if( myKnob.knobTurnHasOccurredSinceLastCheck() )
  {
    knobPosition += myKnob.retrieveRotaryKnobOffset(); 
    Serial.println("Knob: " + String(knobPosition) );
  }

  if( myKnob.buttonWasClickedSinceLastCheck() )
  {
    Serial.println("Button clicked " + String(myKnob.retrieveNumButtonClicksSinceLastCheck()) + " times");
  }

  if( myKnob.buttonHoldOccurring() )
  {
    Serial.println("Button held for " + String( myKnob.retrieveMillisButtonHeldFor() ) + " ms");
  }
}