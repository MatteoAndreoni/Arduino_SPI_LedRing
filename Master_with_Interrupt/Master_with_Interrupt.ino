#include <SPI.h>

#define USER_INPUT 5

bool handshake = false;
uint32_t timeSessionStart = 0;
uint8_t hours = 1;
uint8_t minutes = 30;
uint32_t totalSessionMilliseconds = 0;

void setup (void)
{
  Serial.begin (115200);
  Serial.println ("Starting");
  pinMode (USER_INPUT, INPUT);
  //digitalWrite (SS, HIGH);
  SPI.begin ();
  SPI.setClockDivider (SPI_CLOCK_DIV8);
}

void loop (void)
{
  if (!handshake) {
    digitalWrite (SS, LOW);
    SPI.transfer (1);
    delayMicroseconds (15);
    byte handshakeValue = SPI.transfer (0);
    delayMicroseconds (15);
    if (handshakeValue == 1) {
      handshake = true;
      Serial.println ("Handshake done");
    }
    digitalWrite (SS, HIGH);
  }
  else {
    if (digitalRead (USER_INPUT)) {
      if (timeSessionStart == 0) {
        digitalWrite (SS, LOW);
        SPI.transfer (3);
        delayMicroseconds (15);
        SPI.transfer (hours);
        delayMicroseconds (15);
        SPI.transfer (minutes);
        delayMicroseconds (15);
        digitalWrite (SS, HIGH);
        timeSessionStart = millis ();
        totalSessionMilliseconds = hours * 60 * 60 * 1000 + minutes * 60 * 1000;
        Serial.println ("Session time: " + String (hours) + " hour(s) and " + String (minutes) + " minute(s)");
        Serial.println ("Total seconds: " + String ((totalSessionMilliseconds / 1000)));
      }
      else {
        digitalWrite (SS, LOW);
        SPI.transfer (4);
        delayMicroseconds (15);
        Serial.println ("Access denied: session already started");
        digitalWrite (SS, HIGH);
      }
    }
    if ((timeSessionStart != 0) && ((millis () - timeSessionStart) >= totalSessionMilliseconds)) {
      digitalWrite (SS, LOW);
      SPI.transfer (2);
      delayMicroseconds (15);
      digitalWrite (SS, HIGH);
      timeSessionStart = 0;
      Serial.println ("Session finished, now desk is free");
    }
  }
  delay (3000);
}
