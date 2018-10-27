#include <EasyVR.h>

/*
Code to use the EasyVR shield to control a 120v relay.

To use this code, download the EasyVR library first.
http://www.veear.eu/downloads/

This code is based on an example-code made by Veear 

*/


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #include "SoftwareSerial.h"
  SoftwareSerial port(12,13);
#else // Arduino 0022 - use modified NewSoftSerial
  #include "WProgram.h"
  #include "NewSoftSerial.h"
  NewSoftSerial port(12,13);
#endif

#include "EasyVR.h"
#include <RemoteSwitch.h>
#define SND_BEEP  0

KaKuSwitch kaKuSwitch(11);
EasyVR easyvr(port);
EasyVRBridge bridge;
uint32_t mask = 0;
int8_t group = 0;
uint8_t train = 0;
char name[32];
boolean onestat = true;
boolean twostat = true;
boolean threestat = true;
boolean fivestat = true;
boolean sixstat = true;




void setup()
{

pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
  
  // bridge mode?
  if (bridge.check())
  {
    cli();
    bridge.loop(0, 1, 12, 13);
  }

  Serial.begin(9600);
  port.begin(9600);
  
  if (!easyvr.detect())
  {
    Serial.println("EasyVR not detected!");
    for (;;);
  }
  
  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(EasyVR::ITALIAN);
  
  int16_t count = 0;
  
  
  if (easyvr.getGroupMask(mask)) // get trained user names and passwords
  {
    uint32_t msk = mask;  
    for (group = 0; group <= EasyVR::PASSWORD; ++group, msk >>= 1)
    {
      if (!(msk & 1)) continue;
      if (group == EasyVR::TRIGGER)
        Serial.print("Trigger: ");
      else if (group == EasyVR::PASSWORD)
        Serial.print("Password: ");
      else
      {
        Serial.print("Group ");
       Serial.print(group);
        Serial.print(": ");
      }
      count = easyvr.getCommandCount(group);
      Serial.println(count);
      for (int8_t idx = 0; idx < count; ++idx)
      {
        if (easyvr.dumpCommand(group, idx, name, train))
        {
          Serial.print(idx);
          Serial.print(" = ");
          Serial.print(name);
          Serial.print(", Trained ");
          Serial.print(train, DEC);
          if (!easyvr.isConflict())
            Serial.println(" Times, OK");
          else
          {
            int8_t confl = easyvr.getWord();
            if (confl >= 0)

            else
            {
              confl = easyvr.getCommand();

            }
            Serial.println(confl);
          }
        }
      }
    }
  }
}

void loop()
{

  begin:
  int idx_cmd;  
 
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)
  
  Serial.println("Say a command in group one");  
  easyvr.recognizeCommand(1); // recognise command in group 1 
  while (!easyvr.hasFinished());
  
  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off
 
  idx_cmd = easyvr.getCommand(); 

  if (idx_cmd >= 0) 
  {    
    Serial.print("Name: ");    
    if (easyvr.dumpCommand(1, idx_cmd, name, train))
    {
      Serial.println(name);
     Serial.println(idx_cmd);
    }  
    else
      Serial.println();    
      
    if (idx_cmd == 6){
     if(onestat == 0){
    kaKuSwitch.sendSignal('M',3,2,true);
    onestat = 1;
    Serial.println("Lights turned on!"); 
     easyvr.playSound(SND_BEEP  , EasyVR::VOL_FULL);
    goto begin;
    
     }
      else{
      kaKuSwitch.sendSignal('M',3,2,false);
       onestat = 0;
       Serial.println("Lights turned off!");
        
       easyvr.playSound(SND_BEEP  , EasyVR::VOL_FULL);


       goto begin;
      }
    }
           
    
  
  else
  {
    if (easyvr.isTimeout()) 
        Serial.println("Timed out... try again!");
      
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
     Serial.print("Error ");
      Serial.println(err, HEX);          
    }
  }
}
}


