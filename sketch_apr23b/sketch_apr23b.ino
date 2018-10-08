#include <avr/sleep.h>

const byte LED = 9;
const byte BUTTON = 8;

// Interrupt Service Routine (ISR)
void switchPressed ()
{
  sleep_disable();
  //detachInterrupt(0);
  if (digitalRead (BUTTON) == HIGH)
    digitalWrite (LED, HIGH);
  else
    digitalWrite (LED, LOW);
}  // end of switchPressed

void setup ()
{
  pinMode (LED, OUTPUT);  // so we can update the LED
  digitalWrite (BUTTON, HIGH);  // internal pull-up resistor
  
}  // end of setup

void sleepNow() {
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);   
  noInterrupts ();          // make sure we don't get interrupted before we sleep
  sleep_enable ();          // enables the sleep bit in the mcucr register
  attachInterrupt (0, switchPressed, CHANGE);  // attach interrupt handler
  interrupts ();           // interrupts allowed now, next instruction WILL be executed
  sleep_cpu ();            // here the device is put to sleep
}
void loop ()
{
  sleepNow();
  // loop doing nothing 
} 
