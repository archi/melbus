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

#define WARN_TIME 16

bool g_txLed;
unsigned short g_slowWarn;

void setup () {
    pinMode (RXLED, OUTPUT);
    //make sure rx is off before setup
    digitalWrite (RXLED, HIGH);
    setup_comm ();
    //rx off after setup
    digitalWrite (RXLED, HIGH);
    TXLED0;
    g_txLed = false;
    g_slowWarn = 0;
}

void process () {
    for (int i = g_bufferSize - 1; i > 0; i--) {
        g_inputBuffer[i-1] = g_inputBuffer[i];
    }
    g_inputBuffer[g_bufferSize - 1] = g_inByte;
    g_inByteReady = false;
    handleCmd ();
}

void warnSlow () {
    if (g_slowWarn == 0) {
        g_slowWarn = WARN_TIME;
        //turn on RX LED
        digitalWrite (RXLED, LOW);
    } else {
        g_slowWarn--;
        if (g_slowWarn == 0) {
            g_tooSlow = 0;
            digitalWrite (RXLED, HIGH);
        }
    }
}

void loop () {
    bool inByteRead = false;
    while (g_inByteReady) {
        process ();
        inByteRead = true;
        if (g_tooSlow)
            warnSlow ();
    }

    if (inByteRead) {
        if (g_state == Receive && g_txLed) {
            g_txLed = false;
            TXLED0;
        } else if (!g_txLed) {
            g_txLed = true;
            TXLED1;
        }
    }
}

