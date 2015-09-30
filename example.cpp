#include "application.h"

SYSTEM_MODE(MANUAL);

#pragma SPARK_NO_PREPROCESSOR

#include "RotaryEncoderWithPush.h"

RotaryEncoderWithPush myKnob(D2, D3, D4, 1);

void setup()
{
  Serial.begin(9600);
  delay(2000);

  Serial.println("OO RotaryEncoderWithPush");

  myKnob.setup();
}

static int timeOfLastKnobDump = 0;

void loop()
{
  myKnob.checkOnRotaryEncoder();
  myKnob.checkOnPushButton();
}