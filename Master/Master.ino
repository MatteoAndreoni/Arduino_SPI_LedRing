#include <SPI.h>

/**
 * @file Master_LED.ino
 * 
 * @brief Master che invia a Slave i comandi per l'effetto di luce da visualizzare.
 * 
 * @author Matteo Andreoni
 * @author Filippo Nevi 
 */

/// Pin usato dall'utente per richiedere l'accesso alla scrivania.
#define U_INPUT 5

/// Flag usata per sapere se lo Slave è già stato allertato il ciclo prima.
bool flagInput = LOW;

/// Flag usata per tenere traccia della configurazione dello Slave.
bool handshake = LOW;

/**
 * @brief Funzione che effettua la trasmissione del byte passato come parametro, effettua un delay e ritorna il nuovo valore ricevuto dal dispositivo Slave.
 * 
 * @param value Valore che verrà inviato allo Slave.
 */
byte transferWithDelay(const byte value) {
  byte rx = SPI.transfer(value);
  delay(250);
  return rx;
}

/// Variabile utilizzata per tenere traccia del tempo della sessione.
uint32_t timestamp;

/**
 * @brief Funzione di setup della scheda. Inizializza SPI e il pin 5 per poter ricevere l'input dall'esterno.
 */
void setup() {
  SPI.begin();
  Serial.begin(9600);
     
  pinMode(U_INPUT, INPUT);

  timestamp = 0;
}

/**
 * @brief Funzione di loop. Ogni volta che la funzione viene eseguita e il pin 5 riceve alimentazione, il Master chiede allo Slave la temperatura.
 */
void loop() {
  if(!handshake) {
    if(flagInput == LOW) {
      digitalWrite(SS, LOW);  // Allerta lo slave
      Serial.println("Ho messo SS a LOW");
      delay(50);
      flagInput = HIGH;
    }
    
    delay(50);
    byte slaveHandshake = SPI.transfer(1); //transferWithDelay(1);
    
    if(slaveHandshake == 1) {
      Serial.println("Handshake avvenuto con successo");
      handshake = HIGH;
    }
    
    digitalWrite(SS, HIGH);
    flagInput = LOW;
  }

  
  if(digitalRead(U_INPUT) && handshake == HIGH) {   //bottone premuto e handshake avvenuto con successo
    Serial.println("Bottone premuto");
    if(timestamp == 0) {
      if(flagInput == LOW) {
        digitalWrite(SS, LOW);  // Allerta lo slave
        flagInput = HIGH;
      }
      delay(2000);    //attesa
      byte rx = transferWithDelay(4);   // tentativo di prenotazione della scrivania
      Serial.println("Ho inviato il byte con valore 4 e ho ricevuto " + String(rx));
      if(rx == 2) {
        Serial.println("Ho ricevuto 2 (lo slave è nel case 4 dello switch-case)");
        byte firstByte = transferWithDelay(1);    // primo byte per i minuti della sessione
  
        if(firstByte == 4) {
          Serial.println("Ho ricevuto 4 (lo slave ha ricevuto il primo byte del tempo della sessione)");
          byte secondByte = transferWithDelay(0);   // secondo byte per i minuti della sessione
          if( secondByte == 4){                  
            Serial.println("Ho ricevuto 4 (lo slave ha ricevuto il secondo byte del tempo della sessione)");
            Serial.println("Prenotazione avvenuta con successo, ho ricevuto i byte " + String (firstByte) + " e " + String (secondByte));
            
          }
        }
        

  
        timestamp = millis();
      }
      else Serial.println("Prenotazione NON avvenuta con successo");

      delay(250);
    }
    else {
      if(flagInput == LOW) {
        digitalWrite(SS, LOW);  // Allerta lo slave
        flagInput = HIGH;
      }

      byte rx = SPI.transfer(5);

      /*if(rx != 2)
        Serial.println("Errore 1: è stato ricevuto un valore sbagliato dallo Slave.");
        */
    }
  }
  else {
    digitalWrite(SS, HIGH);
    flagInput = LOW;
  }

  if((timestamp != 0) && ((millis() - timestamp) >= 60000 + 7000)){           // Se sono passati sessanta secondi da quando ho inviato il comando allo slave, è finita la sessione
    if(flagInput == LOW) {
      digitalWrite(SS, LOW);  // Allerta lo slave
      flagInput = HIGH;
    }
    
    byte rx = SPI.transfer(3);
    Serial.println("Ho ricevuto " + String(rx));
    timestamp = 0;

    if(rx != 2)
      Serial.println("Errore 2: è stato ricevuto un valore sbagliato dallo Slave.");

    digitalWrite(SS, HIGH);
    flagInput = LOW;
  }

  delay(500);
}

