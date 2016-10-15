/*
    Motors.h - RPM PID control for encoder DC motors with VNH2SP30-E integrated H-bridge
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


#ifndef MOTORS_H
#define MOTORS_H

#include "Arduino.h"
#include "settings.h"
#include "Sensor.h"
#include "FastPin.h"

const float KIdt = KI * RPM_SAMPLING_PERIOD / 1000.0;
const float KDdt = KD * 1000.0 / RPM_SAMPLING_PERIOD;
const float rpmConv = RPM_SAMPLING_PERIOD * COUNTS_PER_REVOLUTION / 60000.0;


class DcRpmMotor {

  public:
 
  DcRpmMotor (int pinA_, int pinB_, int pinPWM_, int pinSense_, volatile int* pulsesSource_) 
  : sense(pinSense_), pinA(pinA_), pinB(pinB_), pinPWM(pinPWM_), pulsesSource(pulsesSource_), error(0), setPoint(0), iTerm(0) 
  {
    pinA.setOutputLow();
    pinB.setOutputLow();
  }

  void setPWMfreq() {
    pinPWM.setPWMfreq();
  }
  
  void update () {
    int lastError = error;
    *pulsesSource -= setPoint;
    error = setPoint - *pulsesSource; 
    float pTerm = error * KP;
    iTerm = constrain (iTerm + KIdt*error, MIN_OUTPUT, MAX_OUTPUT);    // dt=1
    float dTerm = (error-lastError)*KDdt;                              // dt=1
    float output = constrain (pTerm+iTerm+dTerm, MIN_OUTPUT, MAX_OUTPUT);
    setPwm (output);
  }
    
  void setRpm (int rpm) {setPoint = rpm * rpmConv;}
  inline float getLoad () {return sense.getValue();}
  inline int getError () {return error;}

  inline int getPwm () {
    return pinPWM.get();
  }

  AnalogSensor sense;            // mide la corriente del motor (público para mayor comodidad)

  private:
    FastPin pinA;
    FastPin pinB;
    FastPwmPin pinPWM;
    
    volatile int* pulsesSource;
    
    int error;
    int setPoint;
    float iTerm;
   
    void setPwm (float control) {
      // set digital pins
      if (control > 0) {
        pinA.setLow();
        pinB.setHigh();
      }
      else if (control < 0) {
        pinA.setHigh();
        pinB.setLow();
      }
      else {
        pinA.setLow();
        pinB.setLow();
      }
      #ifndef DEBUG
      // set PWM
      pinPWM.set (abs(control));
      #endif
    } 
};



#endif
