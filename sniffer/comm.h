enum State {PreSend, Send, PreReceive, Receive};
volatile State g_state = Receive;

#define g_bufferSize 16
volatile unsigned char g_inputBuffer[g_bufferSize] = {
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile unsigned char g_outputBuffer[g_bufferSize] = {
//    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile unsigned char** volatile g_outputPtr;
volatile unsigned short g_outputPos = 0;
volatile unsigned short g_outputBit = 0;
volatile unsigned short g_outputSize = 0;

volatile bool g_inByteReady = false;
volatile bool g_tooSlow = false;
volatile unsigned char g_inByte = 0xff;

volatile unsigned short g_bitsRead = 0;
volatile unsigned char g_inCurrentByte = 0xff;

void sendBuffer (volatile unsigned char** buf, unsigned char len, unsigned char offset = 0) {
    g_outputBit = 0;
    g_outputPtr = buf;
    g_outputSize = len;
    g_outputPos = offset;
    g_state = PreSend;
}

inline void sendByte (const unsigned char b) {
    g_outputBuffer[0] = b;
    sendBuffer ((volatile unsigned char**) &g_outputBuffer, 1);
}

inline void sendAck () {
    sendByte (0x00);
}

inline void sync_comm () {
    if (readPin (BUSY_PIN))
    {
      g_inCurrentByte = 0;
      g_bitsRead = 0;
    }
}

void isr_clock () {
    switch (g_state) {
        case PreReceive:
            //Set to input mode and continue receiving
            g_state = Receive;
            ioCfgData (true);
            //fall-through:
        case Receive:
            {
                int bitsRead = g_bitsRead;
                unsigned char b = 0x0;
                if (bitsRead > 0)
                    b = g_inCurrentByte;

                b <<= 1;
                if (readPin (DATA_PIN)) {
                    b += 0x1;
                }
                g_inCurrentByte = b & 0xff;
                bitsRead++;

                if (bitsRead == 8) {
                    if (g_inByteReady) {
                        g_tooSlow = true;
                    }
                    g_inByte = g_inCurrentByte;
                    g_inByteReady = true;
                    bitsRead = 0;
                }
                g_bitsRead = bitsRead;
            }
            break;

        case PreSend:
            g_state = Send;
            ioCfgData (false);
            //fall-through:
        case Send:
            {
                //TODO properly adapt to sending.
                //Since we are going to send in "burst", we could do this outside of the ISR!
                if ((*g_outputPtr)[g_outputPos] && (0x1 << g_outputBit)) {
                    writePin (DATA_PIN, 1);
                } else {
                    writePin (DATA_PIN, 0);
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
    g_inByte = 0x0;

    g_bitsRead = 0;
    g_inCurrentByte = 0x0;
    
    ioCfgData (true);
    ioCfgClock ();
    attachInterrupt (CLK_INT, isr_clock, RISING);
    
    //pull low for 1s to signal presence
    ioCfgBusy (false);
    writePin (BUSY_PIN, 0);

    //blink TX+RX LED for about 1s
    for (int i = 0; i < 5; i++) {
        digitalWrite (RXLED, LOW);
        delay (100);
        digitalWrite (RXLED, HIGH);
        delay (100);
    }
    
    writePin (BUSY_PIN, 1);
    ioCfgBusy (true);    
}
