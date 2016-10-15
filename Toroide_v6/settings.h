/*
    Toroide v6 - Advanced Sumobot (Arduino Mega)
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


#ifndef SETTINGS_H
#define SETTINGS_H


///////////////////////////////////////////////////////////
///////////////////// MAIN ROUTINE ////////////////////////
///////////////////////////////////////////////////////////

const int UmbralSharp = 68;             // umbral de detección para los sensores sharp (cm)
const int UmbralSonar = 57;             // umbral de detección para los sonares (cm)
const int UmbralEmpuje = 13;            // umbral para pasar al estado EMPUJE (cm)
const int IdleVel = 90;                 // velocidad de búsqueda (rpm)
const int SlowTurnVel = 120;            // velocidad de giro en el paseo (rpm)
const int FastTurnVel = 160;            // velocidad de giro en el ataque (rpm)
const int AttackVel = 200;              // velocidad de apuntar al objetivo (rpm)
const int PushLowVel = 30;              // velocidad mínima de empuje (rpm)

#define FIX_TIME          400           // tiempo de los movimientos de corrección de línea blanca (ms)
#define CHG_DIR_TIME      400           // tiempo de cambio de dirección en la búsqueda (ms)
#define PUSH_TIMEOUT      600           // timeout para activar el 'escape', en el estado EMPUJAR (ms) 
#define PUSH_ESC_TIME     300           // duración del 'escape' en el estado EMPUJAR (ms) 

//#define DEBUG


///////////////////////////////////////////////////////////
//////////////////// ANALOG - SHARP ///////////////////////
///////////////////////////////////////////////////////////

#define ANALOG_NUM_SENSORS          6   // los 4 sharp y los 2 sensores de corriente
#define ANALOG_FILTER_POINTS        5   // move average filter para todos los sensores analógicos
#define ANALOG_SAMPLING_PERIOD      7   // tiempo entre sensor y sensor

#define LIN_SHARP_A     6787.0
#define LIN_SHARP_B     -3.0
#define LIN_SHARP_C     -4.0


///////////////////////////////////////////////////////////
////////////////////// LV-MAXSONAR ////////////////////////
///////////////////////////////////////////////////////////

#define SONAR_FILTER_CUTOFF       .2   


//////////////////////////////////////////////////
//////////////////// MOTORS //////////////////////
//////////////////////////////////////////////////

#define KP        .13      
#define KI        .07      
#define KD        .0035    

#define MAX_OUTPUT                255.0
#define MIN_OUTPUT               -255.0

#define RPM_SAMPLING_PERIOD       30               // muestreo de las RPM (ms)

#define COUNTS_PER_REVOLUTION     3200.0

#define HIGH_LOAD_THRESHOLD       33.0             // estos umbrales dependen del "agarre" del terreno
#define LOW_LOAD_THRESHOLD        23.0             // a mayor agarre, más alto el umbral

// factor de conversión de corriente: analogRead * .0376 Amps

//////////////////////////////////////////////////
//////////////////// ENCODERS ////////////////////
//////////////////////////////////////////////////

const int ENC_L_PIN_A = 18;   
const int ENC_L_PIN_B = 19;
const int ENC_R_PIN_A = 21;   
const int ENC_R_PIN_B = 20;


//////////////////////////////////////////////////
/////////////////// POLOLU_QTR ///////////////////
//////////////////////////////////////////////////

#define NEITHER                  -1
#define QTR_NUM_SENSORS           4

const int QtrThresh [QTR_NUM_SENSORS] = {75, 75, 75, 75};    // umbrales de detección de la línea blanca (us) [BLANCO < umbral < NEGRO]


///////////////////////////////////////////////////////////////////////
// función auxiliar para calcular el máximo valor dentro de un array //
///////////////////////////////////////////////////////////////////////

template <typename T>
T maxVal (T* array, int size) {
  int m = array [0];
  for (int i=1; i<size; i++) {
    if (array[i] > m) m = array[i];
  }
  return m;
}


///////////////////////////////////////////////////////////////////////
//         pequeña clasesita muy útil para manejar el estado         //
///////////////////////////////////////////////////////////////////////

template <typename T>
class ChangeTracker {
  private:
    T element;
    bool changed;
  public:
    ChangeTracker () : element ((T)0), changed (false) { }
    void operator = (T value) {
      if (value != element) {
        element = value;
        changed = true;
      }
    }
    T operator () () {
      return element;
    }
    bool isChanged () {
      if (changed) {
        changed = false;
        return true;
      }
      return false;
    }
};

#endif
