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

/**
 * You can either configure everything in this file,
 *  or include one of the predefined configurations
 * Just copy your variant of config.dist.h to config.h
 */
//#include "configs/uno.h"
//#include "configs/promicro.h"

// SOFTWARE CONFIG
/**
 * Enable MDC and/or CDC.
 * Just define MDC and/or CDC
 */
#define MDC
//#define CDC

/**
 * Enable serial output
 */
//#define ENABLE_SERIAL

// HARDWARE CONFIG

/**
 * Pin Configuration:
 *  Use pins on the same port. I.e., use ATmega PINs:
 *  Pnd for Data
 *  Pnc for Clock (needs to support interrupts!)
 *  Pnb for Busy
 * 
 * Than set DDRx, PORTx and PINx to the port n out of {A,B,C,D}
 * The Pin numbers are d,c,b
 * For the clock, we also need the interrupt number and name.
 * You can look these things up in your boards pin mapping online
 * 
 * My config on a Sparkfun Pro Micro:
 *  DATA: Digital Pin 2 = PD1 (SDA/INT1)
 *   CLK: Digital Pin 3 = PD0 (OC0B/SCL/INT0)
 *  BUSY: Digital Pin 4 = PD4 (ICP1/ADC8)
 * 
 * -> Port D, DATA on 1, CLK on 0, BUSY on 4.
 * -> Clock is on INT0
 * 
 * Arduino Code was too slow, so this is done the fast way
 */
//#define DDRx DDRD
//#define PORTx PORTD
//#define PINx PIND
//#define DATA_PIN 1
//#define BUSY_PIN 4
//#define CLK_PIN 0
//#define CLK_INT 0
//#define CLK_INTx INT0

/**
 * This macro controls a LED to signal error [perma on] or init/signaling [blink]
 * For the pro micro it's predefined in the default header files,
 *  but you might want to write one for other boards
 * 
 * LED blink for 1s -> signaling presence
 * LED on -> processing was too low
 */
//#define RXLED1 digitalWrite (your_led_pin, HIGH)
//#define RXLED0 digitalWrite (your_led_pin, LOW)

/**
 * This macro controls a LED to signal communication
 * For the pro micro it's predefined in the default header files,
 *  but you might want to write one for other boards
 * 
 * LED on  -> no data to process (but we saw the master)
 * LED off -> processing data
 */
//#define TXLED1 digitalWrite (your_led_pin, HIGH)
//#define TXLED0 digitalWrite (your_led_pin, LOW)
