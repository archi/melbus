enum State {PreSend, Send, PreReceive, Receive};
volatile State g_state = Receive;

#define g_bufferSize 24
volatile char g_inputBuffer[g_bufferSize] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile char g_outputBuffer[g_bufferSize] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile char** volatile g_outputPtr;
volatile unsigned short g_outputPos = 0;
volatile unsigned short g_outputBit = 0;
volatile unsigned short g_outputSize = 0;

volatile bool g_inByteReady = false;
volatile bool g_tooSlow = false;
volatile char g_inByte = 0xff;

volatile unsigned short g_inPos = 0;
volatile char g_inCurrentByte = 0xff;

void sendBuffer (volatile char** buf, unsigned char len, unsigned char offset = 0) {
    g_outputBit = 0;
    g_outputPtr = buf;
    g_outputSize = len;
    g_outputPos = offset;
    g_state = PreSend;
}

inline void sendByte (const char b) {
    g_outputBuffer[0] = b;
    sendBuffer ((volatile char**) &g_outputBuffer, 1);
}

inline void sendAck () {
    sendByte (0x00);
}

void isr_clock () {
    switch (g_state) {
        case PreReceive:
            g_state = Receive;
            pinMode (PIN_DATA, INPUT);
            //fall-through:
        case Receive:
            {
                char b = g_inCurrentByte;
                b <<= 1;
                b += (0x1 & digitalRead (PIN_DATA));
                g_inCurrentByte = b;

                short bitPos = g_inPos + 1;
                if (bitPos == 8) {
                    if (g_inByteReady) {
                        g_tooSlow = true;
                    }
                    g_inByte = g_inCurrentByte;
                    g_inByteReady = true;
                    bitPos = 0;
                }
                g_inPos = bitPos;
            }
            break;

        case PreSend:
            g_state = Send;
            pinMode (PIN_DATA, OUTPUT); 
            //fall-through:
        case Send:
            {
                if ((*g_outputPtr)[g_outputPos] && (0x1 << g_outputBit)) {
                    digitalWrite (PIN_DATA, HIGH);
                } else {
                    digitalWrite (PIN_DATA, LOW);
                }
                
                short bitPos = g_outputBit;
                if (bitPos == 7) {
                    bitPos = 0;
                    const unsigned short oPos = g_outputPos + 1;
                    if (oPos == g_outputSize) {
                        g_state = Receive;
                    } else {
                        g_outputPos = oPos;
                        bitPos = 0;
                    }
                } else {
                    bitPos = 0;
                }
                g_outputBit = bitPos;
            }
            break;

    }// end switch
}

void setup_comm () {
    for (int i = 0; i < g_bufferSize; i++) {
        g_inputBuffer[i] = 0xff;
        g_outputBuffer[i] = 0xf;;
    }
    
    g_outputPtr = NULL;
    g_outputPos = 0;
    g_outputBit = 0;
    g_outputSize = 0;

    g_inByteReady = false;
    g_tooSlow = false;
    g_inByte = 0xff;

    g_inPos = 0;
    g_inCurrentByte = 0xff;
    
    pinMode (PIN_DATA, INPUT);
    attachInterrupt (digitalPinToInterrupt (PIN_CLK), isr_clock, FALLING);
    
    //pull low for 1s to signal presence
    pinMode (PIN_BUSY, OUTPUT);
    digitalWrite (PIN_BUSY, LOW);

    //blink TX+RX LED for about 1s
    for (int i = 0; i < 5; i++) {
        digitalWrite (RXLED, LOW);
        delay (100);
        digitalWrite (RXLED, HIGH);
        delay (100);
    }
    
    digitalWrite (PIN_BUSY, HIGH);

    //and now as input again
    pinMode (PIN_BUSY, INPUT);
}
