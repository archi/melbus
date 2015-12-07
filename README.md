# MELBUS
Implementation of a node/emulator for MELBUS, as used in older Volvo cars.

The aim of this project is to run some code on an Arduino Pro Micro (ATmega32U4) wired up to my cars HU 603 and attach a cheap AUX connector to the radio, without any error messages.

# Status

I implemented most of the Melbus IO code. A mixed sniffer + emulator prototypes is available from `emulator/`.
The Sketch is configured for my SparkFun Arduino Pro Mico 5V/16MHz, but can easily be configured for other boards (read the beginning of the sketch)

**If you want to use this on an Arduino in your car, I recommend using the stable branch**.
That branch is still pre-alpha, but should at least not contain untested changes. That way you don't need to search for a working commit.

Most important, the stable branch should give you a version which at least supplies you with some kind of AUX input.

# Acknowledgement

Special thanks to Reinis work on the protocol: http://volvo.wot.lv/wiki/doku.php?id=melbus
He also has a more sophisticated carpc project at https://github.com/festlv/carpc
But his implementation for the AVR does "too much", I just want to plugin a 3.5mm connector (from Bluetooth adapter) as pseudo CD-Changer, for now (and learn some stuff about embedded programming).

I should also mention Karl at http://gizmosnack.blogspot.de/2015/11/aux-in-volvo-hu-xxxx-radio.html has a nice write up plus (more simple!) code for the Arduino Nano there.

# Future work

In no general order:

2. Test current state attached to a HU & see if everything works
3. Properly reply to messages (sendAck and sendBuffer seem to fail; eventually the HU expects the emulator to generate its own clock)
3. Send Text to the HU (people have done this)
3. Open up my cheap Bluetooth receiver and see if I can forward the NextTrack/PrevTrack buttons [just maybe]
4. Have the arduino become a USB keyboard, so I can attach it to my phone and forward ALL button functions to it
5. Clean switching of audio via relays [maybe]
6. Control power of a raspberry pi, which could act as a Media Player [attached to RTI input]
5. Use a Bluetooth board to stream AD2P (if that's possible with Arduino)
6. migrate parts of the code to C++ classes insteads of polluting the global namespace

# Power control plans:

Automotive power if a bit of an issue. The arduino does not drain a lot of power (I could implement a low power mode), but a 12V to 5V step down converter could drain the battery if the car is not moved for a while. **Solution**: The MELBUS run line seems to supply 12V if the HU is on. But since I don't trust the power regulators on my Pro Micro, I will add a cheap 5V/1A DC-DC converter (which can than also power my bluetooth thingy).

Once that's all up and running (and if I add an RPi), the RPi could go behind a small battery, which is in turn charged only when ignition is on. That way, there could be a easy "stay on for 20 minutes after engine is turned off" (useful at the gas station, or when only waiting for a short while).

Charging of the battery could happen with a bigger charger on the normal "ignition on" 12V line (i.e. the 5V/5A charger on Amazon for <10USD).
