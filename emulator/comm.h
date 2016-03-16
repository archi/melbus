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
        
        if (readPin (CLK_PIN)) {
            while (readPin (CLK_PIN))  {
                blinkTX(Fast);
            }
        }
        while (!readPin (CLK_PIN))
        {
            blinkTX(Fast);
        }
    }
}

inline void sendByteRawClkd (const unsigned char b) {
    for (int bitPos = 7; bitPos >= 0; bitPos--) {
        writePin (CLK_PIN, 0);
        delayMicroseconds(8);
        
        bool val = b & (1 << bitPos);
        if (val) {
            writePin (DATA_PIN, 1);
        } else {
            writePin (DATA_PIN, 0);
        }
        
        writePin (CLK_PIN, 1);
        delayMicroseconds(8);
    }
}

void sendBuffer (unsigned char* buf, int len) {
    unsigned char b;

    //output mode
    intFastOff;
    ioCfgData (false);  
    ioCfgClock (false); 
    ioCfgBusy (true);
    
    for (int bytePos = 0; bytePos < len; bytePos++) {
        b = buf[bytePos];
        sendByteRawClkd (b);
    }
   
    //input mode
    ioCfgBusy (false);
    ioCfgClock (true);
    ioCfgData (true);
    intFastOn;

    blinkClear ();

#ifdef ENABLE_SERIAL
    Serial.print ("> ");
    for (int bytePos = 0; bytePos < len - 1; bytePos++) {
        Serial.print (buf[bytePos], HEX);
        Serial.print (" ");
    }
    Serial.println (buf[len-1], HEX);
#endif
}

void sendByte (const unsigned char b) {
    intFastOff;
    ioCfgData (false); //output mode
    
    sendByteRaw (b);
    
    ioCfgData (true);
    intFastOn;

    blinkClear ();

#ifdef ENABLE_SERIAL
    Serial.print ("SENT: ");
    Serial.println (b, HEX);
#endif
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

//this is experimental:
//#define SMART_BUSY

void comm_signal () {
#ifdef SMART_BUSY
    intFastOff;

    /**
     * wait for BUSY to go high [=idle], so we don't jam the line
     */
    while (readPin (BUSY_PIN) && !g_inByteReady) {
        blinkAsync (Fast);
    }

    //Idea: busy line got free; but if we got a byte, maybe no need to signal presence?
    //if this is enabled, intFastOff needs to be moved 
    if (g_inByteReady) {
        blinkClear ();
#ifdef ENABLE_SERIAL
        Serial.println ("presence noted?");
#endif
        return;
    }
#else
    /**
     * wait for BUSY to go high [=idle], so we don't jam the line
     */
    while (readPin (BUSY_PIN)) {
        blinkAsync (Fast);
    }
    
    intFastOff;
#endif
    
    //ok, busy is HIGH, we can pull it low now
           
    ioCfgBusy (false); //output
    writePin (BUSY_PIN, 0); //low

    //blink RX LED for about 1s
    unsigned long m = millis () + 1000;
    while (millis () < m) {
        blinkAsync (Slow);
    }
    
    writePin (BUSY_PIN, 1); //high
    ioCfgBusy (true); //input
    
    intFastOn;

    blinkClear ();
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
    ioCfgClock (true);
    attachInterrupt (CLK_INT, isr_clock, RISING);
}

