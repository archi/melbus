volatile Cmd g_currentCmd = Cmd::Wait;
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
            //cmd matches, check if for us
            char devId = g_inputBuffer[g_bufferSize - lCmd - 1]
            for (int i = 0; i < g_devicesSize; i++) {
                if (!g_devices[i].initialized)
                    continue;
                
                char id = g_devices[i].baseid;
                if (id == devId || (id + 1) == devId)
                    return g_cmdTable[i].cmd;
            }
            
            //cmd matches, but is not for us
            return Cmd::Wait;
        }
    }
    
    return Cmd::Wait;
}

void handleCmd () {
    Cmd c = g_currentCmd; 
    if (c == Cmd::Wait && c != Cmd::Init)
        c = decodeCmd ();
    if (c == Cmd::Wait)
        return;
    
    g_currentCmd = c;
    
    switch (c) {
        case Cmd::Wait:
            //do nothing, just wait...
            return;
            
        case Cmd::NopAck:
            sendAck ();
            break;
            
        case Cmd::Init:    {
                bool haveUninitialized = false;
                for (int i = 0; i < g_devicesSize; i++) {
                    if (g_devices[i].enabled
                     && !g_devices[i].initialized
                     && g_devices[i].baseId == g_inByte) {
                        g_devices[i].initialized  = true;
                        sendByte (g_devices[i].respId);
                    } else if (!g_devices[i].initialized) {
                        haveUninitialized = true;
                    }
                }
                
                if (!haveUninitialized) {
                    g_currentCmd = Cmd::Wait;
                }
                return;
            }
            break;
            
        case Cmd::TrackInfo:
            sendBuffer (g_dev->info, 9);
            break;
            
        case Cmd::CartInfo:
            sendBuffer (&(g_dev->info[9]), 6);
            break;
            
        case Cmd::NextTrack: {
            char n = g_dev->info[5]+1;
            if (n > 0x99) n = 0x99;
            if ((n & 0x0f) > 0x09) n += 6;
            g_dev->info[5] = n;
            break;
        }
        
        case Cmd::PrevTrack: {
            char n = g_dev->info[5]-1;
            if (n == 0x00) n = 0x01;
            if ((n & 0x0f) == 0x0f) n -= 6;
            g_dev->info[5] = n;
            break;
        }
        
        case Cmd::PrevDisc:
        case Cmd::NextDisc: {
            char n = g_dev->info[3];
            if (c == Cmd::NextDisc)
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
    
    g_currentCmd = Cmd::Wait;
}
