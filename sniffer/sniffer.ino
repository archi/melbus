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

bool g_print = true;
bool g_answer = false;

void loop () {
    while (g_inByteReady) {
        char in = g_inByte;
        g_inByteReady = false;
      
        for (int i = g_bufferSize - 1; i > 0; i--) {
            g_inputBuffer[i-1] = g_inputBuffer[i];
        }
        g_inputBuffer[g_bufferSize - 1] = in;

        if (g_tooSlow) {
            g_tooSlow = false;
            Serial.print ("Too slow!");
        }

        int i = in & 0xff;
        if (g_print) {
            Serial.print ("Data: 0x");
            
            Serial.print (i, HEX);
            Serial.print (" 0x");
            i = (~i) & 0xff;
            Serial.println (i, HEX);
        }

        Cmd c = decodeCmd ();
        
        if (g_answer) {
            handleCmd (c);
        }

#define PCMD(_X_) case _X_: Serial.println (#_X_); break        
        if (!g_answer && c != Wait) {
            switch (c) {
                PCMD (Init);
                PCMD (TrackInfo);
                PCMD (CartInfo);
                PCMD (NextTrack);
                PCMD (PrevTrack);
                PCMD (NextDisc);
                PCMD (PrevDisc);
                PCMD (NopAck);
                default:
                    Serial.println ("Got Wait or InitWaiting??");
                    break;
            }
        }
#undef PCMD
    }

    sync_comm ();
    
    SerialEvent ();
}

bool SerialEvent () {
    if (Serial.available ()) {
        char inChar = (char)Serial.read();
        switch (inChar) {
            case 'c':
                g_answer = !g_answer;
                break;
                
            case 'p':
                g_print = !g_print;
                break; 
              
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

