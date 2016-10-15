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


const float Sharp::LIN_A = LIN_SHARP_A*ANALOG_FILTER_POINTS;
const float Sharp::LIN_B = LIN_SHARP_B*ANALOG_FILTER_POINTS;
const float Sharp::LIN_C = LIN_SHARP_C;

AnalogSensor* sensors [ANALOG_NUM_SENSORS] = {&shFront, &shLeft, &shRight, &shBack, &motorL.sense, &motorR.sense};      // array de sensores indexados

void readSensors () {
  static int index = 0; 
  sensors[index]->update (ADCReadConv());
  index = (index+1) % ANALOG_NUM_SENSORS;
  ADCInitConv (sensors[index]->channel);
}  


//////////////////////////////////////////////////////////////////////////////////////
//                                 fast analog read                                 //
//////////////////////////////////////////////////////////////////////////////////////

void ADCInitConv (byte channel) {
  byte bitmask = _BV(MUX5);                                        // the MUX5 bit of ADCSRB selects whether we're reading from
  ADCSRB = (channel > 7? ADCSRB|bitmask : ADCSRB&~bitmask);        // ... channels 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high)
  ADMUX = (channel&0x07) | (1<<6);                                 // setea el canal ADC 
  //delayMicroseconds(2);
  ADCSRA |= _BV(ADSC);                                             // inicia una nueva conversión manual  
}

int ADCReadConv () {
  while (bit_is_set(ADCSRA, ADSC));                // espera hasta que se apague ADSC (está pronta la conversión)  
  return ADC;                                      // combine the two bytes
}

int ADCReadAndInitNewConv () {
  while (ADCSRA & _BV(ADSC)) ;                   // espera hasta que se apague ADSC (está pronta la conversión)  
  byte low  = ADCL;
  byte high = ADCH;
  ADCSRA |= _BV(ADSC);                           // inicia una nueva conversión manual
  return (high << 8) | low;                      // combine the two bytes
}


////////////////////////////////////////////////
//                    ISRs                    //
////////////////////////////////////////////////

void isrL_A () {encoderL.change (ChangeA);}
void isrL_B () {encoderL.change (ChangeB);}
void isrR_A () {encoderR.change (ChangeA);}
void isrR_B () {encoderR.change (ChangeB);}

// todo: convertir en timer con millis (o no...)
void timerISR () {
  static unsigned long count = 0;
  count ++;
  if (!(count%rpm_period)) {
    motorL.update();
    motorR.update();
  }
  if (!(count%analog_period)) {
    readSensors ();
  }  
}


//////////////////////////////////////////////////
//              rutinas auxiliares              //
//////////////////////////////////////////////////

void monitoring () {
  Serial.print("  State: "); Serial.print (state());
  Serial.print("  QTR: "); Serial.print (readQtr());
  Serial.print("  Sonar L: "); Serial.print (sonarIzq.getDist());
  Serial.print("  Sonar R: "); Serial.print (sonarDer.getDist());
  //Serial.print("  Sonar DIF: "); Serial.print ((sonarDer.getDist()-sonarIzq.getDist())/10.0);
  Serial.print("  sh front: "); Serial.print (shFront.getDist());
  Serial.print("  sh back: "); Serial.print (shBack.getDist());
  Serial.print("  sh left: "); Serial.print (shLeft.getDist());
  Serial.print("  sh right: "); Serial.print (shRight.getDist());
  //Serial.print("  total LOAD : "); Serial.print (motorL.getLoad()+motorR.getLoad());
  //Serial.print("  LOAD left: "); Serial.print (motorL.getLoad()); Serial.print(" right: "); Serial.print (motorR.getLoad());
  //Serial.print("  ERROR left: "); Serial.print (motorL.getError()); Serial.print(" right: "); Serial.print (motorR.getError()); 
  Serial.println();
}






