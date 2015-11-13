# MELBUS
Implementation of a node for MELBUS, as used in older Volvo cars.

The aim of this project is to run some code on an Arduino Pro Micro (ATmega32U4) wired up to my cars HU 603 and attach a cheap AUX connector to the radio.

Maybe later I will try to forward the buttons to my android phone.

# Acknowledgement

Special thanks to Reinis Veips work on the protocol: http://volvo.wot.lv/wiki/doku.php?id=melbus
The current state is basically a C-style write up of what he published there.
He also has a more sophisticated carpc project at https://github.com/festlv/carpc
But his implementation for the AVR does "too much", I just want to plugin a 3.5mm connector (from Bluetooth adapter) as pseudo CD-Changer.

# Future work

0. Implement communication code (via ISR)
1. figure out how that's actually done
2. I need to get real hardware to test the code (I don't have an Arduino -- d'oh)
3. Open up the Bluetooth receiver and see if I can forward the NextTrack/PrevTrack buttons
4. Have the arduino become a USB keyboard, so I can attach it to my phone and forward ALL button functions to it
