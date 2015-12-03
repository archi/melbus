volatile Cmd g_currentCmd = Wait;
device_t* volatile g_dev;

Cmd decodeCmd () {
    for (int i = 0; i < g_cmdTableSize; i++) {
        bool match = true;
        const int length = g_cmdTable[i].length;
        for (int k = 0; match && k < length; k++) {
            const int bPos = k + g_bufferSize - length;
            if (g_cmdTable[i].code[k] != g_inputBuffer[bPos])
                match = false;
        }


        if (match) {
            Cmd cmd = g_cmdTable[i].cmd;
        
            //Init is for everyone -> early out    
            if (cmd == Init)
                return cmd;

            //cmd matches, check if it is for us
            char devId = g_inputBuffer[g_bufferSize - length - 1];
            for (int i = 0; i < g_devicesSize; i++) {
                if (!g_devices[i].initialized)
                    continue;
                
                char id = g_devices[i].baseId;
                if (id == devId || (id + 1) == devId)
                    return cmd;
            }
            
            //cmd matches, but is not for us
            return Wait;
        }
    }
    
    return Wait;
}

void handleCmd (Cmd in) {
    Cmd c = g_currentCmd; 
   
    if (c != InitWaiting) {
        c = in;
        g_currentCmd = c;
    }
    
    if (c == Wait)
        return;
    
    switch (c) {
        case Wait:
            //do nothing, just wait...
            return;
            
        case NopAck:
            sendAck ();
            break;

        case Init:
#ifdef ENABLE_SERIAL
            Serial.println ("clear init");
#endif
            for (int i = 0; i < g_devicesSize; i++) {
                g_devices[i].initialized = false;
            }
            g_currentCmd = InitWaiting;

            //no break, fall through to case InitWaiting

        case InitWaiting:
            {
                bool haveUninitialized = false;
                for (int i = 0; i < g_devicesSize; i++) {
                    if (!g_devices[i].enabled)
                        continue;
                        
                    if (!g_devices[i].initialized
                     && g_devices[i].baseId == g_inByte) {
                        g_devices[i].initialized  = true;
                        sendByte (g_devices[i].respId);
                    } else if (!g_devices[i].initialized) {
                        haveUninitialized = true;
                    }
                }
                
                if (!haveUninitialized) {
#ifdef ENABLE_SERIAL
                    Serial.println ("all init");
#endif
                    g_currentCmd = Wait;
                }

                //NO BREAK, but return:
                return;
            }
            break;
            
        case TrackInfo:
            sendBuffer (g_dev->info, 9);
            break;
            
        case CartInfo:
            sendBuffer (g_dev->info, 6, 9);
            break;
            
        case NextTrack:
            {
                unsigned char n = g_dev->info[5]+1;
                if (n > 0x99) n = 0x99;
                if ((n & 0x0f) > 0x09) n += 6;
                g_dev->info[5] = n;
                break;
            }
        
        case PrevTrack:
            {
                char n = g_dev->info[5]-1;
                if (n == 0x00) n = 0x01;
                if ((n & 0x0f) == 0x0f) n -= 6;
                g_dev->info[5] = n;
                break;
            }
        
        case PrevDisc:
        case NextDisc:
            {
                char n = g_dev->info[3];
                if (c == NextDisc)
                    n++;
                else
                    n--;
                
                if (n > 0x0a)
                    n = 0x0a;
                g_dev->info[3] = n;
                g_dev->info[5] = 0x01;
                sendAck ();
                break;
            }
        

    } // end of switch (c)
    
    g_currentCmd = Wait;
}
