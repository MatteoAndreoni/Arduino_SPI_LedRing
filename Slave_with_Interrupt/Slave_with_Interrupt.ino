// Slave

void setup (void)
{
  Serial.begin(9600);
  pinMode(MISO, OUTPUT);
  SPCR |= bit(SPE);
  SPCR |= bit(SPIE);
}

volatile char bufin[3] = {0};
volatile char buf [] = {'4', '1', '1'};
volatile int pos;
volatile bool active;

bool is_ready = false;

ISR (SPI_STC_vect)
{
  byte c = SPDR;
  bufin[pos] = c;
  if (c == '1')  // starting new sequence?
    {
    active = true;
    pos = 0;
    SPDR = buf [pos++];   // send first byte
    return;
    }

  if (!active)
    {
    SPDR = 0;
    return;
    }

  SPDR = buf [pos];
  if (buf [pos] == 0 || ++pos >= sizeof (buf))
    active = false;

  if(pos >= 2){
    is_ready = true;
  }
}

void loop (void)
{
  if(is_ready){
    Serial.print(bufin[0]);
    Serial.print(bufin[1]);
    Serial.print(bufin[2]);
    Serial.println("");
    is_ready = false;
  }
}
