// PIR motion detector sensor with Arduino nano in ultimate power saving mode 
//(reducing power consumption almost by 70% (4.7mA) of normal operation consumption (13mA)) 
// Current code by Electromania 24th Feb 2016 //https://youtu.be/n-Oiz76aVYs
// Thanks to:- http://playground.arduino.cc/Learning/ArduinoSleepCode  for information about sleep.h library and modes
             // http://www.kevindarrah.com/download/arduino_code/LowPowerVideo.ino 
             // for useful explanation of  "Low Power Arduino! Deep Sleep Tutorial" for bare arduino uno chip
             // http://www.atmel.com/images/atmel-8271-8-bit-avr-microcontroller-atmega48a-48pa-88a-88pa-168a-168pa-328-328p_datasheet_complete.pdf  // datasheet

#include <avr/interrupt.h>        // Library to use interrupt
#include <avr/sleep.h>            // Library for putting our arduino into sleep modes

const int PIRsensorInterrupt = 0; //interrupt 0 at arduino nano pin D2
const int LedPin = 13;            // external LED or relay connected to pin 13
   
volatile int lastPIRsensorState = 1;  // previous sensor state
volatile int PIRsensorState = 0;   // current state of the button

void wakeUpNow(){                  // Interrupt service routine or ISR  
  PIRsensorState = !lastPIRsensorState;    // we negate previous state and assign to current state
}

void setup() {
    pinMode(LedPin, OUTPUT);    // initialize pin 13 as an output pin for LED or relay etc.
  //Serial.begin(115200);     // initialize serial communication only for debugging purpose
 // Serial.println("Warming up... wait for a min...");

 // delay execution of sketch for a min, to allow PIR sensor get stabilized
 for( int i = 1; i <= 120; i++){  // LED at pin 13 blinks until PIR sensor is stabilized
    digitalWrite(LedPin, HIGH); 
    delay(100);         
    digitalWrite(LedPin, LOW); 
    delay(100); 
 }
 
 // Serial.println("Ready");     // enable only for debugging purpose
  
  pinMode(PIRsensorInterrupt, INPUT);        // define interrupt pin D2 as input to read interrupt received by PIR sensor
}

void Hibernate()         // here arduino is put to sleep/hibernation
{
 set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // lowest power consumption mode 
 //"The Power-down mode saves the register contents but freezes the Oscillator, disabling all other chip functions 
 // until the next interrupt or hardware reset."  text from ATMEGA328P datasheet

 ADCSRA &= ~(1 << 7);   // Disable ADC - don't forget to flip back after waking up if you need ADC in your application ADCSRA |= (1 << 7);  (From Kevin's sketch)
    
 sleep_enable();                       // enable the sleep mode function
 sleep_bod_disable();                  //to disable the Brown Out Detector (BOD) before going to sleep. 

 attachInterrupt(PIRsensorInterrupt,wakeUpNow, CHANGE);   // Attach interrupt at pin D2  (int 0 is at pin D2  for nano, UNO)
  // here since PIR sensor has inbuilt timer to swtich its state from OFF to ON, we are detecting its CHANGE IN STATE to control our LED/relay at pin 13. 
   // therefore, we will not need to use arduino delay timer to Set "ON time" of our LED/relay, it can be adjusted physically using potentiometer provided on PIR sensor board.
   // This further helps in using SLEEP_MODE_PWR_DOWN which is ultimate lowest power consumption mode for ATMEGA8328P chip  
   //(please note - because of onboard power regulators of arduino boards, power consumption cannot be reduced to predicted few microAmps level of bare chips. 
   //To achieve further reduction in current consumption, we will need bare ATMEGA328P chip)
 
   for (int i = 0; i < 20; i++) {
    if(i != 13)//  because the LED/Relay is connected to digital pin 13
    pinMode(i, INPUT);
  }
 
 sleep_mode();                // calls function to put arduino in sleep mode
 
 sleep_disable();            // when interrupt is received, sleep mode is disabled and program execution resumes from here
 detachInterrupt(PIRsensorInterrupt);   // we detach interrupt from pin D2, to avoid further interrupts until our ISR is finished
}


void loop() {
 interrupts();    // enable interrupts for Due and Nano V3

 if (PIRsensorState != lastPIRsensorState){

  if (PIRsensorState == 0) {
     digitalWrite(LedPin, LOW);
   //  Serial.print("Sleeping-");            // enable for debugging
   //  Serial.println(PIRsensorState);   // read status of interrupt pin
  }
  
  else {       
     digitalWrite(LedPin, HIGH); 
    // Serial.print("Awake-");    // enable for debugging
  //   Serial.println(PIRsensorState);  // read status of interrupt pin   enable for debugging
     delay(50);
      }
  }

   lastPIRsensorState = PIRsensorState;    // reset lastinterrupt state
   delay(50);
   Hibernate();   // go to sleep - calling sleeping function
}



