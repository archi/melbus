/**
 * This will NOT build!
 *
 * Protocol Ref:
 *   http://volvo.wot.lv/wiki/doku.php?id=melbus
 */

#define PIN_CLK 2
#define PIN_DATA 3
#define PIN_BUSY 4
 
#include "../shared/devices.h"
#include "../shared/cmds.h"
#include "../shared/comm.h"
#include "../shared/protocol.h"

void setup () {
    Serial.begin(115200,SERIAL_8N1);
    Serial.println ("Setup...");
    pinMode (PIN_BUSY, INPUT);
    pinMode (PIN_DATA, INPUT);
    attachInterrupt (digitalPinToInterrupt (PIN_CLK), isr_readBit, FALLING);
}

unsigned char g_lnCount;

void loop () {
    if (g_inByteReady) {

        for (int i = g_bufferSize - 1; i > 0; i--) {
            g_inputBuffer[i-1] = g_inputBuffer[i];
        }
        g_inputBuffer[g_bufferSize - 1] = g_inByte;
        g_inByteReady = false;

        if (g_tooSlow) {
            g_tooSlow = false;
            Serial.println ("\nProcessing is too slow!");
        }
        
        if (g_lnCount < 7) {
            Serial.print (g_inByte, HEX);
            Serial.print (", ");
            g_lnCount++;
        } else if (g_lnCount == 8) {
            Serial.println (g_inByte, HEX);
            g_lnCount = 0;
        }
    }
}
