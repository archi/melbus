/**
 * Since these functions are also used in the ISR, one could make the variables here volatile
 * But imho it's okay if the blink state gets messed up for the fraction of a second.
 */
unsigned long g_rxTime = 0;
unsigned long g_txTime = 0;
bool g_rxState = false;
bool g_txState = false;

inline void _blinkRX (unsigned int delay) {
    unsigned long m = millis ();
    if (m > g_rxTime) {
        if (g_rxState) {
            g_rxState = false;
            RXLED0;
        } else {
            g_rxState = true;
            RXLED1;
        }
        g_rxTime = m + delay;
    }
}

inline void _blinkTX (unsigned int delay) {
    unsigned long m = millis ();
    if (m > g_txTime) {
        if (g_txState) {
            g_txState = false;
            TXLED0;
        } else {
            g_txState = true;
            TXLED1;
        }
        g_txTime = m + delay;
    }
}

inline void _blinkBoth (unsigned int delay, bool sync) {
    unsigned long m = millis ();
    if (m > g_rxTime) {
        if (g_rxState) {
            g_rxState = false;
            RXLED0;
            if (sync) {
                TXLED0;
            } else {
                TXLED1;
            }
        } else {
            g_rxState = true;
            RXLED1;
            if (sync) {
                TXLED1;
            } else {
                TXLED0;
            }
        }
        g_txState = sync ? g_rxState : !g_rxState;
        g_rxTime = m + delay;
    }
}

inline void blinkClear () {
    TXLED0;
    RXLED0;
    g_rxState = false;
    g_txState = false;
}

enum BlinkSpeed {
    Fast = 50,
    Normal = 100,
    Slow = 200
};

inline void blinkRX (BlinkSpeed s) {
    _blinkRX ((unsigned int)s);
}

inline void blinkTX (BlinkSpeed s) {
    _blinkTX ((unsigned int)s);
}

inline void blinkSync (BlinkSpeed s) {
    _blinkBoth ((unsigned int)s, true);
}

inline void blinkAsync (BlinkSpeed s) {
    _blinkBoth ((unsigned int)s, false);
}
