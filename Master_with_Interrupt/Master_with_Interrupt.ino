/**
 * @file Master_with_interrupt.ino
 *
 * @brief Codice Master che invia al dispositivo Slave gli effetti di luce da riprodurre
 *
 * @author Matteo Andreoni
 * @author Filippo Nevi
 */

#include <SPI.h>

/// Pin di lettura dell'input dell'utente
#define USER_INPUT 5

/// Flag per tenere traccia di handshake tra Master e Slave
bool handshakeDone = false;

/// Tempo ricavato dalla funzione millis() in cui è iniziata la sessione
uint32_t timeDeskBooked;

/// Byte rappresentante le ore di durata della sessione
uint8_t hours = 0;

/// Byte rappresentante i minuti di durata della sessione
uint8_t minutes = 1;

/// Durata totale della sessione calcolata in millisecondi
uint32_t totalSessionMilliseconds;

/// Byte in cui si viene salvata la risposta dello Slave ad alcuni comandi
byte rx;

void setup (void)
{
  Serial.begin (9600);
  pinMode (USER_INPUT, INPUT);
  SPI.begin ();
  SPI.setClockDivider (SPI_CLOCK_DIV8);
}

void loop (void)
{
  if (!handshakeDone) {
    digitalWrite (SS, LOW);
    SPI.transfer (1);
    delayMicroseconds (15);
    byte handshakeAck = SPI.transfer (0);
    delayMicroseconds (15);
    if (handshakeAck == 1) {
      handshakeDone = true;
      Serial.println ("handshake done: " + String(handshakeAck));
      digitalWrite (SS, HIGH);

    }
    else {
      digitalWrite (SS, HIGH);
    }
  }
  else {
    if (digitalRead (USER_INPUT)) {
      if (timeDeskBooked == 0) {

        digitalWrite (SS, LOW);
        SPI.transfer (3);
        delayMicroseconds (15);

        SPI.transfer (hours);
        delayMicroseconds (15);

        SPI.transfer (minutes);
        delayMicroseconds (15);


        digitalWrite (SS, HIGH);
        timeDeskBooked = millis ();
        totalSessionMilliseconds = (hours * 3600000) + (minutes * 60000);
        Serial.println ("Session time: " + String (hours) + " hour(s) and " + String (minutes) + " minute(s)");
        Serial.println ("Total seconds: " + String (totalSessionMilliseconds / 1000));
      }
      else {
        digitalWrite (SS, LOW);
        SPI.transfer (4);
        delayMicroseconds (15);
        Serial.println ("Access denied: session already started");
        digitalWrite (SS, HIGH);
      }
    }
    if ((timeDeskBooked != 0) && ((millis () - timeDeskBooked) >= totalSessionMilliseconds)) {
      digitalWrite (SS, LOW);
      SPI.transfer (2);
      delayMicroseconds (15);
      digitalWrite (SS, HIGH);
      timeDeskBooked = 0;
      Serial.println ("Session finished, now desk is free");
    }
  }
  delay (500);
}
