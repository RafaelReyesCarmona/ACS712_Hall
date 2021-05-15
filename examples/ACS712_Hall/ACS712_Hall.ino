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

 ACS712 sensor(A0, ACS712_05B,5.051);
 //ACS712 sensor1(A0, ACS712_05B,5.051);

 void setup(void){
   Serial.begin(57600);
   //sensor.setADC(4096);
 //  sensor.analogRef(INTERNAL4V096);      //analogReference(INTERNAL4V096);

 //  sensor.setADC(4096);
 //  analogReadResolution(11);             // Only work with analogRead().
 }

 void loop(void){
   float current_lowNoise = sensor.getCurrent_DC_LowNoise();
   Serial.print("Sensor (LowNoise)- current is ");
   Serial.print(current_lowNoise);
   Serial.println(" mA.");
   delay(250);
/*
   float current = sensor1.getCurrent_DC();
   Serial.print("Sensor (AnalogRead)- current is ");
   Serial.print(current);
   Serial.println(" mA.");
   delay(250);
 */
}
