// ATtiny85 sleep mode, wake on pin change interrupt demo
// Author: Nick Gammon
// Date: 12 October 2013

// ATMEL ATTINY 84A / ARDUINO


#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management

const byte LED = 9;  // pin 3
const byte SWITCH = 0; // pin 3 / PCINT4

ISR (PCINT0_vect) 
 {
 // do something interesting here
 }
 
void setup ()
  {
  pinMode (LED, OUTPUT);
  pinMode (SWITCH, INPUT);
  digitalWrite (SWITCH, HIGH);  // internal pull-up
  
  // pin change interrupt (example for D4)
  PCMSK0  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF0);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE0);    // enable pin change interrupts 
  
  }  // end of setup

void loop ()
  {
  digitalWrite (LED, HIGH);
  delay (500); 
  digitalWrite (LED, LOW);
  delay (500); 
  goToSleep ();
  }  // end of loop
  
  
void goToSleep ()
  {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();                             
  sleep_disable();   
  power_all_enable();    // power everything back on
  }  // end of goToSleep 
