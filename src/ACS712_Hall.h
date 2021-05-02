/*
ACS712_Hall.h - Arduino library for ACS Current Sensor Hall Effect- 5A, 20A
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

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef ACS712_Hall_h
#define ACS712_Hall_h

enum ACS712_type {
	ACS712_05B = 5405,  // 0.185
	ACS712_20A = 10000, // 0.100
	ACS712_30A = 15151  // 0.066
};

class ACS712 {
    private:
			#if defined(__LGT8F__)
			  int _ADC_MAX = 4096;  //ADC max. value (4093) + 1 -> 4096. But it will be 4069 by design.
			#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
				int _ADC_MAX = 1024;  //ADC max. value (1023) + 1 -> 1024.
			#else
				int _ADC_MAX = 1024;  //ADC max. value (1023) + 1 -> 1024.
			#endif
        int _PIN;
		    ACS712_type _TYPE;
				int _OFFSET;
				float _VREF = 5.0;

				float _alphaEMA_LOW = 0.91;
				float _alphaACS712;

    public:
			  double _current = 0.0;

				ACS712(int, ACS712_type);
				ACS712(int, ACS712_type, float);

				void setADC(int);
				void setEMA(float);

				float getCurrent_DC(int numsamples = 15);
				float getCurrent_AC(int frecuency = 50);

};

#endif
