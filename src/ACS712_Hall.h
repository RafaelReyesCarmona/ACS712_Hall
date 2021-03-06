/*
ACS712_Hall.h - Arduino library for ACS Current Sensor Hall Effect- 5A, 20A
and 30A models.
v0.3.2

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

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef ACS712_Hall_h
#define ACS712_Hall_h

enum ACS712_type {
	ACS712_05B = 0, // 0.185
	ACS712_20A = 1, // 0.100
	ACS712_30A = 2  // 0.066
};

static const float ACS712_sens[] PROGMEM = {
	5405.4054054,			//ACS712_05B (1/0.185)
	10000.000000,			//ACS712_20A (1/0.100)
	15151.515151			//ACS712_30A (1/0.066)
};

#ifndef ACS712_slope
#define ACS712_slope 0
#endif
class ACS712 {
    private:
			#if defined(__LGT8F__)
			  	int _ADC_MAX = 4096;  //ADC max. value (4095) + 1 -> 4096. But it will be 4069 by design.
			#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
				int _ADC_MAX = 1024;  //ADC max. value (1023) + 1 -> 1024.
			#else
				int _ADC_MAX = 1024;  //ADC max. value (1023) + 1 -> 1024.
			#endif
			int _PIN;
			float _SENS;
			int _OFFSET;
			float _VREF = 5.0f;

			float _alphaACS712;
			//#define _alphaACS712 ((_VREF) * (_SENS) / (float)(_ADC_MAX))

    public:
			//double _current = 0.0;

			ACS712(int, ACS712_type);
			ACS712(int, ACS712_type, float);

			void setVref(float Vref){_VREF = Vref;_alphaACS712 = (_VREF * _SENS) / (float)_ADC_MAX;};
			void setADC(int);
			void analogRef(uint8_t mode);

			float getCurrent_DC();
			float getCurrent_DC_LowNoise();
			float getCurrent_AC(int frecuency = 50);

};

#endif
