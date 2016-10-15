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


//////////////////////////////////////////////////////////////////
//        procesa la información de los sensores de piso        //
//////////////////////////////////////////////////////////////////

void processQTR () {
  
  static int phase;
  static int lastQtr;

  int qtr = readQtr();
  if (qtr != NEITHER) {
    state = CORREGIR;
    lastQtr = qtr;
    phase = 0;
  }
  
  if (state() != CORREGIR) return;
  
  switch (phase) {
    case 0: {
      if (qtr == NEITHER) {
        timer.set (FIX_TIME);
        phase = 1;
      } else {
        int dir;
        switch (qtr) {
          case FRONT_L: dir = BACK_L; break;
          case FRONT_R: dir = BACK_R; break;
          case BACK_L: dir = FRONT_R; break;
          case BACK_R: dir = FRONT_L; break;
        }
        curva (dir, IdleVel, .5);
      }
      break;
    }
    case 1: {
      if (timer.check()) {
        int dir;
        switch (lastQtr) {
          case FRONT_L: case BACK_L: dir = FRONT_R; break;
          case FRONT_R: case BACK_R: dir = FRONT_L; break;
        }
        curva (dir, IdleVel, .33);
        // salida
        timer.reset();
        state = BUSCAR; 
      }
      break;
    }
  }
}


//////////////////////////////////////////////////////////////
//                     distance process                     //
//////////////////////////////////////////////////////////////

void distanceProcess () {
    float frontDist = shFront.getDist();
    if (frontDist < UmbralEmpuje && frontDist > 0) 
      state = EMPUJAR;
    else if (frontDist < UmbralSharp && frontDist > 0) {
      adelante (AttackVel);
      state = APUNTAR;
    }
    else {
      float l = shLeft.getDist();
      float r = shRight.getDist();
      float b = shBack.getDist();
      bool left = (l < UmbralSharp && l > 0);
      bool middleLeft = (sonarIzq.getDist() < UmbralSonar);
      bool middleRight = (sonarDer.getDist() < UmbralSonar);
      bool right = (r < UmbralSharp && r > 0);
      bool back = (b < UmbralSharp && b > 0);
      
      byte distCode = left<<4 | middleLeft<<3 | middleRight<<2 | right<<1 | back;
      
      switch (distCode) {
        case 0b10000: case 0b11000:     // izquierda lateral o izquierda ambos
          izquierda (FastTurnVel);
          state = APUNTAR;
          break;
        case 0b00010: case 0b00110:     // derecha lateral o derecha ambos
          derecha (FastTurnVel);
          state = APUNTAR;
          break;
        case 0b01000:                           // izquierda sólo sonar
          //izquierda (SlowTurnVel);
          curva (FRONT_L, SlowTurnVel, .8);
          state = APUNTAR;
          break;
        case 0b00100:                           // derecha sólo sonar
          //derecha (SlowTurnVel);
          curva (FRONT_R, SlowTurnVel, .8);
          state = APUNTAR;
          break;
        case 0b01100:                           // ambos sonares (caso raro pero no imposible)
          adelante (SlowTurnVel);
          state = APUNTAR;
          break;
        case 0b00001: {                         // sensor de atrás
          static boolean dir;
          if (state() != APUNTAR) dir = random(2);
          if (dir) derecha (FastTurnVel);
          else izquierda (FastTurnVel);
          state = APUNTAR;
          break;  
        }
        case 0b00000:                           // ningún sensor              
          if (state() != BUSCAR) {
            timer.fire();
            state = BUSCAR;
          }
          break;
        default:                                // combinaciones muy raras o imposibles
          stop();
          break;   
      }
    }
}


///////////////////////////////////////////////////////////////
//                   algoritmo de búsqueda                   //
///////////////////////////////////////////////////////////////

void search () {
  static float curve = random (-.6, .6);                        // dirección de la búsqueda
  if (state.isChanged()) curve = 0;
  
  if (timer.check()) {
    curve += (random(-28, 28)/100.0 - curve*.1);
    curve = constrain (curve, -1, 1);
    if (curve < 0) {
      curva (FRONT_L, IdleVel, -curve);
    } else {
      curva (FRONT_R, IdleVel, curve);
    }
    timer.set (CHG_DIR_TIME);
  }
}

///////////////////////////////////////////////////////////////
//                    algoritmo de empuje                    //
///////////////////////////////////////////////////////////////

void push () {
  static float pushVel;                           // velocidad del empuje
  static int phase;
  
  if (state.isChanged()) {
    pushVel = AttackVel;
    phase = 0;
  }

  if (phase < 2) {
      // reducción de RPM en función del consumo
      float loadL = motorL.getLoad();
      float loadR = motorR.getLoad();
      if (loadL>HIGH_LOAD_THRESHOLD || loadR>HIGH_LOAD_THRESHOLD) {
        pushVel *= .98;
      }
      else if (loadL<LOW_LOAD_THRESHOLD || loadR<LOW_LOAD_THRESHOLD) {
        pushVel *= 1.02;
      }
      pushVel = constrain (pushVel, PushLowVel, AttackVel);
      adelante (pushVel);
  }

  // rutina de "escape"
  switch (phase) {
    case 0:
      if (pushVel == PushLowVel) {
        phase = 1;
        timer.set (PUSH_TIMEOUT);
      }
      break;
    case 1:
      if (pushVel > PushLowVel) phase = 0;
      else if (timer.check()) {
        if (sonarDer.getDist() > sonarIzq.getDist()) {
          curva (BACK_R, AttackVel, 0.5);
        } else {
          curva (BACK_L, AttackVel, 0.5);
        }
        phase = 2;
        timer.set (PUSH_ESC_TIME);
      }
      break;
    case 2: 
      if (timer.check())
        state = BUSCAR;      // el timer queda 'fired'
        break;
  }
}

