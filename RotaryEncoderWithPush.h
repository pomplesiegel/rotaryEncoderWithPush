#include "application.h"

#ifndef _Rotary_Encoder_With_Push_H_
#define _Rotary_Encoder_With_Push_H_

class RotaryEncoderWithPush
{

public:

RotaryEncoderWithPush(int _rotaryInputPinChannelA, int _rotaryInputPinChannelB, int _pushButtonInputPin);

//Method to set up the rotary encoder, including input and interrupt settings
void setup();

//Relative position of rotary knob since it was last checked.
//This counts as checking and will clear the current value back to 0
int retrieveRotaryKnobOffset();

//Indicates if the knob offset is != 0
//Does not clear the current offset
bool knobTurnHasOccurred();

//Indicates whether button is currently depressed
bool buttonIsCurrentlyDepressed();

//Returns number of seconds button was depressed for
float secondsButtonHeldFor();

void checkOnRotaryEncoder(); 

void checkOnPushButton();

protected:

void rotaryInterruptHandlerChannelA();
void rotaryInterruptHandlerChannelB();
void pushButtonInterruptHandler();

//Push button state
bool buttonCurrentlyDepressed;
unsigned long millisAtWhichButtonWasLastPressed;
volatile bool pushButtonInterruptOccurred; 

//Rotary encoder state
int rotaryKnobOffsetSinceLastCheck;

volatile bool interruptOccurredOnRotaryChannelA;
volatile bool interruptOccurredOnRotaryChannelB;
volatile unsigned long millisOfLastRotaryInterrupt;







//Input pins used for sensing state of rotary encoder + push button
const int rotaryInputPinChannelA;
const int rotaryInputPinChannelB;
const int pushButtonInputPin;
};
#endif