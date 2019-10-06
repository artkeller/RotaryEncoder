//  Getting Started: E00 KY-040 RotaryEncoder Demo
//
//  Web Search String: 
//    "ky-040 rotary decoder encoder module for arduino"
//
//  Functions:
//    Rotate CW:   count up         0..15, 0
//    Rotate CCW:  count down       0, 15..0
//    Push Button: reset counter to 0
//
//
//  Wiring:
//         KY-040                 Arduino AVR (UNO, Nano, etc.)
//    +-------------+
//    |             |--- CLK  >   o Pin 3
//    |             |--- DAT  >   o Pin 2
//    |   top view  |--- SW   >   o Pin A1
//    |             |--- VCC  >   o 5.0V
//    |             |--- GND  >   o GND
//    +-------------+
//
//     No external RC- or R-parts are used. Debouncing will be supported by Rotary 
//     Encoder Interrupt Handler 'RotaryEncoder.ino'
//
//  Copyright (c) 2015 Thomas Walloschke (artkeller at gmx.de). All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

// ***************************************************************

#if ARDUINO >= 100
#include "Arduino.h"
#else  /* !ARDUINO */
#include "WProgram.h"
#endif /* end !ARDUINO */

// This implementation based on  
// "Another Interrupt Library THAT REALLY WORKS" by rafbuff
// http://playground.arduino.cc/Main/RotaryEncoders 

enum PinAssignments {                                           // CW = count up, CCW = count down
  encoderPinA = 3,                                              // DAT
  encoderPinB = 2,                                              // CLK
  clearButton = A1                                              // BUTTON
};

volatile byte encoderPos = 0;                                   // a counter for the dial
byte lastReportedPos = 1;                                       // change management
static bool rotating = false;                                   // debounce management

// ***************************************************************

void setup() {

  // Set input and urn on pullup resistors
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(clearButton, INPUT_PULLUP);

  // Encoder pins on interrupt
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  // Initialize Serial
  Serial.begin(115200);
  Serial.print(F("E00 KY-040 RotaryEncoder Demo"));
  Serial.println(F(" (compiled " __DATE__ " " __TIME__ ")"));
}

// ***************************************************************

// Main loop
// Work is done by interrupt service routines, this one prints stuff
void loop() {

  rotating = true;                                              // reset the debouncer

  // Reset counter if button has ben pushed
  if ( ( clearButton < A0 )                                     // check analog or digital pin
       ? ( not digitalRead(clearButton) )                       // undebounced (!)
       : ( analogRead(clearButton) < 100 ) )                    // threshold approx. 10% of VCC
    encoderPos = 0;                                             // reset encoder position

  // Print changes
  if (lastReportedPos != encoderPos) {                          // verify if encoder position has changed
    Serial.print(F("Position: "));
    Serial.println(encoderPos % 16, DEC);                       // print counter for the dial (values 0..15)
    lastReportedPos = encoderPos;                               // update new position 
  }
}

// ***************************************************************

