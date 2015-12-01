enum State {PreSend, Send, PreReceive, Receive};
volatile State g_state = Receive;
#define g_bufferSize 8
volatile unsigned char g_inputBuffer[g_bufferSize] = {
    //0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    //0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile unsigned char g_outputBuffer[g_bufferSize] = {
    //0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    //0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

volatile unsigned char** volatile g_outputPtr;
volatile unsigned short g_outputPos = 0;
volatile unsigned short g_outputBit = 0;
volatile unsigned short g_outputSize = 0;

volatile bool g_inByteReady = false;
volatile bool g_tooSlow = false;
volatile unsigned char g_inByte = 0x0;

volatile unsigned short g_inBitsRead = 0;
volatile unsigned char g_inCurrentByte = 0x0;

void sendBuffer (volatile unsigned char** buf, unsigned char len, unsigned char offset = 0) {
    g_outputBit = 0;
    g_outputPtr = buf;
    g_outputSize = len;
    g_outputPos = offset;
    g_state = PreSend;
}

inline void sendByte (const char b) {
    g_outputBuffer[0] = b & 0xff;
    sendBuffer ((volatile unsigned char**) &g_outputBuffer, 1);
}

inline void sendAck () {
    sendByte (0x00);
}

bool g_busyInv = false;
volatile bool g_dataInv = false;
volatile bool g_hbf = true;
bool g_fallEdge = false;

void isr_clock () {
    bool inBit = readPin (DATA_PIN);// xor g_dataInv;
    //g_inBitsRead = (g_inBitsRead + 1) % 8;
    //Serial.print (bu?"b":"B");
    //Serial.print (inBit?"1":"0");
    //Serial.println (g_inBitsRead);
    //return;
    unsigned char b = g_inCurrentByte;
    int bitsRead = g_inBitsRead;
    b <<= 1;
    if (inBit) {
        b += 0x1;
    }
    
    bitsRead++;
    g_inCurrentByte = b & 0xff;

    if (bitsRead == 8) {
        g_inByte = b;
        g_inByteReady = true;
        g_inCurrentByte = 0x0;
        bitsRead = 0x0;
    }
    g_inBitsRead = bitsRead;
}

void setup_comm () {
    for (int i = 0; i < g_bufferSize; i++) {
        g_inputBuffer[i] = 0x0;
        g_outputBuffer[i] = 0x0;
    }
    
    g_outputPtr = NULL;
    g_outputPos = 0;
    g_outputBit = 0;
    g_outputSize = 0;

    g_inByteReady = false;
    g_tooSlow = false;
    g_inByte = 0x0;

    g_inBitsRead = 0;
    g_inCurrentByte = 0x0;
    
    //pinMode (PIN_DATA, INPUT_PULLUP);
    //pinMode (PIN_CLK, INPUT_PULLUP);
    ioCfgData (true);
    ioCfgClock ();
    attachInterrupt (CLK_INT, isr_clock, g_fallEdge ? FALLING : RISING);
    interrupts ();
    //pull low for 1s to signal presence
    //pinMode (PIN_BUSY, OUTPUT);
    //digitalWrite (PIN_BUSY, g_busyInv ? LOW : HIGH);
    //delay (1000);
    //digitalWrite (PIN_BUSY, g_busyInv ? HIGH : LOW);

    //and now as input again
    //pinMode (PIN_BUSY, INPUT_PULLUP);

    ioCfgBusy (false); //output
    writePin (BUSY_PIN, g_busyInv ? 1 : 0); //low
    delay(1000);
    writePin (BUSY_PIN, g_busyInv ? 0 : 1); //high
    ioCfgBusy (true); //input
}
