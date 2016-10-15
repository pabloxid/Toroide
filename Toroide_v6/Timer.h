/*
    Timer.h - millis() utils
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


#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>


// clase universal para timers (con millis())
class Timer {
  private:
    unsigned long startTime;
    unsigned int periodo;
    void (*runtime)();
    void (*runtime2)(byte&);
  public:
    Timer (int periodo_, void (*runtime_)());
    Timer (int periodo_, void (*runtime_)(byte&));
    Timer (void (*runtime_)());
    Timer () {
      periodo = 0;
      runtime = NULL;
      runtime2 = NULL;
    };
    void reset ();
    void fire ();
    void set (int periodo_);
    bool check ();
    void update ();
    void update (byte&);
    
};

Timer::Timer (int periodo_, void (*runtime_)()) : periodo (periodo_), runtime (runtime_) {
  reset();
}

Timer::Timer (int periodo_, void (*runtime_)(byte&)) : periodo (periodo_), runtime2 (runtime_) {
  reset();
}

Timer::Timer (void (*runtime_)()) : periodo (0), runtime (runtime_) {
  reset();
}

void Timer::reset () {
  startTime = millis ();
}

void Timer::fire () {
  startTime = 0;
}

void Timer::set (int periodo_) {
  periodo = periodo_;
  reset();
}

bool Timer::check () {
  return (millis()-startTime > periodo);
}

void Timer::update () {
  if (periodo==0 || runtime==NULL) return;
  long now = millis ();
  if (now - startTime > periodo) {
    startTime = now;
    (*runtime) ();
  }   
}

void Timer::update (byte &param) {
  if (periodo==0 || runtime2==NULL) return;
  long now = millis ();
  if (now - startTime > periodo) {
    startTime = now;
    (*runtime2) (param);
  }   
}

#endif
