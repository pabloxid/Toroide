/*
    lv_maxsonar.h - LV MaxSonar ultrasonic distance sensor Serial output decoding
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


#ifndef SONAR_H
#define SONAR_H

#include <Arduino.h>
#include "Dsp.h"
#include "settings.h"

#define SEARCH_R           -1
#define N_DIGITS            3

const float Inch2Cm = 2.54;

class LvMaxSonar {

  private:
  
    HardwareSerial* serial;
    float dist;
    int state;
    char buffer [N_DIGITS]; 
    RecursiveFilter filter;

  public:

    LvMaxSonar (HardwareSerial* port) : serial (port), filter (LPF, SONAR_FILTER_CUTOFF) { }
  
    void init () {
      serial->begin (9600); 
      state = SEARCH_R;
      dist = 100.0;               // valor arbitrariamente alto
    }
    
    void update () {
      while (serial->available()) {
        char c = serial->read();
        switch (state) {
          case SEARCH_R:
            if (c == 'R') state = 0;
            break;
          default:
            buffer [state++] = c;
            if (state == N_DIGITS) {
              state = SEARCH_R;
              dist = filter.update(atoi(buffer)*Inch2Cm);
            }
            break; 
        }
      }
    }
  
    float getDist () {
      return dist;
    }

};


#endif
