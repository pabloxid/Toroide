/*
    FastPin.h - Arduino fast GPIO
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


#ifndef FastPin_H
#define FastPin_H

#include "Arduino.h"


class FastPin {

public:  
  
  FastPin (int pin) {
    uint8_t port = digitalPinToPort(pin);
    modeReg = portModeRegister(port);
    outputReg = portOutputRegister(port);
    inputReg = portInputRegister(port);
    pinBitMask = digitalPinToBitMask(pin);
  }

  inline void setInput () {
    *modeReg &= ~pinBitMask;        // pinMode(INPUT)
    *outputReg &= ~pinBitMask;      // digitalWrite(LOW)
  }
  
  inline void setInputPullup () {
    *modeReg &= ~pinBitMask;        // pinMode(INPUT)
    *outputReg |= pinBitMask;       // digitalWrite(HIGH)
  }
  
  inline void setOutputHigh () {
    *outputReg |= pinBitMask;        // digitalWrite(HIGH)
    *modeReg |= pinBitMask;          // pinMode(OUTPUT)
  } 

  inline void setOutputLow () {
    *outputReg &= ~pinBitMask;       // digitalWrite(LOW)
    *modeReg |= pinBitMask;          // pinMode(OUTPUT)
  } 

  inline bool read () {
    return (*inputReg & pinBitMask);    // digitalRead()
  } 

  inline void setHigh () {
    *outputReg |= pinBitMask;        // digitalWrite(HIGH)
  } 

  inline void setLow () {
    *outputReg &= ~pinBitMask;       // digitalWrite(LOW)
  } 

  inline void setOutput () {
    *modeReg |= pinBitMask;          // pinMode(OUTPUT)
  } 
  

private:
  
  // pin registers & masks for fast GPIO
  volatile uint8_t* modeReg;
  volatile uint8_t* outputReg;
  volatile uint8_t* inputReg;
  uint8_t pinBitMask;
  
};


//////////////////////////////////////////////////////////////////////////////////////
//                            FastPwmPin (versión alfa)                             //
// ATENCIÓN: sólo funciona con timers de 16bits en la Mega2560 (timers 1, 3, 4 y 5) //
//////////////////////////////////////////////////////////////////////////////////////

class FastPwmPin {

public:  
  
  FastPwmPin (int pin) {
    pinMode (pin, OUTPUT);           // setea el pin en OUTPUT
    
    // registros y bits que no dependen del canal
    switch (digitalPinToTimer(pin)) {
      case TIMER1A: case TIMER1B: case TIMER1C:
        ctrRegA = &TCCR1A;
        ctrRegB = &TCCR1B;
        csBitMask0 = _BV(CS10);
        csBitMask1 = _BV(CS11);
        csBitMask2 = _BV(CS12);
        break;
      case TIMER3A: case TIMER3B: case TIMER3C:
        ctrRegA = &TCCR3A;
        ctrRegB = &TCCR3B;
        csBitMask0 = _BV(CS30);
        csBitMask1 = _BV(CS31);
        csBitMask2 = _BV(CS32);
        break;
      case TIMER4A: case TIMER4B: case TIMER4C:
        ctrRegA = &TCCR4A;
        ctrRegB = &TCCR4B;
        csBitMask0 = _BV(CS40);
        csBitMask1 = _BV(CS41);
        csBitMask2 = _BV(CS42);
        break;
      case TIMER5A: case TIMER5B: case TIMER5C:
        ctrRegA = &TCCR5A;
        ctrRegB = &TCCR5B;
        csBitMask0 = _BV(CS50);
        csBitMask1 = _BV(CS51);
        csBitMask2 = _BV(CS52);
        break;
    }
    
    // registros y bits que sí dependen del canal
    switch (digitalPinToTimer(pin)) {
      case TIMER1A:
        ocr = &OCR1A;
        conBitMask = _BV(COM1A1);
        break;
      case TIMER1B:
        ocr = &OCR1B;
        conBitMask = _BV(COM1B1);
        break;
      case TIMER1C:
        ocr = &OCR1C;
        conBitMask = _BV(COM1C1);
        break;
      case TIMER3A:
        ocr = &OCR3A;
        conBitMask = _BV(COM3A1);
        break;
      case TIMER3B:
        ocr = &OCR3B;
        conBitMask = _BV(COM3B1);
        break;
      case TIMER3C:
        ocr = &OCR3C;
        conBitMask = _BV(COM3C1);
        break;
      case TIMER4A:
        ocr = &OCR4A;
        conBitMask = _BV(COM4A1);
        break;
      case TIMER4B:
        ocr = &OCR4B;
        conBitMask = _BV(COM4B1);
        break;
      case TIMER4C:
        ocr = &OCR4C;
        conBitMask = _BV(COM4C1);
        break;
      case TIMER5A:
        ocr = &OCR5A;
        conBitMask = _BV(COM5A1);
        break;
      case TIMER5B:
        ocr = &OCR5B;
        conBitMask = _BV(COM5B1);
        break;
      case TIMER5C:
        ocr = &OCR5C;
        conBitMask = _BV(COM5C1);
        break;
    }
        
    // connect port
    *ctrRegA |= conBitMask;
  }

  inline void set (int pwmVal) {
    *ocr = pwmVal;
  }
  
  inline int get() {
    return *ocr;
  }

  /* 
   * setea la frecuencia del PWM
   * NOTA: la init() de Arduino hace una inicialización..
   * .. de los timers, en la cual descansa esta clase.
   * Por eso este método hay que llamarlo desde el setup()
  */
  void setPWMfreq() {
    // ~30 KHz
    //*ctrRegB |= csBitMask0;
    //*ctrRegB &= ~csBitMask1;
    //*ctrRegB &= ~csBitMask2;
    
    // ~4 KHz
    *ctrRegB &= ~csBitMask0;
    *ctrRegB |= csBitMask1;
    *ctrRegB &= ~csBitMask2;  
  }
    
private:
  // timer registers & masks for PWM
  volatile uint16_t* ocr;
  volatile uint8_t* ctrRegA;        
  volatile uint8_t* ctrRegB;
  uint8_t conBitMask;
  uint8_t csBitMask0;
  uint8_t csBitMask1;
  uint8_t csBitMask2;
    
};


#endif

