#include "application.h"

#ifndef _Rotary_Encoder_With_Push_H_
#define _Rotary_Encoder_With_Push_H_

class RotaryEncoderWithPush
{

//General usage in real-time: 
// 1. Check to see if something has happened or is currently happening
// including button holds, button presses, knob turns
// 2. 'Retrieve' the value for what has occurred. This will clear that value!
// as it counts as you checking the state of the knob/button

public:

//Constructor
RotaryEncoderWithPush(int _rotaryInputPinChannelA, 
                      int _rotaryInputPinChannelB, 
                      int _pushButtonInputPin, 
                      unsigned long _totalRotaryDebounceTimeMicros,
                      unsigned int _numDebounceChecksInDebounceTime, 
                      unsigned long _rotaryTimeoutPeriodMillis,
                      unsigned long _clickVsHoldThresholdMillis);

//Method to set up the rotary encoder, including input and interrupt settings
void setup();

//Knob methods
//--------------------------------

//Relative position of rotary knob since it was last checked.
//This counts as checking and will clear the current value back to 0
int retrieveRotaryKnobOffset();

//Indicates if the knob has been turned since user last called retrieveRotaryKnobOffset()
//Does not clear the current offset
bool knobTurnHasOccurredSinceLastCheck();

//Button clicking methods
//--------------------------------

//Indicates a button click has occurred
bool buttonWasClickedSinceLastCheck();

//Retrieve the number of button clicks which have happened.
//This will clear the button clicks back to 0!
unsigned int retrieveNumButtonClicksSinceLastCheck();

//Button hold methods
//--------------------------------

//Indicates whether button has been held for a time period longer than a click
bool buttonHoldOccurring();

//Returns number of milliseconds button was or has currently been held for
//For greater context, check buttonHoldOccurring() 
//This does not clear anything, as it should only be checked 
//If button is currently being held, as indicated by buttonHoldOccurring()
unsigned long retrieveMillisButtonHeldFor();

//--------------------------------

protected:
//ISRs for each hardware we're sampling
void rotaryInterruptHandlerChannelA();
void rotaryInterruptHandlerChannelB();
void pushButtonInterruptHandler();

//Live state
//--------------------------------
//Button

//internal state
volatile bool buttonCurrentlyDepressed;
volatile unsigned long millisAtWhichButtonWasLastPressed;

//Length of time for which the button was most recently held for.
//Will not be updated until user lets off of button. Call method instead
volatile unsigned long millisButtonWasHeldFor;

volatile unsigned int numButtonClicksSinceLastCheck; 

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

//Denotes after how many milliseconds a click will be treated as a button hold instead
const unsigned long clickVsHoldThresholdMillis; 

};
#endif