#include "LEDApi.h"

LEDApi led;
bool handshake = false;

void setup() {
  Serial.begin(9600);
  led.initialize();
}

void loop() {

  if(digitalRead(SS) == LOW){
    
    pinMode(MISO, OUTPUT);
    
    if(!handshake){
      byte masterReady = led.SPItransfer(1);    // slave invia 1 come conferma dell'handshake
      Serial.println("Ho ricevuto handshake");
      if(masterReady == 1){   // master vuole che lo slave si abiliti
        handshake = HIGH;
        led.ledDefaultState();    // slave si inizializza 
        led.ledSetFree();
      }
    }
  
    else{
      byte commandFromMaster = led.SPItransfer(2);  // slave invia 2 come conferma della ricezione del comando

      delay(250);

      Serial.println("Ho inviato " + String(2) + " a Master, e ho ricevuto " + String(commandFromMaster));

      switch(commandFromMaster){
        case 3:
          led.ledSetFree();
          break;
        case 4:
          byte firstMinutesByte;
          firstMinutesByte = led.SPItransfer(4);
          byte secondMinutesByte;
          secondMinutesByte = led.SPItransfer(4);
          float sessionTime;
          sessionTime = firstMinutesByte +  secondMinutesByte;   // tempo totale della sessione
          Serial.println("Primo byte = " + String(firstMinutesByte) + " , secondo byte = " + String(secondMinutesByte) + " , somma = " + String(sessionTime));
          led.ledSetBooked(sessionTime);
          break;
        case 5:
          led.ledSetDenied();
          led.ledSetFree();
          break;
      } 
    }

  }
  else {
  }
}
