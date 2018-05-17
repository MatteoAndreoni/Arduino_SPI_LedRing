#ifndef LEDApi_h
#define LEDApi_h

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <SPI.h>

class LEDApi {
  public:

    bool sessionStillActive = false;
    bool ledAccessDeniedEffect = false;
    /**
       @brief Costruttore della classe. Inizializza la variabile di gestione dei LED e la connessione SPI.
    */
    void initialize();

    /**
       @brief Configurazione che indica lo stato di default. I LED lampeggiano di bianco.
    */
    void ledDefaultState();

    /**
       @brief Configurazione che indica la scrivania libera. I LED vengono accesi di verde uno alla volta.
    */
    void ledSetFree();

    /**
       @brief Configurazione che indica la scrivania prenotata. I LED vengono accesi di blu, e vengono spenti uno alla volta in base al tempo che scorre. Una volta finito il tempo lampeggiano di giallo.

       @param sessionTime Indica, in minuti, la durata totale della sessione corrente.
    */
    void ledSetBooked(float sessionTime);

    /**
       @brief Configurazione che indica l'accesso negato alla scrivania. I LED lampeggiano tre volte di rosso.
    */
    void ledSetDenied();

    /**
      @brief Funzione per inizializzare lo slave
    */
    void SlaveInit();


    /**
      @brief Funzione utilizzata per effettuare la trasmissione e la ricezione di un byte.

      @param value Valore da inviare al dispositivo Master.

      @return Valore ricevuto dal Master.
    */
    byte SPItransfer(byte value);

  private:
    /// Delay parziale per controllare eventuali input dal Master durante l'animazione dello scorrere del tempo.
    int ledDelay;

    /// Variabile usata per gestire i LED.
    Adafruit_NeoPixel strip;

    /// Pin a cui sono collegati i led
    int PIN;
};

#endif
