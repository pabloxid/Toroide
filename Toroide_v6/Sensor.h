/*
    Sensor.h - filter process for analog / Sharp distance sensors 
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

#ifndef SENSOR_H
#define SENSOR_H

#include "Dsp.h"
#include "settings.h"
#include <util/atomic.h>

class AnalogSensor {

  public :
    byte channel;
    volatile int value;
      
    AnalogSensor (int pin) {
      pinMode (pin, INPUT);
      channel = (pin >= A0? pin-A0 : pin);     // allow for channel or pin numbers
    }
  
    void update (int newValue) {
      value = filter.process (newValue);
    }

    float getValue () {
      int v;
      ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {v = value;}
      return v*1.0/ANALOG_FILTER_POINTS;
    }

  private:
    MovAvgFilter <int, ANALOG_FILTER_POINTS> filter;
  
};


class Sharp : public AnalogSensor {

  public :
    
    Sharp (int pin) : AnalogSensor (pin) { }
  
    float getDist () {
      int v;
      ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {v = value;}
      return LIN_A/(v+LIN_B)+LIN_C;
    }

  private:
    static const float LIN_A, LIN_B, LIN_C;
  
};


#endif
