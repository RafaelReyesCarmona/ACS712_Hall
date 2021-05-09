/*
ACS712_Hall.ino - Arduino library for ACS Current Sensor Hall Effect- 5A, 20A
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

/**
 * Example sketch for the ACS712_Hall library.
 */

#include <ACS712_Hall.h>

ACS712 sensor(A0, ACS712_05B,5.041);

void setup(void){
  Serial.begin(57600);
  //sensor.setADC(4096);
  //sensor.analogRef(INTERNAL4V096);      //analogReference(INTERNAL4V096);

  sensor.setADC(4096);
  analogReadResolution(12);
}

void loop(void){
  double current = sensor.getCurrent_DC(32);
  Serial.print("Sensor - current is ");
  Serial.print(current);
  Serial.println(" mA.");
  delay(500);
}
