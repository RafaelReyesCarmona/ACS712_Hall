/*
ACS712_Hall.cpp - Arduino library for ACS Current Sensor Hall Effect- 5A, 20A
and 30A models.
v0.3.1

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
#include <avr/sleep.h>
#include <EMA.h>
#include <ADC.h>

ACS712::ACS712(int PIN, ACS712_type TYPE){
  _PIN = PIN;
  _SENS = ACS712_sens[TYPE];

  _alphaACS712 = (_VREF * _SENS) / (float)_ADC_MAX;
  _OFFSET = (_ADC_MAX >> 1);
  _OFFSET += ACS712_slope;

  pinMode(_PIN, INPUT);

  //static EMA<3> EMA_filter(_ADC_MAX >> 1);
  //_current = (float)((int)EMA_filter(analogRead(_PIN)) - _OFFSET) * _alphaACS712;
}


ACS712::ACS712(int PIN, ACS712_type TYPE, float VREF){
  _PIN = PIN;
  _SENS = ACS712_sens[TYPE];
  _VREF = VREF;
  _alphaACS712 = (_VREF * _SENS) / (float)_ADC_MAX;
  _OFFSET = (_ADC_MAX >> 1);
  _OFFSET += ACS712_slope;

  pinMode(_PIN, INPUT);

  //static EMA<3> EMA_filter(_ADC_MAX >> 1);  
  //_current = (float)((int)EMA_filter(analogRead(_PIN)) - _OFFSET) * _alphaACS712;
}


void ACS712::setADC(int ADC_MAX){
/* First version of function --------------------------------------------
  if (ADC_MAX != _ADC_MAX) {
    int ADJ = _OFFSET - (_ADC_MAX >> 1);
    ADJ = ADJ * ADC_MAX / _ADC_MAX;
    _ADC_MAX = ADC_MAX;
    _OFFSET = (_ADC_MAX >> 1);
    _OFFSET += ADJ;
  }-----------------------------------------------------------------------
*/
  if (~((_ADC_MAX) & (ADC_MAX))){
    int a = max(_ADC_MAX,ADC_MAX);
    int b = min(_ADC_MAX,ADC_MAX);
    int i=1;
    int ADJ = _OFFSET - (_ADC_MAX >> 1);
    while (~(a >> i)&(b)) {
      i++;
    }
    (ADC_MAX > _ADC_MAX) ? _OFFSET = (ADJ << i) : _OFFSET = (ADJ >> i);
    _ADC_MAX = ADC_MAX;
    _OFFSET += (_ADC_MAX >> 1);

    _alphaACS712 = (_VREF * _SENS) / (float)_ADC_MAX;
    //_current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
  }
}


void ACS712::analogRef(uint8_t mode){
  analogReference(mode);
  float VREF = _VREF;
  switch (mode) {
    #if defined(__LGT8F__)
    case INTERNAL1V024:
      _VREF = 1.024;
      break;
    case INTERNAL2V048:
      _VREF = 2.048;
      break;
    case INTERNAL4V096:
      _VREF = 4.096;
      break;
    #endif
    case DEFAULT:
      break;
  }
  _alphaACS712 = (_VREF * _SENS) / (float)_ADC_MAX;
  int ADJ = _OFFSET - (_ADC_MAX >> 1);
  _OFFSET = ((int)(VREF * 1000) >> 1);
  _OFFSET += (int)(ADJ * VREF / _VREF);
  //_current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


float ACS712::getCurrent_DC(){
  static EMA<3> EMA_filter(_ADC_MAX >> 1);
  signed int ADC_filtered = -_OFFSET;
  ADC_filtered += EMA_filter(analogRead(_PIN));


  //Serial.println(ADC_filtered);
  //Serial.println(_OFFSET);
  //Serial.println(_alphaACS712);

  return ((float)(ADC_filtered) * _alphaACS712);
}


float ACS712::getCurrent_DC_LowNoise(){
  uint8_t PORT = _PIN - A0;
  static EMA<3> EMA_filter(_ADC_MAX >> 1);
  signed int ADC_filtered = -_OFFSET;
  uint16_t pVal;

  ADMUX &= 0xE0;  // Clear Port setting 0 MUX0..3.
  ADMUX |= PORT;  // Setting Port for read ADC.
  
  pVal = adcGet_();
  
  ADC_filtered += EMA_filter(pVal);

  //Serial.println(ADC_filtered);

  return ((float)(ADC_filtered) * _alphaACS712);
}


float ACS712::getCurrent_AC(int frecuency){
  static EMA<3> EMA_filter(_ADC_MAX >> 1);
  uint16_t ADC_filtered;
  signed int ADC_RMS;

  unsigned long period = 1000000 / frecuency;
  unsigned long t_start = micros();

  unsigned long EMA_2 = 0, measurements_count = 0;

  while (micros() - t_start < period) {
    ADC_filtered = EMA_filter(analogRead(_PIN));
    ADC_RMS = ADC_filtered - _OFFSET;
    EMA_2	+= sq(ADC_RMS);
    measurements_count++;
  }

  return (_current = sqrt((float)(EMA_2 / measurements_count)) * _alphaACS712);
}


#ifndef ISR_interrupt
#define ISR_interrupt
ISR(ADC_vect) {
}
#endif
