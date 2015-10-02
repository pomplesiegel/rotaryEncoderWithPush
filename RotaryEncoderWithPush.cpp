#include "RotaryEncoderWithPush.h"


//Millisecond version of Helper function for something we do very often:
//See if it's been 'millisInterval' milliseconds since we last did a certain thing.
//Includes error protection for millis overflow
inline bool EnoughMillisHaveElapsed(unsigned long lastMillisPerformed, unsigned long millisInterval)
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
                                              int _pushButtonInputPin,
                                              unsigned long _totalRotaryDebounceTimeMicros,
                                              unsigned int _numDebounceChecksInDebounceTime,
                                              unsigned long _rotaryTimeoutPeriodMillis,
                                              unsigned long _clickVsHoldThresholdMillis
                                            ) :
                                              rotaryInputPinChannelA(_rotaryInputPinChannelA),
                                              rotaryInputPinChannelB(_rotaryInputPinChannelB),
                                              pushButtonInputPin(_pushButtonInputPin),
                                              rotaryDebounceMicrosPerCheck( _totalRotaryDebounceTimeMicros / _numDebounceChecksInDebounceTime ),
                                              numDebounceChecksInDebounceTime(_numDebounceChecksInDebounceTime),
                                              rotaryTimeoutPeriodMillis(_rotaryTimeoutPeriodMillis),
                                              clickVsHoldThresholdMillis(_clickVsHoldThresholdMillis)
{
  //initial values
  rotaryKnobChangeHasOccurred = false;
  rotaryKnobOffsetSinceLastCheck = 0;

  buttonCurrentlyDepressed = false;
  numButtonClicksSinceLastCheck = 0;
  millisAtWhichButtonWasLastPressed = 0;
  millisButtonWasHeldFor = 0;
  millisOfMostRecentSuccessfulRotaryInterrupt = 0;
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

unsigned int RotaryEncoderWithPush::retrieveNumButtonClicksSinceLastCheck()
{
  //one-time delcaration using `static`
  static unsigned int valueToReturn = 0;

  valueToReturn = numButtonClicksSinceLastCheck; //temp assignment
  numButtonClicksSinceLastCheck = 0; //reset our counter
  return valueToReturn; //return temporary value
}

bool RotaryEncoderWithPush::buttonWasClickedSinceLastCheck()
{
  return ( numButtonClicksSinceLastCheck != 0 );
}

//Indicates whether button has been held for a time period longer than a click
bool RotaryEncoderWithPush::buttonHoldOccurring()
{
  return ( buttonCurrentlyDepressed && 
          EnoughMillisHaveElapsed(millisAtWhichButtonWasLastPressed, clickVsHoldThresholdMillis) );
}

//Returns number of milliseconds button has currently been held for
//If button not currently held, returns 0
unsigned long RotaryEncoderWithPush::retrieveMillisButtonHeldFor()
{
  //If button's not currently depressed, return how long it was last held for
  if( !buttonCurrentlyDepressed )
    return millisButtonWasHeldFor;

  //Otherwise, do some math based on the current time
  //current time minus point at which button was last pressed
  return ( millis() - millisAtWhichButtonWasLastPressed );
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
  rotaryKnobChangeHasOccurred = false;
  return currentKnobValueToReturn;
}


//Indicates if the knob offset is != 0
//Does not clear the current offset
bool RotaryEncoderWithPush::knobTurnHasOccurredSinceLastCheck()
{
  return ( rotaryKnobChangeHasOccurred );
}

//counter for interrupt debouncing
static unsigned int i = 0;

void RotaryEncoderWithPush::rotaryInterruptHandlerChannelA()
{
  //If a successful interrupt has occurred very recently, ignore this one
  if( !EnoughMillisHaveElapsed(millisOfMostRecentSuccessfulRotaryInterrupt, rotaryTimeoutPeriodMillis) )
    return;

  for(i=0; i < numDebounceChecksInDebounceTime; i++)
  {
    delayMicroseconds(rotaryDebounceMicrosPerCheck); //debounce delay
    //if chan A is high, or chan B is low, this is an error
    if( pinReadFast(rotaryInputPinChannelA) || !pinReadFast(rotaryInputPinChannelB) ) 
      return; //ignore this interrupt and exit
  }

  millisOfMostRecentSuccessfulRotaryInterrupt = millis();

  rotaryKnobOffsetSinceLastCheck++;
  rotaryKnobChangeHasOccurred = true;
}


void RotaryEncoderWithPush::rotaryInterruptHandlerChannelB()
{
  //If a successful interrupt has occurred very recently, ignore this one
  if( !EnoughMillisHaveElapsed(millisOfMostRecentSuccessfulRotaryInterrupt, rotaryTimeoutPeriodMillis) )
    return;

  for(i=0; i < numDebounceChecksInDebounceTime; i++)
  {
    delayMicroseconds(rotaryDebounceMicrosPerCheck); //debounce delay
    //if chan B is high, or chan A is low, this is an error
    if( pinReadFast(rotaryInputPinChannelB) || !pinReadFast(rotaryInputPinChannelA) )
      return; //ignore this interrupt and exit
  }

  millisOfMostRecentSuccessfulRotaryInterrupt = millis();

  rotaryKnobOffsetSinceLastCheck--;
  rotaryKnobChangeHasOccurred = true;
}

static unsigned long calculatedTimeButtonHeld = 0;

void RotaryEncoderWithPush::pushButtonInterruptHandler()
{
  //If button released
  if( pinReadFast(pushButtonInputPin) )
  {
    buttonCurrentlyDepressed = false;

    //calculate how long button was held
    calculatedTimeButtonHeld = millis() - millisAtWhichButtonWasLastPressed; 

    //if it's short enough of a time, count this as a click
    if(calculatedTimeButtonHeld < clickVsHoldThresholdMillis)
      numButtonClicksSinceLastCheck++;
    else
      millisButtonWasHeldFor = calculatedTimeButtonHeld; 
  }
  //Button was pressed
  else
  {
    buttonCurrentlyDepressed = true;
    millisAtWhichButtonWasLastPressed = millis();
  }
}