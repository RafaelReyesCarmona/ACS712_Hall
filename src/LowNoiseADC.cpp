/*This example sketch shows how to make ADC measurements via registers and how to use the low noise or noise cancellation ADC measurement capability. 
This was shown in an ADC tutorial on the ForceTronics YouTube Channel.
This code is free and open for anybody to use at their own risk. 1/9/15
*/
#include <avr/sleep.h>

int normADC[10]; //Create an arrray to hold the "normal" or non-low noise ADC measurements
int lowNoiseADC[10]; //Create an array to hold the low noise ADC measurements

int const aVCC = 0b01000000; //variable to set ADC to use VCC as reference
int const iREF = 0b11000000; //variable to set ADC to use internal 1.1V as reference
int const aREF = 0b00000000; //variable to set ADC to use VCC as reference
//bits 7 and 6 select the ADC reference source
//The four zeros (bits 3 thru 0) at the end of the above binary register values sets the analog channel to A0
//Bit 5 selects right adjust for the result and bit 4 is not used

//Setup code only run once
void setup() {
  ADMUX = aVCC; //Configure the ADC via the ADMUX register for VCC as reference, result right adjust, and source A0 pin
  ADCSRA |= 1<<ADEN; //Turn the ADC on by setting the ADEN bit to 1 in the ADCSRA register
  ADCSRA |= 1<<ADIE; //Setting the ADIE bit to 1 means when the ADC is done a measurement it generates an interrupt, the interrupt will wake the chip up from low noise sleep
  ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //Prescaler at 128 so we have an 125Khz clock source
  //The ADC clock must be between 50kHz and 200kHz, since the Uno clock is 16MHz we want to divide it by 128 to get 125kHz. This may need to be adjusted for other Arduino boards
  //We do not have to use registers to set the sleep mode since we are using the avr/sleep.h library
  sleep_enable(); //enable the sleep capability
  set_sleep_mode(SLEEP_MODE_ADC); //set the type of sleep mode. We are using the ADC noise reduction sleep mode
  //Note that in this sleep mode the chip automatically starts an ADC measurement once the chip enters sleep mode
  //Note that the reason for making the ADC measurements and storing in arrays was to avoid using the arduino serial functions with sleep mode since serial uses interrupts
  for(int i=0; i<10; i++) { //Loop ten times and make ten ADC measurements in low noise mode
    sei(); //enable interrupts
    sleep_cpu(); //enter low ADC noise sleep mode, this action turns off certain clocks and other modules in the chip so they do not generate noise that affects the accuracy of the ADC measurement
    //The chip remains in sleep mode until the ADC measurement is complete and executes an interrupt which wakes the chip from sleep
    lowNoiseADC[i] = ADC; //get ADC result and store it in low noise measurement array
  }
  
  for(int k=0; k<10; k++) { //loop executes 10 times and makes 10 normal ADC measurements
    ADCSRA |= 1<<ADSC; //this bit starts conversion. It will remain high until conversion is complete
    while(ADCSRA & (1<<ADSC)) { delay(1);} //When ADSC bit goes low conversion is done
    normADC[k] = ADC; //get ADC result and store it in normal ADC measurement array
  }
  
  Serial.begin(9600); //Start serial comm to print out results to serial monitor
  while(!Serial.available()) { delay(1); } //wait to print out results until serial monitor is open and any character is sent
  Serial.println("Noise reduction:"); //labels the readings being printed
  for(int j=0; j<10;j++) { //loop ten times to print out all the low noise measurement in array
    Serial.println(convertToVolt(lowNoiseADC[j]),3); //Convert each low noise measurement to a voltage level and send it over serial with three decimal places precision
  }
  Serial.println(); //print a line feed
  Serial.println("No noise reduction:"); //print heading for normal ADC measurements
  for(int l=0; l<10;l++) { //loop ten times and print each ADC reading
    Serial.println(convertToVolt(normADC[l]),3); //Convert each normal ADC measurement to a voltage level and send it over serial with three decimal places precision
  }
}

void loop() {
 //do nothing in the loop
}

//This function takes the ADC integer value and turns it into a voltage level. The input is the measured ADC value.
float convertToVolt(int aVAL) {
  float refVal = 5.01;  //hard coded in AVCC value, measured the real value for higher accuracy
  return (((float)aVAL/1023)*refVal); //formula to convert ADC value to voltage reading
}

//this is the interrupt service routine for the ADC interrupt. This must be in the code for the interrupt to work correctly
ISR(ADC_vect) {
}
