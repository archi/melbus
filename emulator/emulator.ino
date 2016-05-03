/**
 *  archi/melbus - A melbus node for CD/MD emulation in swedish cars, for Arduino
 *  See https://github.com/archi/melbus
 *   Copyright (C) 2015-2016 Sebastian Meyer
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
 *  only TX, fast: Sending data [actually: waiting for a clock cylce so the current bit was accepted]
 *  
 *  TX+RX async, fast: waiting for busy line to become free
 *  TX+RX async, normal: we are signaling presence on the busy line
 * 
 *  TX+RX sync, slow: waiting for data
 *  
 *  only RX, fast: receiving data
 *  TX+RX sync, fast: receiving data, but we were too slow at some point in time [cleared after 10s]
 */

/**
 * Your configuration goes to config.h
 * Take a look at config.dist.h for details
 */
#include "config.h"

#include "blink.h"
#include "io.h"
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

#ifdef ENABLE_SERIAL
bool g_printHex;
#endif

unsigned long g_clearSlow;
unsigned long g_initWait;
bool g_initDone;

void setup () {
#ifdef ENABLE_SERIAL
    g_printHex = false;
    Serial.begin(230400,SERIAL_8N1);
    Serial.setTimeout (5000);
   // char x;
   // Serial.readBytes (&x, 1);
    Serial.println (F("Starting up!"));
#endif
    blinkClear ();

    setup_comm ();
    
    g_initDone = false;
    g_initWait = millis () + 500;
}

void loop () {
    //if we did not see an init after 2s of power up, we signal the HU
    if (!g_initDone && g_initWait < millis ()) {
            comm_signal ();
            g_initWait = millis () + 1000;
    }
    
    while (g_inByteReady) {
        unsigned char in = g_inByte;
        g_inByteReady = false;
      
        for (int i = 0; i < g_bufferSize - 1; i++) {
            g_inputBuffer[i] = g_inputBuffer[i+1];
        }
        g_inputBuffer[g_bufferSize - 1] = in;

#ifdef ENABLE_SERIAL
        if (g_printHex) {
            Serial.println (in, HEX);
        }
#endif

        const Cmd c = decodeCmd ();

#ifdef ENABLE_SERIAL
        if (c != Wait) {
            printCmd (c);
        }
#endif
        
        const bool allInitiated = handleCmd (c);
        if (allInitiated || c == Init) {
            g_initDone = true;
        }

        if (g_tooSlow) {
            blinkSync (Fast);
            if (g_clearSlow == 0) {
#ifdef ENABLE_SERIAL
                Serial.println ("slow!");
#endif
                g_clearSlow = millis () + 10000;
            } else if (millis () > g_clearSlow) {
                g_tooSlow = false;
                g_clearSlow = 0;
            }
        } else {
            blinkRX (Fast);
        }
    }

    sync_comm ();

    blinkSync (Slow);

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
