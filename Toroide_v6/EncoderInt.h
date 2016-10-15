/*
    EncoderInt.h - incremental quadrature encoder class
    Copyright (C) 2016 Pablo Gindel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENCODERINT_H
#define ENCODERINT_H

#include "Arduino.h"
#include "FastPin.h"

enum {ChangeA, ChangeB};

struct EncoderInt {
  FastPin pinA;
  FastPin pinB;
  boolean stateA, stateB;
  volatile int pulses;
  
  EncoderInt (int pinA_, int pinB_) 
  : pinA(pinA_), pinB(pinB_), pulses(0) 
  {
    pinA.setInput();
    pinB.setInput();
  }
  
  void change (boolean sentido) {
    if (sentido==ChangeA){
      stateA = pinA.read();
    } else {
      stateB = pinB.read();
    }
    sentido ^= (stateA == stateB); 
    pulses += (sentido? 1 : -1);
  }
  
};


#endif


