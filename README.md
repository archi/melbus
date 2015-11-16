# MELBUS
Implementation of a node for MELBUS, as used in older Volvo cars.

The aim of this project is to run some code on an Arduino Pro Micro (ATmega32U4) wired up to my cars HU 603 and attach a cheap AUX connector to the radio.

Maybe later I will try to forward the buttons to my android phone.

# Status

This git is just a dump of what I researched and wrote down this morning.

# Acknowledgement

Special thanks to Reinis Veips work on the protocol: http://volvo.wot.lv/wiki/doku.php?id=melbus
The current state is basically a C-style write up of what he published there.
He also has a more sophisticated carpc project at https://github.com/festlv/carpc
But his implementation for the AVR does "too much", I just want to plugin a 3.5mm connector (from Bluetooth adapter) as pseudo CD-Changer.

# Future work

0. Implement communication code (via ISR)
1. figure out how that's actually done
2. I need to get real hardware to test the code (I don't have an Arduino -- d'oh)
3. Send Text to the HU (people have done this)
3. Open up the Bluetooth receiver and see if I can forward the NextTrack/PrevTrack buttons [just maybe]
4. Have the arduino become a USB keyboard, so I can attach it to my phone and forward ALL button functions to it
5. Clean switching of audio via relays [maybe]
6. Control power of a raspberry pi, which could act as a Media Player [attached to RTI input]

# Power control plans:

Automotive power if a bit of an issue. The arduino does not drain a lot of power (I could implement a low power mode), but a 12V to 5V step down converter could drain the battery if the car is not moved for a while.
So, idea: attach a step down converter to the 12V "only on when ignition is on" power line. Attach a small 800mAh to 3000mAh buffer battery with an intelligent charger.

There are cheap, readily assembled 5V @ 5A step down converters with 4 USB ports available at Amazon.de for about 10 USD. The Battery Buffer board could be attached to that (not sure where to find a board for that, may be need my own), and power the arduino.
The arduino could in turn manage the power of an attached RPi

Bonus: The remaining USB ports could be forwarded to the passanger compartment, which would make for a nice & clean USB charger without those annoying lights. 5A should enough for Arduino + RPi + 2.5" HDD + Buffer Charger + 2 Smart Phones.

Okay, maybe I should start taking a closer look at repurposing Reinis code :)
