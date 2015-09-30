#include "RotaryEncoderWithPush.h"

//Millisecond version of Helper function for something we do very often:
//See if it's been 'millisInterval' milliseconds since we last did a certain thing.
//Includes error protection for millis overflow
bool EnoughMillisHaveElapsed(unsigned long lastMillisPerformed, unsigned long millisInterval)
{
  //two functions: 1. See if enough milliseconds have elapsed
  //since we last did a particular task
  //2. Error checking for millis overflow, which occurs every 49 days
  if ( ( millis() >= ( lastMillisPerformed + millisInterval ) ) or
    ( millis() < lastMillisPerformed ) )
  {
    return true; //it's time to do 'it' again
  }
  return false; //not yet
}

RotaryEncoderWithPush::RotaryEncoderWithPush( int _rotaryInputPinChannelA, 
                                              int _rotaryInputPinChannelB, 
                                              int _pushButtonInputPin) :
                                              rotaryInputPinChannelA(_rotaryInputPinChannelA),
                                              rotaryInputPinChannelB(_rotaryInputPinChannelB),
                                              pushButtonInputPin(_pushButtonInputPin)
{
  //initial values
  interruptOccurredOnRotaryChannelA = false;
  interruptOccurredOnRotaryChannelB = false;
  millisOfLastRotaryInterrupt = 0;
  rotaryKnobOffsetSinceLastCheck = 0;

  pushButtonInterruptOccurred = false;
  buttonCurrentlyDepressed = false;
  millisAtWhichButtonWasLastPressed = 0;
}


void RotaryEncoderWithPush::setup()
{
  //Assign the three pins as inputs
  pinMode(rotaryInputPinChannelA, INPUT);
  pinMode(rotaryInputPinChannelB, INPUT);
  pinMode(pushButtonInputPin, INPUT);

  //Attach interrupts to interrupt handler methods
  attachInterrupt(rotaryInputPinChannelA, &RotaryEncoderWithPush::rotaryInterruptHandlerChannelA, this, FALLING);
  attachInterrupt(rotaryInputPinChannelB, &RotaryEncoderWithPush::rotaryInterruptHandlerChannelB, this, FALLING);
  attachInterrupt(pushButtonInputPin, &RotaryEncoderWithPush::pushButtonInterruptHandler, this, CHANGE);
}

//Relative position of rotary knob since it was last checked.
//This counts as checking and will clear the current value back to 0
int RotaryEncoderWithPush::retrieveRotaryKnobOffset()
{
  //static delcaration to avoid re-allocation
  //This occurs only at program initialization time
  static int currentKnobValueToReturn = 0;

  currentKnobValueToReturn = rotaryKnobOffsetSinceLastCheck; 
  rotaryKnobOffsetSinceLastCheck = 0; //reset knob offset value
  return currentKnobValueToReturn;
}


//Indicates if the knob offset is != 0
//Does not clear the current offset
bool RotaryEncoderWithPush::knobTurnHasOccurred()
{
  return ( rotaryKnobOffsetSinceLastCheck != 0 );
}

void RotaryEncoderWithPush::checkOnRotaryEncoder()
{
  //static delcaration to avoid re-allocation
  //This occurs only at program initialization time
  static bool channelAState = false;
  static bool channelBState = false;

  //If neither interrupt has occurred
  if( !interruptOccurredOnRotaryChannelA && !interruptOccurredOnRotaryChannelB )
    return;

  //At this point we know one of our interrupts has occurred
  
  //If we haven't waited long enough for the value to resolve, return
  if( !EnoughMillisHaveElapsed(millisOfLastRotaryInterrupt, 1) )
    return; 

  // if( millis()- )

  //temporary state for this method
  channelAState = pinReadFast(rotaryInputPinChannelA);
  channelBState = pinReadFast(rotaryInputPinChannelB);

  if( interruptOccurredOnRotaryChannelA )
  {
    //reset flag
    interruptOccurredOnRotaryChannelA = false;

    if(channelBState)
      rotaryKnobOffsetSinceLastCheck++;
    else
      return;
  }

  else if( interruptOccurredOnRotaryChannelB )
  {
    //reset flag
    interruptOccurredOnRotaryChannelB = false;

    if(channelAState)
      rotaryKnobOffsetSinceLastCheck--;
    else
      return;
  }

  Serial.println("Knob: " + String(rotaryKnobOffsetSinceLastCheck) );
}


void RotaryEncoderWithPush::checkOnPushButton()
{
  if( !pushButtonInterruptOccurred )
    return;

  //reset flag
  pushButtonInterruptOccurred = false;

  //If button pressed
  if( !pinReadFast(pushButtonInputPin) )
  {
    Serial.println("Button pressed!");
    buttonCurrentlyDepressed = true;
    millisAtWhichButtonWasLastPressed = millis();
  }
  else
  {
    Serial.println("Button released after " + String(millis()-millisAtWhichButtonWasLastPressed) + " ms" );
    buttonCurrentlyDepressed = false;
  }
}


void RotaryEncoderWithPush::rotaryInterruptHandlerChannelA()
{
  //don't run if the last interrupt still hasn't been handled
  if( interruptOccurredOnRotaryChannelA || interruptOccurredOnRotaryChannelB )
    return;

  interruptOccurredOnRotaryChannelA = true; //mark that something happened
  millisOfLastRotaryInterrupt = millis();
}

void RotaryEncoderWithPush::rotaryInterruptHandlerChannelB()
{
  //don't run if the last interrupt still hasn't been handled
  if( interruptOccurredOnRotaryChannelA || interruptOccurredOnRotaryChannelB )
    return;

  interruptOccurredOnRotaryChannelB = true; //mark that something happened
  millisOfLastRotaryInterrupt = millis();
}

void RotaryEncoderWithPush::pushButtonInterruptHandler()
{
  pushButtonInterruptOccurred = true; //mark that button was pressed
}