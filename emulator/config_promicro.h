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

//make sure to include just a single config file
#ifndef CONFIGURED
#define CONFIGURED
#else
#error JUST INCLUDE A SINGLE CONFIG FILE
#endif

/**
 * My config on a Sparkfun Pro Micro:
 *  DATA: Digital Pin 2 = PD1 (SDA/INT1)
 *   CLK: Digital Pin 3 = PD0 (OC0B/SCL/INT0)
 *  BUSY: Digital Pin 4 = PD4 (ICP1/ADC8)
 */
#define DDRx DDRD
#define PORTx PORTD
#define PINx PIND
#define DATA_PIN 1
#define BUSY_PIN 4
#define CLK_PIN 0
#define CLK_INT 0
#define CLK_INTx INT0
