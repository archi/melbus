/**
 * Sniffer serial commands:
 * 
 * r reset comm
 * 
 * s signal presence
 * c toggle communication (allow answer)
 * 
 * p toggle printing of hex values
 * 
 * LED meaning:
 *  RX: blinking for 1s -> signal presence
 *  RX: const -> processing was too slow [never cleared!]
 *  TX: on -> waiting for data
 */

/**
 * Enable MDC and/or CDC.
 * Just define MDC and/or CDC
 */
#define MDC
//#define CDC

/**
 * Pin Configuration:
 *  Use pins on the same port. I.e., use ATmega PINs:
 *  Pnd for Data
 *  Pnc for Clock (needs to support interrupts!)
 *  Pnb for Busy
 * 
 * Than set DDRx, PORTx and PINx to the port n out of {A,B,C,D}
 * The Pin numbers are d,c,b
 * For the clock, we also need the interrupt number and name.
 * You can look these things up in your boards pin mapping online
 * 
 * My config on a Sparkfun Pro Micro:
 *  DATA: Digital Pin 2 = PD1 (SDA/INT1)
 *   CLK: Digital Pin 3 = PD0 (OC0B/SCL/INT0)
 *  BUSY: Digital Pin 4 = PD4 (ICP1/ADC8)
 * 
 * -> Port D, DATA on 1, CLK on 0, BUSY on 4.
 * -> Clock is on INT0
 * 
 * Arduino Code was too slow, so this is done the fast way
 */
#define DDRx DDRD
#define PORTx PORTD
#define PINx PIND
#define DATA_PIN 1
#define BUSY_PIN 4
#define CLK_PIN 0
#define CLK_INT 0
#define CLK_INTx INT0

/**
 * Enable serial output
 */
#define ENABLE_SERIAL

/**
 * This macro controls a LED to signal error [perma on] or init/signaling [blink]
 * For the pro micro it's predefined, but you might want to write one for other boards
 * 
 * LED blink for 1s -> signaling presence
 * LED on -> processing was too low
 */
//#define RXLED1 digitalWrite (your_led_pin, HIGH)
//#define RXLED0 digitalWrite (your_led_pin, LOW)

/**
 * This macro controls a LED to signal communication
 * For the pro micro it's predefined, but you might want to write one for other boards
 * 
 * LED on  -> no data to process (but we saw the master)
 * LED off -> processing data
 */
//#define TXLED1 digitalWrite (your_led_pin, HIGH)
//#define TXLED0 digitalWrite (your_led_pin, LOW)


#include "io.h"
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

#ifdef ENABLE_SERIAL
bool g_printHex;
#endif

void setup () {
#ifdef ENABLE_SERIAL
    g_printHex = false;
    Serial.begin(230400,SERIAL_8N1);
    Serial.setTimeout (5000);
    char x;
    Serial.readBytes (&x, 1);
    Serial.println (F("Waiting for master..."));
#endif

    setup_comm ();
    unsigned long startWaiting = millis ();
    while (!g_inByteReady) {
        if (millis () > startWaiting + 1000) {
            startWaiting = 1000 + millis (); //add the delay in comm_signal!
            comm_signal ();
        }
    }
}

bool g_txLed = true;

void loop () {
    while (g_inByteReady) {
        unsigned char in = g_inByte;
        g_inByteReady = false;
      
        for (int i = 0; i < g_bufferSize - 1; i++) {
            g_inputBuffer[i] = g_inputBuffer[i+1];
        }
        g_inputBuffer[g_bufferSize - 1] = in;

        if (g_txLed) {
            TXLED0;
            g_txLed = false;
        }

#ifdef ENABLE_SERIAL
        if (g_printHex) {
            Serial.println (in, HEX);
        }
#endif

        Cmd c = decodeCmd ();

#ifdef ENABLE_SERIAL
        if (c != Wait) {
            printCmd (c);
        }
#endif
        
        handleCmd (c);

        if (g_tooSlow) {
            g_tooSlow = false;
            RXLED1;
#ifdef ENABLE_SERIAL
            Serial.println ("slow!");
#endif
        }
    }

    sync_comm ();

    if (!g_txLed) {
        TXLED1;
        g_txLed = true;
    }
#ifdef ENABLE_SERIAL
    SerialEvent ();
#endif
}

#ifdef ENABLE_SERIAL
bool SerialEvent () {
    if (Serial.available ()) {
        char inChar = (char)Serial.read();
        switch (inChar) {
            case 'h':
                g_printHex = !g_printHex;
                Serial.println (g_printHex ? F("hex on") : F("hex off"));
                break;
                
            case 'r':
                Serial.println (F("reset comm"));
                detachInterrupt (CLK_INT);
                setup_comm ();
                break;

            case 's':
                Serial.println (F("Signaling presence"));
                comm_signal ();
                break;
                               
        } //end switch

        return true;
    }
    return false;
}
#endif
