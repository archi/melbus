/**
 * This will NOT build!
 *
 * Protocol Ref:
 *   http://volvo.wot.lv/wiki/doku.php?id=melbus
 */

#define PIN_CLK 2
#define PIN_DATA 3
#define PIN_BUSY 4
 
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

void setup () {
    Serial.begin(115200,SERIAL_8N1);
    pinMode (PIN_BUSY, INPUT);
    pinMode (PIN_DATA, INPUT);
    attachInterrupt (digitalPinToInterrupt (PIN_CLK), isr_readBit, FALLING);
}

unsigned char g_lnCount;

void loop {
    if (g_inByteReady) {
        g_inByteReady = false;
        
        if (g_lnCount < 7) {
            Serial.print (g_inByte, HEX);
            Serial.print (", ");
            g_lnCount++;
        } else if (g_lnCount == 8) {
            Serial.println (g_inByte, HEX);
            g_lnCount = 0;
        }

        for (int i = g_bufferSize - 1; g > 0; g--) {
            g_inputBuffer[i-1] = g_inputBuffer[i];
        }

        g_inputBuffer[g_bufferSize - 1] = g_inByte;
        //TODO: once we can send, we should handle any cmds!
        //handleCmd ();
    }
}
