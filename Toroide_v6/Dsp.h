/*
    DSP.h - Arduino DSP mini library
    Copyright (C) 2015 Pablo Gindel

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


#ifndef DSP_H
#define DSP_H

#include <arduino.h>


///////////////////////////////////////////////
//         ring buffer (con template)        //
///////////////////////////////////////////////

template <typename T, int S>
struct RingBuffer { 
  
  RingBuffer(): pos(0) {
    memset((void*)buffer, 0, S*sizeof(T));
  }
  
  void store (T value) {    
    pos = (pos+S-1) % S;
    buffer [pos] = value;
  }
  
  T get (int index) {
    return buffer [(pos+index)%S];
  }
  
  private:
    T buffer [S];
    int pos;
};


typedef RingBuffer <float, 3> RBF3;


//////////////////////////////////////////////////////////
//                  filtros recursivos                  //
//////////////////////////////////////////////////////////

enum FilterType {LPF, HPF};

struct RecursiveFilter {
  
  float a0, a1, b1;                      // coeficientes
  RBF3 input_, output_;                  // ring buffers
  
  RecursiveFilter (FilterType type, float cutoff) {           // cutoff entre 0 y 0.5    
    // calcula los coeficientes
    b1 = exp (-2*PI*cutoff);
    switch (type) {
      case LPF:
        a0 = 1-b1;
        a1 = 0;
        break;
      case HPF:
        a0 = (1+b1)/2.0;
        a1 = -a0;
        break;
    }
  }
  
  float update (float value) {
    input_.store (value);
    output_.store (a0*input_.get(0) + a1*input_.get(1) + b1*output_.get(0));
    return output_.get(0);
  }
  
};


///////////////////////////////////////////////////////////////////
//                     moving average filter                     //
///////////////////////////////////////////////////////////////////         

template <typename T, int S>
class MovAvgFilter {
  public:
    T process (T new_value) {
      filter_value -= rb.get(S-1);
      rb.store (new_value);
      filter_value += rb.get(0);
      return filter_value;
    }
  private:
    RingBuffer <T,S> rb;
    T filter_value;
};


#endif
