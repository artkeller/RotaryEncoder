/*
* @title	Arduino Rotary Encoder Library
* @file		RotaryEncoder.cpp
* @version	Version 0.1 A4
* @author	Thomas Walloschke
* @contact	mailto:artkeller@gmx.de
*
* @synopsis
*   	#include "RotaryEncoder.h"				// include this lib
*	RotaryEncoder rotaryEncoder = RotaryEncoder();		// default initializing of Rotary Encoder
*								// CLK = pin 3, DAT = pin 2, SW = pin A1, CW = count up, CCW = count down
*								// one instance only is allowed due to plain ISR usage with global data
*	// Aternative initialization
*	RotaryEncoder rotaryEncoder = RotaryEncoder(2,3,4);	// individual initializing of Rotary Encoder
*								// CLK = pin 2, DAT = pin 3, SW = pin 4, CW = count down, CCW = count up
*
*	void loop() {						// apply Rotary Encoder methods
*		...
*	  rotaryEncoder.startDebouncer();			// has to be inside loop() to enable rotary debouncer
*	  rotaryEncoder.updateClearButton();			// enable Button to clear recent encoder position if pushed, set to 0
*
*	  if (rotaryEncoder.hasChanged()) {			// verify if encoder position has changed
*		volatile byte encoderPos = rotaryEncoder.encodedPos() % 16;   // update counter for the dial (values 0..15)
*		do_something_with(encoderPos);
*	  }
*		...
*	}
*
* @description	Rotary Encoder Library for measuring quadrature encoded signals
*
* @examples
*	For KY-040 Rotary Encoder Module
*	Web Search String:
*	  "ky-040 rotary decoder encoder module for arduino"
*
*	examples/E01_KY-040_RotaryEncoder/E01_KY-040_RotaryEncoder.ino		- E01 KY-040 Rotary Encoder Demo w/ "RotaryEncoder.h"
*	examples/E00_KY-040_RotaryEncoder/E00_KY-040_RotaryEncoder.ino		- E00 KY-040 Rotary Encoder Demo w/o "RotaryEncoder.h"
*
* @disclaimer The library is tested with some ARV Arduino only. It may possibly work with other MCUs.
*
* @acklowledge
*	This code was inspired by
*
*		1.	"Another Interrupt Library THAT REALLY WORKS" by rafbuff
*			http://playground.arduino.cc/Main/RotaryEncoders
*		2.	"Encoder Library, for measuring quadrature encoded signals" by Paul Stoffregen
*			http://www.pjrc.com/teensy/td_libs_Encoder.html
*
*	Thanks to all.
*
* @copyright	Copyright (c) 2015 Thomas Walloschke. All rigths reserved.
* @licence
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in
*	all copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*	THE SOFTWARE.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else  // not ARDUINO
#include "WProgram.h"
#endif // ARDUINO

#include "RotaryEncoder.h"

// Global vars
volatile byte encoderPinA;			  // CKK or DAT, depending on initialization, used by Interrupt service routine
volatile byte encoderPinB;			  // DAT or CLK, depending on initialization, used by Interrupt service routine
byte pushButton;				  // SW

// Interrupt service routine vars
volatile bool A_set		= false;
volatile bool B_set		= false;
volatile byte minEncoderPos   	= 0;		
volatile byte maxEncoderPos   	= 255;			
volatile byte encoderPos      	= minEncoderPos;  // a counter for the dial
volatile byte lastReportedPos 	= maxEncoderPos;  // Change management, seeded
volatile bool rotating 		= false;	  // debounce management

// Prototypes of Interrupt service routines
void _doEncoderA();
void _doEncoderB();

/*
* @constructor
* @parameter		CLK sets the digital pin that CLK is connected to
* @parameter		DAT sets the digital pin that DAT is connected to
* @parameter		SW  sets the digital or analog pin that SW  is connected to
* @returns		RotaryEncoder object
* @globalvar		encoderPinA		w
* @globalvar		encoderPinB		w
* @globalvar		pushButton		w
* @pinmode		encoderPinA		înput pullup
* @pinmode		encoderPinB		înput pullup
* @pinmode		pushButton		înput pullup
* @attisr		_doEncoderA		encoderPinA
* @attisr		_doEncoderB		encoderPinB
*/
RotaryEncoder::RotaryEncoder(byte CLK, byte DAT, byte SW) {
	
	encoderPinA = CLK;
	encoderPinB = DAT;
	pushButton  = SW;
	
	// Set input and turn on pullup resistors
	pinMode(encoderPinA, INPUT_PULLUP);	// interruptable digital pin
	pinMode(encoderPinB, INPUT_PULLUP);	// interruptable digital pin
	pinMode(pushButton,  INPUT_PULLUP);	// analog or digital pin!
	
	// Encoder pins on interrupt
	attachInterrupt(digitalPinToInterrupt(encoderPinA), _doEncoderA, CHANGE);
	attachInterrupt(digitalPinToInterrupt(encoderPinB), _doEncoderB, CHANGE);
}

/*
* @method		setRange
* @parameter		min
* @parameter		max
* @returns		---
* @globalvar		minEncodedPos	w
* @globalvar		maxEncodedPos	w
* @globalvar		lastEncodedPos	w
*/
void RotaryEncoder::setRange(byte min, byte max){
	if ( min < max ) {
		encoderPos	= minEncoderPos   = min;
		lastReportedPos = maxEncoderPos   = max;
	}
}

/*
* @method		startDebouncer
* @parameter		---
* @returns		---
* @globalvar		rotating	w
*/
void RotaryEncoder::startDebouncer() {
	rotating = true;		// start the debouncer
}

/*
* @method		encodedPos
* @parameter		---
* @returns		encoded position (minEncoderPos..maxEncodedPos)
* @globalvar		encoderPos	r
* @descriptionn Calculate encoded position between minEncoderPos and maxEncodedPos
*/
byte RotaryEncoder::encodedPos() {
	return encoderPos;
}

/*
* @method		hasChanged
* @parameter		---
* @returns		true  if position changed
* @returns		false if position stayed
* @globalvar		lastReportedPos	rw
* @globalvar		encoderPos	r
*/
bool RotaryEncoder::hasChanged() {

	if (lastReportedPos != encoderPos) {		// change comparision
		lastReportedPos = encoderPos;		// update new position
		return true;
		} else {
		return false;
	}
}

/*
* @method		updateClearButton
* @parameter		---
* @returns		---
* @globalvar		minEncoderPos	r
* @globalvar		encoderPos	w
* @description	reset encoder to min position if button has been pushed
*/
void RotaryEncoder::updateClearButton(){
	if ( doPushButton() ) encoderPos = minEncoderPos;
}

/*
* @method		hasButtonPushed
* @parameter		---
* @returns		true  if button has been pushed
* @returns		false if button has not been pushed
*/
bool RotaryEncoder::hasButtonPushed(){
	return doPushButton();				// check push-button (undebounced)
}

/*
* @method		doPushButton
* @parameter		---
* @returns		true  if button pushed
* @returns		false if button unpushed
* @description	Simple input polling of Button
*/
bool RotaryEncoder::doPushButton(){
	return
	( pushButton < A0 )				// check analog or digital pin
	? ( not digitalRead(pushButton) )		// undebounced (!)
	: ( analogRead(pushButton) < 100 ) ;		// threshold approx. 10% of VCC
}

/*
* @function		_doEncoderA
* @isr			int 0 or 1, depending of initialization
* @trigger		pin 2 or 3, depending of initialization
* @parameter		---
* @returns		---
* @globalvar		rotating	rw
* @globalvar		encoderPinA	r
* @globalvar		A_set		rw
* @globalvar		B_set		r
* @globalvar		encoderPos	w
* @globalvar		minEncoderPos	r
* @globalvar		maxEncoderPos	r
* @description	Interrupt on A changing state
*/
void _doEncoderA() {
	
	// Debounce
	if ( rotating ) delay ( 1 );			// wait a little until the bouncing is done

	// Test transition, did things really change?
	if ( digitalRead(encoderPinA ) != A_set ) { 	// debounce once more
		A_set = !A_set;

		// Adjust counter + if A leads B
		if ( A_set && !B_set ) encoderPos = (encoderPos >= maxEncoderPos) ? minEncoderPos : ++encoderPos;

		rotating = false;  // no more debouncing until resetDebouncer() hits again
	}
}

/*
* @function		_doEncoderB
* @isr			int 0 or 1, depending of initialization
* @trigger		pin 2 or 3, depending of initialization
* @parameter		---
* @returns		---
* @globalvar		rotating	rw
* @globalvar		encoderPinB	r
* @globalvar		B_set		rw
* @globalvar		A_set		r
* @globalvar		encoderPos	w
* @globalvar		minEncoderPos	r
* @globalvar		maxEncoderPos	r
* @description	Interrupt on B changing state
*/
void _doEncoderB() {
	
	// Debounce
	if ( rotating ) delay ( 1 );			// wait a little until the bouncing is done
	
	// Test transition, did things really change?
	if ( digitalRead(encoderPinB ) != B_set ) { 	// debounce once more
		B_set = !B_set;
		
		//  Adjust counter - 1 if B leads A
		if ( B_set && !A_set ) encoderPos = (encoderPos <= minEncoderPos) ? maxEncoderPos : --encoderPos;

		rotating = false;  // no more debouncing until resetDebouncer() hits again
	}
}

/*
* @changelog
*   2016-03-05 - Thomas Walloschke : Documentation Update
*   2015-12-26 - Thomas Walloschke : Documentation
*   2015-12-25 - Thomas Walloschke : Class extention
*   2015-12-23 - Thomas Walloschke : Initial Class implementation
*/

