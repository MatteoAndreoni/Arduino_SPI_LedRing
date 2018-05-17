#include "LEDApi.h"

void LEDApi::initialize() {

  //Serial.begin(9600);

  ledDelay = 200;
  PIN = 6;

  strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
  SlaveInit();

  //Serial.println("Prima di begin");
  strip.begin();
  //Serial.println("Dopo begin");
  strip.show();
  //Serial.println("Dopo show");
}

void LEDApi::ledDefaultState() {
  uint16_t i, j, k;

  for (k = 0; k < 3; k++) {

    for (j = 1; j < 32; j++) {

      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(j, j, j));
      }

      strip.show();
      delay(20);
    }

    delay(10);

    if (k < 2) {
      for (j = 31; j > 1; j--) {

        for (i = 0; i < strip.numPixels(); i++)
          strip.setPixelColor(i, strip.Color(j, j, j));

        strip.show();
        delay(20);
      }
    }
  }
}

void LEDApi::ledSetFree() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 63, 0));
    strip.show();
    delay(250);
  }
}

void LEDApi::ledSetBooked(float sessionTime) {
  uint32_t i, j, k;
  sessionStillActive = true;

  // Dissolvenza da verde a spento
  for (j = 63; j > 0; j--) {

    for (i = 0; i < strip.numPixels(); i++) {

      if (j == 1)
        strip.setPixelColor(i, 0);

      else
        strip.setPixelColor(i, strip.Color(0, j, 0));

    }
    strip.show();
    delay(20);
  }


  // Dissolvenza da spento a blu
  for (j = 0; j < 63; j++) {

    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, j));
    }
    strip.show();
    delay(20);
  }

  // Effetto orologio:

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 63));
  }
  strip.show();

  float sessionTimeSeconds = sessionTime * 60;                        // Conversione da minuti a secondi
  float ledTime = sessionTimeSeconds / (float)strip.numPixels();      // Numero di secondi per ogni led
  //Serial.println("ST = " + String(sessionTimeSeconds));

  uint8_t ledCount = 0;                                               // Contatore per tenere traccia del prossimo led da spegnere

  for (float index = 0; index <= (sessionTimeSeconds * 1000); index += ledDelay) {
    if (ledAccessDeniedEffect == true) {
      ledAccessDeniedEffect = false;
      ledSetDenied();
      index += 3800;                                              // L'effetto ledSetDenied viene eseguito in 3800 millisecondi in totale
      ledCount = index / (ledTime * 1000);

      for (uint8_t y = ledCount; y < strip.numPixels(); y++)
        strip.setPixelColor(y, strip.Color(0, 0, 63));
      strip.show();
    }

    if (index >= (ledTime * (ledCount + 1) * 1000)) {                 // Controllo se il led numero ledCount deve essere spento

      Serial.println("Ho spento il led " + String(ledCount) + " al tempo " + String(index));

      for (uint8_t w = 0; w <= ledCount; w++)
        strip.setPixelColor(w, strip.Color(0, 0, 0));

      ledCount++;
      for (uint8_t w = ledCount; w < strip.numPixels(); w++)
        strip.setPixelColor(w, strip.Color(0, 0, 63));

      strip.show();
    }
    delay(ledDelay);
  }
  /*strip.setPixelColor(ledCount, 0);
    strip.show();
  */
  for (k = 0; k < 3; k++) {                                       // Per tre volte accende e spegne in dissolvenza i led, in colore giallo
    for (j = 0; j < 63; j++) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(j, j, 0));
      }
      strip.show();
      delay(10);
    }

    for (j = 63; j > 0; j--) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(j, j, 0));
      }
      strip.show();
      delay(10);
    }
  }

  for (i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, 0);

  strip.show();

  for (uint16_t i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show();
  sessionStillActive = false;
}

void LEDApi::ledSetDenied() {

  uint32_t i, j, k;

  for (k = 0; k < 3; k++) {
    for (j = 0; j < 63; j++) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(j, 0, 0));
      }
      strip.show();
      delay(10);
    }

    for (j = 63; j > 0; j--) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(j, 0, 0));
      }
      strip.show();
      delay(10);
    }

    for (i = 0; i < strip.numPixels(); i++)
      strip.setPixelColor(i, 0);

    strip.show();
  }

}

void LEDApi::SlaveInit() {
  // Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(SS, INPUT);

  // Enable SPI as slave.
  SPCR = (1 << SPE);
}

byte LEDApi::SPItransfer(byte value) {
  SPDR = value;

  while (!(SPSR & (1 << SPIF)));
  delay(20);
  return SPDR;
}
