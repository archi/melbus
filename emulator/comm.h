/**
 *  archi/melbus - A melbus node for CD/MD emulation in swedish cars, for Arduino
 *  See https://github.com/archi/melbus
 *   Copyright (C) 2015-2016 Sebastian Meyer
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

enum State {PreSend, Send, Receive};
volatile State g_state = Receive;

#define g_bufferSize 10
unsigned char g_inputBuffer[g_bufferSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char g_outputBuffer[g_bufferSize] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned short g_outputBufferLength = 0;
unsigned short g_outputBufferPos = 0;
signed short g_outputBitPos = 0;
unsigned char g_outputCurrentByte = 0xff;


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

inline void sendByteRaw (const unsigned char b, bool signal = false) {
    for (int bitPos = 7; bitPos >= 0; bitPos--) {
        bool val = b & (1 << bitPos);
        if (val) {
            writeData (1);
        } else {
            writeData (0);
        }

        if (signal) {
            ioCfgBusy (false);
            writePin (BUSY_PIN, 1);
        }
        
        while (readPin (CLK_PIN))  {
            blinkRX(Fast);
        }
        while (!readPin (CLK_PIN))
        {
            blinkTX(Fast);
        }

        if (signal)
            ioCfgBusy (true);
    }
}

inline void sendByteRawClkd (const unsigned char b) {
    for (int bitPos = 7; bitPos >= 0; bitPos--) {
        writePin (CLK_PIN, 0);
        delayMicroseconds(8);
        
        bool val = b & (1 << bitPos);
        if (val) {
            writeData (1);
        } else {
            writeData (0);
        }
        
        writePin (CLK_PIN, 1);
        delayMicroseconds(8);
    }
}

void sendBuffer (unsigned char* buf, int len) {
    return;
    //copy the buffer into our output buffer:
    for (int i = 0; i < len; i++) {
        g_outputBuffer[i] = buf[i];
    }

    g_outputBufferPos = 0;
    g_outputBufferLength = len;
    g_outputBitPos = 0;
    g_state = PreSend;

#ifdef ENABLE_SERIAL_
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
    Serial.print ("> ");
    Serial.println (b, HEX);
#endif
}

inline void sendAck () {
    return;
    sendByte (0x00);
}

void isr_clock () {
    unsigned char b = 0x0;

    if (g_state == PreSend) {
        ioCfgData (false);
        g_state = Send;
    }

    //load the next byte if necessary
    if (g_state == Send && g_outputBitPos == 0) {
        signed short pos = g_outputBufferPos++;
        //whoops, we sent everything -> back to receive
        if (pos >= g_outputBufferLength) {
            ioCfgData (true);
            g_state = Receive;
        } else {
            g_outputCurrentByte = g_outputBuffer[pos];
            g_outputBitPos = 8;
            Serial.print (":");
            Serial.println (g_outputCurrentByte, HEX);
        }
    }

    if (g_state == Send) {
        g_outputBitPos--;
        bool val = g_outputCurrentByte & (1 << g_outputBitPos);
        delayMicroseconds (4);
        if (val) {
            writeData (1); //should be 1
        } else {
            writeData (0); //should be 0
        }

        return;
    }

    if (g_state == Receive) {
        int bitsRead = g_bitsRead;
        if (bitsRead > 0)
            b = g_inCurrentByte;

        b <<= 1;
        if (readData ()) {
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

