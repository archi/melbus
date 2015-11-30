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

volatile short g_inPos = 0;
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
            g_inCurrentByte <<= 1;
            g_inCurrentByte += digitalRead (PIN_DATA);
            g_inPos++;
            if (g_inPos == 8) {
                if (g_inByteReady) {
                    g_tooSlow = true;
                }
                g_inByte = g_inCurrentByte;
                g_inByteReady = true;
                g_inPos = 0;
            }
            break;

        case PreSend:
            g_state = Send;
            pinMode (PIN_DATA, OUTPUT); 
            //fall-through:
        case Send:
            if ((*g_outputPtr)[g_outputPos] && (0x1 << g_outputBit)) {
                digitalWrite (PIN_DATA, HIGH);
            } else {
                digitalWrite (PIN_DATA, LOW);
            }

            if (g_outputBit == 7) {
                g_outputBit = 0;
                const unsigned short oPos = g_outputPos + 1;
                if (oPos == g_outputSize) {
                    g_state = Receive;
                } else {
                    g_outputPos = oPos;
                    g_outputBit = 0;
                }
            } else {
                g_outputBit++;
            }
    }// end switch
}

void setup_isr () {
    pinMode (PIN_BUSY, INPUT);
    pinMode (PIN_DATA, INPUT);
    attachInterrupt (digitalPinToInterrupt (PIN_CLK), isr_clock, FALLING);
}
