/**
 * This will NOT build!
 *
 * Protocol Ref:
 *   http://volvo.wot.lv/wiki/doku.php?id=melbus
 */

#define PIN_CLK 2
#define PIN_DATA 3
#define PIN_BUSY 4

#define RXLED 17
 
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

void setup () {
    setup_isr ();
    pinMode (RXLED, OUTPUT); 
    digitalWrite (RXLED, LOW);
}

bool g_txLed = false;

void loop () {
    if (g_inByteReady) {

        for (int i = g_bufferSize - 1; i > 0; i--) {
            g_inputBuffer[i-1] = g_inputBuffer[i];
        }
        g_inputBuffer[g_bufferSize - 1] = g_inByte;
        g_inByteReady = false;
        handleCmd ();
    }

    if (g_tooSlow) {
        digitalWrite (RXLED, HIGH);
    }

    if (g_state == Receive && g_txLed) {
        g_txLed = false;
        TXLED0;
    } else if (!g_txLed) {
        g_txLed = true;
        TXLED1;
    }
}

