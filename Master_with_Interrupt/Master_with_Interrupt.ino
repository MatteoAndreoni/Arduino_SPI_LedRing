// Master

#include <SPI.h>

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
  char buf [20];

  digitalWrite(SS, LOW);    
  SPI.transfer ('1');
  for (int pos = 0; pos < 2; pos++)
    {
    delayMicroseconds (15);
    buf [pos] = SPI.transfer ('2');
    if (buf [pos] == 0)
      {
      break;
      }
    }

  digitalWrite(SS, HIGH);

  Serial.print ("Received: ");
  Serial.print (buf[0]);
  Serial.print (buf[1]);
  Serial.print (buf[2]);
  Serial.println("");

  delay (1000);
}
