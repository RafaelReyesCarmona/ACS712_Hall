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
#include <avr/sleep.h>

ACS712::ACS712(int PIN, ACS712_type TYPE){
  _PIN = PIN;

  _SENS = ACS712_sens[TYPE];
  _alphaACS712 = (_VREF * _SENS) / (_ADC_MAX * 1000);
  _OFFSET = (_ADC_MAX >> 1);
  _OFFSET += _ADC_MAX * (ACS712_noise[TYPE]-ACS712_slope[TYPE]/(_VREF*1000)) / 1000;

  pinMode(_PIN, INPUT);
  _current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


ACS712::ACS712(int PIN, ACS712_type TYPE, float VREF){
  _PIN = PIN;
  _SENS = ACS712_sens[TYPE];
  _VREF = VREF;
  _alphaACS712 = (_VREF * _SENS) / (_ADC_MAX * 1000);
  _OFFSET = (_ADC_MAX >> 1);
  _OFFSET += _ADC_MAX * (ACS712_noise[TYPE]-ACS712_slope[TYPE]/(_VREF*1000)) / 1000;

  pinMode(_PIN, INPUT);
  _current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
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

    _alphaACS712 = (_VREF * _SENS) / (_ADC_MAX * 1000);
    _current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
  }
}


void ACS712::setEMA(float EMA){
  _alphaEMA_LOW = EMA;
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
  _alphaACS712 = (_VREF * _SENS) / (_ADC_MAX * 1000);
  int ADJ = _OFFSET - (_ADC_MAX >> 1);
  _OFFSET = ((int)(VREF * 1000) >> 1);
  _OFFSET += (int)(ADJ * VREF / _VREF);
  _current = (float)(analogRead(_PIN) - _OFFSET) * _alphaACS712;
}


float ACS712::getCurrent_DC(int numsamples){
  float EMA_LOW = (float)analogRead(_PIN);
  //int microdelay;

  //microdelay = (1 <<((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)));
  //microdelay = microdelay * 2000000 / F_CPU;

  for (byte i = numsamples; i--; ){
    _NOP();_NOP();        //delayMicroseconds(microdelay);
    EMA_LOW = (_alphaEMA_LOW * (float)analogRead(_PIN)) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
  }

//  Serial.println(EMA_LOW);
//  Serial.println(_OFFSET);

  float current = (EMA_LOW - (float)_OFFSET) * _alphaACS712;
//  Serial.println(current);
  return (_current = (_alphaEMA_LOW * current) + ((1.0 - _alphaEMA_LOW) * _current));
}


float ACS712::getCurrent_DC_LowNoise(int numsamples){
//#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
  int PORT = _PIN - A0;
  float EMA_LOW;

  ADMUX &= 0xE0;  // Clear Port setting 0 MUX0..3.
  ADMUX |= PORT;  // Setting Port for read ADC.
  ADCSRA |= _BV(ADIE); //Setting the ADIE bit to 1 means when the ADC is done a measurement it generates an interrupt, the interrupt will wake the chip up from low noise sleep
  ADCSRA |= _BV(ADEN); //Turn the ADC on by setting the ADEN bit to 1 in the ADCSRA register

  sleep_enable();
  set_sleep_mode(SLEEP_MODE_ADC);
  sei();
  sleep_cpu();

  // Gain error correction for __LGT8F__
  #if defined(__LGT8FX8E__)
    EMA_LOW = (float)(ADC - (ADC >> 5));
  #elif defined(__LGT8FX8P__)
    EMA_LOW = (float)(ADC - (ADC >> 7));
  #else
    EMA_LOW = (float)ADC;
  #endif

  for(byte i = numsamples; i--;) {
    sei(); //enable interrupts
    sleep_cpu();
    _NOP();_NOP();
    // Gain error correction for __LGT8F__
    #if defined(__LGT8FX8E__)
      EMA_LOW = (_alphaEMA_LOW * (float)(ADC - (ADC >> 5))) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
    #elif defined(__LGT8FX8P__)
      EMA_LOW = (_alphaEMA_LOW * (float)(ADC - (ADC >> 7))) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
    #else
      EMA_LOW = (_alphaEMA_LOW * (float)ADC) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
    #endif
  }
  //Serial.println(EMA_LOW);

  ADCSRA &= ~(_BV(ADIE));
  sleep_disable();

  float current = (EMA_LOW - (float)_OFFSET) * _alphaACS712;
  return (_current = (_alphaEMA_LOW * current) + ((1.0 - _alphaEMA_LOW) * _current));
//#endif
//  return getCurrent_DC(numsamples); // For __LGT8F__
}


float ACS712::getCurrent_AC(int frecuency){
  float EMA_LOW = (float)(analogRead(_PIN)-_OFFSET);
  unsigned long period = 1000000 / frecuency;
  unsigned long t_start = micros();

  unsigned long EMA_2 = 0, measurements_count = 0;

  while (micros() - t_start < period) {
    EMA_LOW = (_alphaEMA_LOW * (float)(analogRead(_PIN)-_OFFSET)) + ((1.0 - _alphaEMA_LOW) * EMA_LOW);
  	EMA_2	+= sq(EMA_LOW);
  	measurements_count++;
  }

  return (_current = sqrt(EMA_2 / measurements_count) * _alphaACS712);
}


#ifndef ISR_interrupt
#define ISR_interrupt
ISR(ADC_vect) {
}
#endif
