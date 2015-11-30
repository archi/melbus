/**
 * This will NOT build!
 *
 * Protocol Ref:
 *   http://volvo.wot.lv/wiki/doku.php?id=melbus
 */

#define PIN_CLK 2
#define PIN_DATA 3
#define PIN_BUSY 4

/**
 * Counter intuitive:
 *  HIGH => off
 *  LOW  => on
 */
#define RXLED 17
 
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

unsigned short g_lnCount;
bool g_txLed;

void setup () {
    pinMode (RXLED, OUTPUT);
    digitalWrite (RXLED, HIGH);
    
    Serial.begin(230400,SERIAL_8N1);
    Serial.println ("Setup...");
    
    //TXLED1;
    g_txLed = true;
    g_lnCount = 0;
    
    setup_comm ();
}

void loop () {
    if (g_inByteReady) {
        //TODO disabled for now (faster code)
        //for (int i = g_bufferSize - 1; i > 0; i--) {
        //    g_inputBuffer[i-1] = g_inputBuffer[i];
        //}
        //g_inputBuffer[g_bufferSize - 1] = g_inByte;
        char in = g_inByte;
        g_inByteReady = false;

        if (g_tooSlow) {
            g_tooSlow = false;
            digitalWrite (RXLED, LOW);
            Serial.println ("!!!");
        }
        
        if (g_lnCount < 7) {
            Serial.print (in, HEX);
            Serial.print (", ");
            g_lnCount++;
        } else if (g_lnCount == 8) {
            Serial.println (in, HEX);
            g_lnCount = 0;

            //Toggle the TXLED every 8 bytes received
            if (g_txLed) {
              //TXLED0;
              g_txLed = false;
            } else {
              g_txLed = true;
              //TXLED1;
            }
        }
    }
}
