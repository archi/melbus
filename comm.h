enum State {Send, Receive};
volatile State g_state = State::Receive;

#define g_bufferSize 7
volatile char g_inputBuffer[g_bufferSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
volatile char g_outputBuffer[g_bufferSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

char** volatile g_outputPtr;
volatile unsigned short g_outputPos;
volatile unsigned short g_outputSize;

void sendBuffer (char** buf, unsiged char len) {
    g_outputPtr = buf;
    g_outputLen = len;
    g_outputPos = 0;
    g_state = State::Send;
    //TODO set pins to output mode!
}

inline void sendByte (const char b) {
    g_outputBuffer[0] = b;
    sendBuffer (&g_outputBuffer, 1);
}

inline void sendAck () {
    sendByte (0x00);
}
