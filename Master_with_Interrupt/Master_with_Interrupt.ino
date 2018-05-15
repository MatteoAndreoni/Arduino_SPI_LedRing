// Master


#include <SPI.h>

bool handshake = false;



void setup (void)
{
  Serial.begin (115200);
  Serial.println ("Starting");

  digitalWrite(SS, HIGH);

  SPI.begin ();

  SPI.setClockDivider(SPI_CLOCK_DIV8);

}

void loop (void)
{
  if(!handshake){
  char buf [20];

  digitalWrite(SS, LOW);    
  SPI.transfer ('1');
  for (int pos = 0; pos < 3; pos++)
    {
    delayMicroseconds (15);
    buf [pos] = SPI.transfer ('2');
    if (buf [pos] == 0)
      {
      break;
      }
    }

  digitalWrite(SS, HIGH);
  handshake = true;
  Serial.println ("Handshake avvenuto");

  }

  

  delay (1000);
}
