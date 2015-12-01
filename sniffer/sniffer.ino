/**
 * Sniffer serial commands:
 * 
 * r reset comm
 * 
 * s signal presence
 * c toggle communication (allow answer)
 * 
 * p toggle printing of hex values
 */

/**
 * Counter intuitive:
 *  HIGH => off
 *  LOW  => on
 */
#define RXLED 17

#include "io.h"
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

void setup () {
    pinMode (RXLED, OUTPUT);
    digitalWrite (RXLED, HIGH);
    
    Serial.begin(230400,SERIAL_8N1);
    Serial.println ("Waiting 30s for input");
    
    Serial.setTimeout (30000);
    char x;
    Serial.readBytes (&x, 1);
    Serial.println ("ok");
    
    setup_comm ();
}

void loop () {
    while (g_inByteReady) {
        unsigned char in = g_inByte;
        g_inByteReady = false;
      
        for (int i = 0; i < g_bufferSize - 1; i++) {
            g_inputBuffer[i] = g_inputBuffer[i+1];
        }
        g_inputBuffer[g_bufferSize - 1] = in;

        Serial.println (in, HEX);

        if (g_tooSlow) {
            g_tooSlow = false;
            Serial.print ("Too slow!");
        }

        Cmd c = decodeCmd ();

#ifdef SNIFFER
        printCmd (c);
#else 
        handleCmd (c);
#endif

    }

    sync_comm ();
    
    SerialEvent ();
}

bool SerialEvent () {
    if (Serial.available ()) {
        char inChar = (char)Serial.read();
        switch (inChar) {
            case 'r':
                Serial.println ("reset comm");
                detachInterrupt (CLK_INT);
                setup_comm ();
                break;

            case 's':
                Serial.println ("Signaling presence");
                noInterrupts ();
                       
                ioCfgBusy (false); //output
                writePin (BUSY_PIN, 0); //low
                delay(1000);
                writePin (BUSY_PIN, 1); //high
                ioCfgBusy (true); //input
                
                interrupts ();
                break;
                               
        } //end switch

        return true;
    }
    return false;
}

