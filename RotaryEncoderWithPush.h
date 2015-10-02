#include "application.h"

#ifndef _Rotary_Encoder_With_Push_H_
#define _Rotary_Encoder_With_Push_H_

class RotaryEncoderWithPush
{

public:

//Constructor
RotaryEncoderWithPush(int _rotaryInputPinChannelA, int _rotaryInputPinChannelB, 
                      int _pushButtonInputPin, unsigned long _totalRotaryDebounceTimeMicros,
                      unsigned int _numDebounceChecksInDebounceTime, unsigned long _rotaryTimeoutPeriodMillis);

//Method to set up the rotary encoder, including input and interrupt settings
void setup();

//Relative position of rotary knob since it was last checked.
//This counts as checking and will clear the current value back to 0
int retrieveRotaryKnobOffset();

//Indicates if the knob has been turned since user last called retrieveRotaryKnobOffset()
//Does not clear the current offset
bool knobTurnHasOccurredSinceLastCheck();

//Indicates whether button is currently depressed
bool buttonIsCurrentlyDepressed();

//Returns number of milliseconds button was or has currently been held for
//For greater context, check buttonIsCurrentlyDepressed() as well
unsigned long millisButtonHeldFor();

protected:
void rotaryInterruptHandlerChannelA();
void rotaryInterruptHandlerChannelB();
void pushButtonInterruptHandler();

//Live state
//--------------------------------
//Button

volatile bool buttonCurrentlyDepressed;
volatile unsigned long millisAtWhichButtonWasLastPressed;

//Length of time for which the button was most recently held for
volatile unsigned long millisButtonWasHeldFor;

//Rotary encoder state
volatile bool rotaryKnobChangeHasOccurred;
volatile int rotaryKnobOffsetSinceLastCheck;

//Time of most recent successful rotary interrupt, so we can timeout for a bit
volatile unsigned long millisOfMostRecentSuccessfulRotaryInterrupt; 
//--------------------------------

//Input pins used for sensing state of rotary encoder + push button
const int rotaryInputPinChannelA;
const int rotaryInputPinChannelB;
const int pushButtonInputPin;

//micros we wait before taking another reading of the rotary dial within an interrupt
const unsigned long rotaryDebounceMicrosPerCheck;

//Number of times we'll poll the switch for consistent values wihtin the debounce time
const unsigned int numDebounceChecksInDebounceTime; 

//How long after a successful rotary interrupt reading will the device timeout, 
//ignoring other rotary interrupts
const unsigned long rotaryTimeoutPeriodMillis; 

};
#endif