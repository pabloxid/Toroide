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


///////////////////////////////////////////////////////////////////////////////////////
// retorna el primer sensor que está por debajo del umbral, NEITHER si no es ninguno //
///////////////////////////////////////////////////////////////////////////////////////

const int QTR_TIMEOUT = maxVal (QtrThresh, QTR_NUM_SENSORS);       // el timeout es igual al máximo umbral

int readQtr () {
    for (int i=0; i<QTR_NUM_SENSORS; i++) {
        qtrPins[i].setOutputHigh();              // make sensor line an output; drive sensor line high
    }
    delayMicroseconds(10);                       // charge lines for 10 us
    MsTimer2::stop();
    for (int i=0; i<QTR_NUM_SENSORS; i++) {
        qtrPins[i].setInput();                   // make sensor line an input; important: disable internal pull-up!
    }
    unsigned long startTime = micros();
    int elapsed;
    do {
      elapsed = micros()-startTime;
      for (int i=0; i<QTR_NUM_SENSORS; i++) {
        if (!qtrPins[i].read() && elapsed<QtrThresh[i]) {     // eliminar la segunda condición para threshold único
          MsTimer2::start();
          return i;
        }
      }
    } while (elapsed < QTR_TIMEOUT);
    MsTimer2::start();
    return NEITHER;
}


////////////////////////////////////////////////////
//                  calibración                   //
////////////////////////////////////////////////////

#define ABSOLUTE_TIMEOUT   2500
#define TEST_LOOP          3000

void calibrar () {
  Serial.println ("Calibracion: presione una tecla cuando este listo");
  while (!Serial.available());
  Serial.read();
  adelante (20);
  int valores [QTR_NUM_SENSORS];
  int minimos [QTR_NUM_SENSORS] = {ABSOLUTE_TIMEOUT, ABSOLUTE_TIMEOUT, ABSOLUTE_TIMEOUT, ABSOLUTE_TIMEOUT};
  int maximos [QTR_NUM_SENSORS] = {0, 0, 0, 0};
  for (int f=0; f<TEST_LOOP; f++) {
    readQtr (valores);
    for (int i=0; i<QTR_NUM_SENSORS; i++) {
      if (valores[i] < minimos[i]) minimos[i] = valores[i];
      if (valores[i] > maximos[i]) maximos[i] = valores[i];
    }
    delay (1);
  }
  stop ();
  Serial.print ("Los minimos registrados son: ");
  for (int i=0; i<QTR_NUM_SENSORS; i++) {
    Serial.print (minimos[i]);
    Serial.print (", ");
  }
  Serial.println ();
  Serial.print ("Los maximos registrados son: ");
  for (int i=0; i<QTR_NUM_SENSORS; i++) {
    Serial.print (maximos[i]);
    Serial.print (", ");
  }
  Serial.println ();
}

// esta es la readQtr original, que retorna los valores de tiempo absolutos

void readQtr (int* values) {
    for (int i=0; i<QTR_NUM_SENSORS; i++) {
        qtrPins[i].setOutputHigh();
        values [i] = -1;
    }
    delayMicroseconds(10);                       // charge lines for 10 us
    MsTimer2::stop();
    for (int i=0; i<QTR_NUM_SENSORS; i++) {
        qtrPins[i].setInput();
    }
    unsigned long startTime = micros();
    int elapsed;
    do {
      elapsed = micros()-startTime;
      for (int i=0; i<QTR_NUM_SENSORS; i++) {
        if (!qtrPins[i].read() && values[i]==-1) {
          values [i] = elapsed;
        }
      }
    } while (elapsed < ABSOLUTE_TIMEOUT);
    MsTimer2::start();
}





