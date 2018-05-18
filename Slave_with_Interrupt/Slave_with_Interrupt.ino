#include "LEDApi.h"

LEDApi led;
bool handshake = false;
bool handshakeDone = false;
bool commandReceived = false;
bool hoursFlag = false;
bool minutesFlag = false;
byte commandFromMaster;
byte hours;
byte minutes;
float sessionTime;
byte sessionByte = false;



void setup (void)
{
  Serial.begin(9600);
  pinMode(MISO, OUTPUT);
  SPCR |= bit(SPE);
  SPCR |= bit(SPIE);
  led.initialize();
  Serial.println("started");
}

ISR (SPI_STC_vect)
{
  byte c = SPDR;
  /*if (c == 0)
    return;
  */

  if (c == 1 && !sessionByte)  // starting new sequence?
  {
    handshake = true;
    SPDR = c;   // send first byte
    Serial.println("Sent 1 for hanshake");
    return;
  }

  if (c == 2 && !sessionByte)  // starting new sequence?
  {
    commandFromMaster = c;
    commandReceived = true;
    // SPDR = 1;   // send first byte
    return;
  }

  if (c == 3  && !sessionByte)
  {
    commandFromMaster = c;
    hoursFlag = true;
    sessionByte = true;
    // SPDR = 1;
    return;
  }

  if (hoursFlag)
  {
    hoursFlag = false;
    hours = c;
    minutesFlag = true;
    // SPDR = 1;
    return;
  }

  if (minutesFlag)
  {
    minutesFlag = false;
    minutes = c;
    // SPDR = 1;
    commandReceived = true;
    sessionByte = false;
    return;
  }

  if (c == 4  && !sessionByte) {
    if (led.sessionStillActive == true) {
      led.ledAccessDeniedEffect = true;
    }
    else {
      commandFromMaster = c;
      commandReceived = true;
      //   SPDR = 1;
      return;
    }
  }
}

void loop (void)
{
  if (handshake && !handshakeDone) {
    handshakeDone = true;
    Serial.println("Handshake done");
    led.ledDefaultState();
    led.ledSetFree();
  }
  else {

    if (commandReceived) {
      commandReceived = false;
      Serial.println("comando ricevuto dal master: " + String(commandFromMaster));


      switch (commandFromMaster) {
        case 2:
          led.ledSetFree();
          break;
        case 3:
          sessionTime = hours * 60.0 + minutes;
          Serial.println("ore: " + String(hours));
          Serial.println("minuti: " + String(minutes));
          led.ledSetBooked(sessionTime);
          break;
        case 4:
          led.ledSetDenied();
          led.ledSetFree();
          break;
      }
    }
  }
}
