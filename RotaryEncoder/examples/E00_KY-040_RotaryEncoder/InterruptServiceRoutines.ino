// ***************************************************************
// This implementation based on  
// "Another Interrupt Library THAT REALLY WORKS" by rafbuff
// http://playground.arduino.cc/Main/RotaryEncoders 

// interrupt service routine vars
bool A_set = false;
bool B_set = false;

// ***************************************************************

// Interrupt on A changing state
void doEncoderA() {
  // debounce
  if ( rotating ) delay (1);                  // wait a little until the bouncing is done

  // Test transition, did things really change?
  if ( digitalRead(encoderPinA) != A_set ) {  // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if ( A_set && !B_set ) encoderPos += 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// ***************************************************************

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if ( rotating ) delay (1);                  // wait a little until the bouncing is done

  if ( digitalRead(encoderPinB) != B_set ) {  // debounce once more
    B_set = !B_set;
    
    //  adjust counter - 1 if B leads A
    if ( B_set && !A_set ) encoderPos -= 1;

    rotating = false;  // no more debouncing until loop() hits again
  }
}

// ***************************************************************

