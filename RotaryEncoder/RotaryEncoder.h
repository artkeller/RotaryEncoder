/*
* @title	Arduino Rotary Encoder Library
* @file		RotaryEncoder.h
* @version	Version 0.1 A3
* @author	Thomas Walloschke
* @contact	mailto:artkeller@gmx.de
*
* @description	Rotary Encoder Library for measuring quadrature encoded signals
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
*
* @history
*	0.1 A3 - 2015-12-25	- Thomas Walloschke : Documentaion Update
*	0.1 A2 - 2015-12-25	- Thomas Walloschke : Class Update
*	0.1 A1 - 2015-12-23	- Thomas Walloschke : Initial Release for AVR
*/

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#if ARDUINO >= 100
#include "Arduino.h"
#else  // not ARDUINO
#include "WProgram.h"
#endif // ARDUINO


#ifdef __AVR__
#ifdef __cplusplus

class RotaryEncoder
{
	public:
	// Arduino Uno, Nano, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc...
	#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__)
	// Constructor
	RotaryEncoder(byte CLK = 3, byte DAT = 2, byte SW = A1); 
	#else	// other boards or MCUs
	#warning "This version does not support those architecture - please adopt, test and respond result to me - Thank you!"
	#endif	// ARV architecture
	
	void setRange(byte min, byte max);			// Set min and max encoder position range
	void startDebouncer();					// Start debouncer - mandatory call inside 'loop()' to enable rotary debouncer
	byte encodedPos();					// Recent position min..max (default 0..255)
	bool hasChanged();					// Position has been changed
	void updateClearButton();				// Use Button to clear recent encoder position if pushed, set to 0
	bool hasButtonPushed();					// Check Button if it has been pushed, transparent for caller

	private:
	bool doPushButton();					// Check button state
};

#endif // __cplusplus
#else  // other MCUs
#warning "This version does not support those architecture - please adopt, test and respond result to me - Thank you!"
#endif // __AVR__
#endif // RotaryEncoder_h
