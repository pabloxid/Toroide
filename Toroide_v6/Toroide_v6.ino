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

//
// TODO: calcular automáticamente los umbrales QTR y de corriente... ¿se podrá?
//

#include <MsTimer2.h>
#include "Motors.h"
#include "EncoderInt.h"
#include "Sensor.h"
#include "settings.h"
#include "Timer.h"
#include "lv_maxsonar.h"
#include "FastPin.h"


// encoders

EncoderInt encoderL (ENC_L_PIN_A, ENC_L_PIN_B);
EncoderInt encoderR (ENC_R_PIN_A, ENC_R_PIN_B);

// motores DC

DcRpmMotor motorL (4, 9, 6, A3, &encoderR.pulses);    // pines: 4, 9, 6 y A3
DcRpmMotor motorR (7, 8, 5, A2, &encoderL.pulses);    // pines: 7, 8, 5 y A2

// sensores de distancia Sharp

Sharp shFront (A14);   // pin A14
Sharp shLeft  (A13);   // pin A13
Sharp shRight (A15);   // pin A15
Sharp shBack  (A12);   // pin A12

// sensores de distancia LV-MAXSONAR

LvMaxSonar sonarIzq (&Serial2);
LvMaxSonar sonarDer (&Serial3);

// pololu QTR1RC sensors

FastPin qtrPins [QTR_NUM_SENSORS] = {{33}, {31}, {29}, {27}};      // pines 33, 31, 29, 27

enum {FRONT_R, FRONT_L, BACK_R, BACK_L};                           // posiciones de los sensores

// hardware timer stuff

int gcd (int A, int B) {do {int t=B; B=A%B; A=t;} while(B); return A;}   // greatest common divisor

const int tick = gcd (RPM_SAMPLING_PERIOD, ANALOG_SAMPLING_PERIOD);
const int rpm_period = RPM_SAMPLING_PERIOD / tick;
const int analog_period = ANALOG_SAMPLING_PERIOD / tick;


////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////
////                        SETUP                         ////
////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////  

void setup() {

  // motors
  motorL.setPWMfreq();
  motorR.setPWMfreq();
    
  // interrupts
  attachInterrupt (digitalPinToInterrupt(ENC_L_PIN_A), isrL_A, CHANGE);
  attachInterrupt (digitalPinToInterrupt(ENC_L_PIN_B), isrL_B, CHANGE);
  attachInterrupt (digitalPinToInterrupt(ENC_R_PIN_A), isrR_A, CHANGE);
  attachInterrupt (digitalPinToInterrupt(ENC_R_PIN_B), isrR_B, CHANGE);
  
  Serial.begin (9600);
  Serial.setTimeout (100);

  sonarIzq.init();
  sonarDer.init();
  
  MsTimer2::set (tick, timerISR);                
  MsTimer2::start();

  randomSeed (analogRead(A8));              // A8 es un pin analógico libre

  // interruptor de arranque
  delay (500);
  while (shBack.getDist() < UmbralSharp) ;

}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////                        LOOP                          ////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

enum SumoState {BUSCAR, CORREGIR, APUNTAR, EMPUJAR}; 

ChangeTracker <SumoState> state;              // estado del robot (arranca en 'BUSCAR')
Timer timer;                                  // timer para agendar cambios de dirección

void loop  () {

  // habilitar esto para testear los sensores QTR
  //calibrar(); return;

  // runtime updates
  sonarIzq.update();
  sonarDer.update();

  // QTR process
  processQTR();

  // distance process
  if (state() != CORREGIR)
    distanceProcess();
  
  // algoritmo de búsqueda
  if (state() == BUSCAR) 
    search();
  
  // algoritmo de empuje
  else if (state() == EMPUJAR) 
    push();

  // monitoring  
  //monitoring ();

  delay (1);   // este delay es importante para que "respire" y pueda ejecutar las interrupciones
   
}

