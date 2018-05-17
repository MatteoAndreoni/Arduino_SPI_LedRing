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



void setup (void)
{
  Serial.begin(9600);
  pinMode(MISO, OUTPUT);
  SPCR |= bit(SPE);
  SPCR |= bit(SPIE);
  led.initialize();
}

ISR (SPI_STC_vect)
{
  byte c = SPDR;

  if (c == 1)  // starting new sequence?
  {
    handshake = true;
    SPDR = 1;   // send first byte
    return;
  }

  if (c == 2)  // starting new sequence?
  {
    commandFromMaster = c;
    commandReceived = true;
    SPDR = 1;   // send first byte
    return;
  }

  if (c == 3)
  {
    commandFromMaster = c;
    hoursFlag = true;
    SPDR = 1;
    return;
  }

  if (hoursFlag)
  {
    hoursFlag = false;
    hours = c;
    minutesFlag = true;
    SPDR = 1;
    return;
  }

  if (minutesFlag)
  {
    minutesFlag = false;
    minutes = c;
    SPDR = 1;
    commandReceived = true;
    return;
  }

  if (c == 4) {
    if (led.sessionStillActive == true) {
      led.ledAccessDeniedEffect = true;
    }
    else {
      commandFromMaster = c;
      commandReceived = true;
      SPDR = 1;
      return;
    }
  }
}

void loop (void)
{
  if (handshake && !handshakeDone) {
    handshakeDone = true;
    led.ledDefaultState();
    led.ledSetFree();
  }
  else {

    if (commandReceived) {
      commandReceived = false;

      switch (commandFromMaster) {
        case 2:
          led.ledSetFree();
          break;
        case 3:
          sessionTime = hours * 60.0 + minutes;
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
