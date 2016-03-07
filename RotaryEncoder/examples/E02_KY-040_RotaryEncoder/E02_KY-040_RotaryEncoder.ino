//  Extended: E02 KY-040 RotaryEncoder Demo
//
//  Web Search String: 
//    "ky-040 rotary decoder encoder module for arduino"
//
//  Functions:
//    Rotate CW:   count up         1..59, 1  or 1..31, 1  or 1..12, 1  or 0..99, 0
//    Rotate CCW:  count down       59..1, 59 or 32..1, 31 or 12..1, 12 or 99..0, 99
//    Push Button 
//      - short:   switch to next digit group;  HH or MM or SS or YY or MM or DD
//      - long:    ready                        20YY-MM-DDTHH:MM:SS
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
//     No external RC- or R-parts are used. Debouncing will be supported by Rotary Encoder library 'RotaryEncoder.h'
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
#else
#include "WProgram.h"
#endif

#include <RotaryEncoder.h>
#include <Time.h>  

// Default Rotary Encoder Object
RotaryEncoder rotaryEncoder = RotaryEncoder();  // CLK = pin 3, DAT = pin 2, SW = pin A1, CW = count up, CCW = count down

// ***************************************************************

void setup() {

  rotaryEncoder.setRange(1,12);                                 // set min and max encoder position
  
  Serial.begin(115200);
  Serial.print(F("E02 KY-040 RotaryEncoder Demo"));
  Serial.println(F(" (compiled " __DATE__ " " __TIME__ ")"));
}

// ***************************************************************

void loop() {                                                   // apply Rotary Encoder methods

  // Get Rotary Encoder status
  rotaryEncoder.startDebouncer();                               // has to be inside loop to enable debouncer
  rotaryEncoder.updateClearButton();                            // enable Button to clear recent encoder position if pushed, set to 0

  // Print changes
  if (rotaryEncoder.hasChanged()) {                             // verify if encoder position has changed
    volatile byte encoderPos = rotaryEncoder.encodedPos();      // update counter for the dial (values 1..12)
    Serial.print(F("Position: "));
    Serial.println(encoderPos, DEC);
  }
}

// ***************************************************************

