/**
 * Sniffer serial commands:
 * j shift input by one bit left
 * k shift input by one bit right
 * 
 * r reset comm
 * 
 * b invert busy signalling
 * d invert data
 * o toggle hbf/lbf
 * x status of above bits 
 * 
 * s signal presence
 * c toggle communication (allow answer)
 * 
 * p toggle printing of hex values
 */

/**
 * Counter intuitive:
 *  HIGH => off
 *  LOW  => on
 */
#define RXLED 17

#include "io.h"
#include "devices.h"
#include "cmds.h"
#include "comm.h"
#include "protocol.h"

unsigned short g_lnCount;
bool g_txLed;

void setup () {
    pinMode (RXLED, OUTPUT);
    digitalWrite (RXLED, HIGH);
    
    Serial.begin(230400,SERIAL_8N1);
    Serial.println ("Waiting 30s for input");
    
    Serial.setTimeout (30000);
    char x;
    Serial.readBytes (&x, 1);
    Serial.println ("ok");
    
    TXLED1;
    g_txLed = true;
    g_lnCount = 0;
    
    setup_comm ();
}

bool g_print = true;
bool g_answer = false;

void loop () {
    while (g_inByteReady) {
        char in = g_inByte;
        g_inByteReady = false;
      
        int i = in & 0xff;
        if (g_print) {
            Serial.print ("Data: 0x");
            
            Serial.print (i, HEX);
            Serial.print (" 0x");
            i = (~i) & 0xff;
            Serial.println (i, HEX);
        }
    }

    if (readPin (BUSY_PIN))
    {
      g_inCurrentByte = 0;
      g_inBitsRead = 0;
    }
    
    SerialEvent ();
}

bool SerialEvent () {
    if (Serial.available ()) {
        char inChar = (char)Serial.read();
        switch (inChar) {
            case 'c':
                g_answer = !g_answer;
                break;
                
            case 'p':
                g_print = !g_print;
                break; 
                
            case 'o':
                g_hbf = !g_hbf;
                Serial.println (g_hbf ? "HBF" : "LBF");
                break;
                
            case 'j':
                noInterrupts ();
                g_inBitsRead--;
                interrupts ();
                Serial.print ("->");
                Serial.println (g_inBitsRead);
                break;

            case 'k':
                noInterrupts ();
                g_inBitsRead++;
                interrupts ();
                Serial.print ("->");
                Serial.println (g_inBitsRead);
                break;
                
            case 'b':
                g_busyInv = !g_busyInv;
                Serial.println (g_busyInv ? "B INV" : "B NORM");
                break;
                
            case 'i':
                g_dataInv = !g_dataInv;
                Serial.println (g_dataInv ? "D INV" : "D NORM");
                break;

            case 'e':
                g_fallEdge = !g_fallEdge;
                Serial.println (g_fallEdge ? "FALLING" : "RISING");
                break;
              
            case 'r':
                Serial.println ("reset comm");
                detachInterrupt (CLK_INT);
                setup_comm ();
                break;

            case 'x':
                Serial.println ("Status:");
                Serial.println (g_fallEdge ? "FALLING" : "RISING");
                Serial.println (g_dataInv ? "D INV" : "D NORM");
                Serial.println (g_busyInv ? "B INV" : "B NORM");
                break;

            case 's':
                Serial.println ("Signaling presence");
                noInterrupts ();
                       
                ioCfgBusy (false); //output
                writePin (BUSY_PIN, g_busyInv ? 1 : 0); //low
                delay(1000);
                writePin (BUSY_PIN, g_busyInv ? 0 : 1); //high
                ioCfgBusy (true); //input
                
                interrupts ();
                break;
                               
        } //end switch

        return true;
    }
    return false;
}

