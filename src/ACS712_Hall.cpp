/*
ACS712_Hall.cpp - Arduino library for ACS Current Sensor Hall Effect- 5A, 20A
and 30A models.
v0.1

Copyright © 2021 Francisco Rafael Reyes Carmona.
All rights reserved.

rafael.reyes.carmona@gmail.com


  This file is part of ACS712_Hall Library.

  ACS712_Hall Library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ACS712_Hall Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ACS712_Hall Library.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "ACS712_Hall.h"
#include <math.h>

ACS712::ACS712(int PIN, ACS712_type TYPE){
  _PIN = PIN;
  _TYPE = TYPE;
  _alphaACS712 = (_VREF * _TYPE) / (_ADC_MAX * 1000);
  _OFFSET = (_ADC_MAX >> 1);

  pinMode(_PIN, INPUT);
  _current = (analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


ACS712::ACS712(int PIN, ACS712_type TYPE, float VREF){
  _PIN = PIN;
  _TYPE = TYPE;
  _VREF = VREF;
  _alphaACS712 = (_VREF * _TYPE) / (_ADC_MAX * 1000);
  _OFFSET = (_ADC_MAX >> 1);

  pinMode(_PIN, INPUT);
  _current = (analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


void ACS712::setADC(int ADC_MAX){
  _ADC_MAX = ADC_MAX;
  _alphaACS712 = (_VREF * _TYPE) / (_ADC_MAX * 1000);
  _OFFSET = (_ADC_MAX >> 1);
  _current = (analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


void ACS712::setEMA(float EMA){
  _alphaEMA_LOW = EMA;
}


float ACS712::getCurrent_DC(int numsamples){
  float EMA_LOW = analogRead(_PIN);
  int microdelay;

  microdelay = (1 <<((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)));
  microdelay = microdelay * 2000000 / F_CPU;

  for (byte i = numsamples; i--; ){
    delayMicroseconds(microdelay);
    EMA_LOW = (_alphaEMA_LOW * (float)analogRead(_PIN)) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
  }
  
  current = (EMA_LOW - _OFFSET) * _alphaACS712;
  return (_current = (_alphaEMA_LOW * current) + ((1.0 - _alphaEMA_LOW) * _current))
}


float ACS712::getCurrent_AC(int frecuency){

}
