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


void adelante (int velocidad) {
  motorL.setRpm (-velocidad);
  motorR.setRpm (velocidad);
}

void atras (int velocidad) {
  motorL.setRpm (velocidad);
  motorR.setRpm (-velocidad);
}

void izquierda (int velocidad) {
  motorL.setRpm (velocidad);
  motorR.setRpm (velocidad);
}

void derecha (int velocidad) {
  motorL.setRpm (-velocidad);
  motorR.setRpm (-velocidad);
}

void stop () {
  motorL.setRpm (0);
  motorR.setRpm (0);
}

void curva (byte direccion, int velocidad, float angulo) {                 // ángulo entre 0-1
  int velocidad2 = velocidad - 2*velocidad*angulo;
  switch (direccion) {
    case FRONT_L:
      motorL.setRpm (-velocidad2);
      motorR.setRpm (velocidad);     
      break;
    case FRONT_R:
      motorL.setRpm (-velocidad);
      motorR.setRpm (velocidad2);     
      break;
    case BACK_L:
      motorL.setRpm (velocidad2);
      motorR.setRpm (-velocidad);     
      break;
    case BACK_R:
      motorL.setRpm (velocidad);
      motorR.setRpm (-velocidad2);     
      break;
  }  
}


