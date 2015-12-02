enum State {Send, Receive};
volatile State g_state = Receive;

#define g_bufferSize 8
unsigned char g_inputBuffer[g_bufferSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

volatile bool g_inByteReady = false;
volatile bool g_tooSlow = false;
volatile unsigned char g_inByte = 0xff;
volatile unsigned short g_bitsRead = 0;
volatile unsigned char g_inCurrentByte = 0xff;

inline void sync_comm () {
    if (readPin (BUSY_PIN))
    {
      g_inCurrentByte = 0;
      g_bitsRead = 0;
    }
}

inline void sendByteRaw (const unsigned char b) {
    for (int bitPos = 7; bitPos >= 0; bitPos--) {
        bool val = b & (1 << bitPos);
        if (val) {
            writePin (DATA_PIN, 1);
        } else {
            writePin (DATA_PIN, 0);
        }

        TXLED1;
        while (readPin (CLK_PIN)) {/*wait*/};
        while (!readPin (CLK_PIN)) {/*wait*/};
        TXLED0;
    }
}

void sendBuffer (unsigned char* buf, int len, int offset = 0) {
    return;
    intFastOff;
    ioCfgData (false); //output mode
    
    for (int bytePos = offset; bytePos < len; bytePos++) {
        sendByteRaw (buf[bytePos]);
    }
    
    ioCfgData (true);
    intFastOn;

    Serial.print ("SENT BUFFER: ");
    for (int bytePos = offset; bytePos < len-1; bytePos++) {
        Serial.print (buf[bytePos]);
    }
    Serial.println (buf[len-1]);
}

void sendByte (const unsigned char b) {
    intFastOff;
    ioCfgData (false); //output mode
    
    sendByteRaw (b);
    
    ioCfgData (true);
    intFastOn;
     
    Serial.print ("SENT: ");
    Serial.println (b, HEX);
}

inline void sendAck () {
    return;
    sendByte (0x00);
}

void isr_clock () {
    if (g_state == Receive) {
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
}

void comm_signal () {
    intFastOff;
           
    ioCfgBusy (false); //output
    writePin (BUSY_PIN, 0); //low
    delay(1000);
    writePin (BUSY_PIN, 1); //high
    ioCfgBusy (true); //input
    
    intFastOn;
}

void setup_comm () {
    for (int i = 0; i < g_bufferSize; i++) {
        g_inputBuffer[i] = 0x0;
    }

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
