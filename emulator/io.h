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

#define SET1(_REG_, _BIT_) _REG_ |= (0x1 << _BIT_)
#define SET0(_REG_, _BIT_) _REG_ &= ~(0x1 << _BIT_)

#define readPin(_X_) (PINx & (0x1 << _X_))
#define writePin(_X_, _Y_) { if (_Y_) { SET1 (PORTx, _X_); } else { SET0 (PORTx, _X_);} }

#define readData() (DATA_PINx & (0x1 << DATA_PIN))
#define writeData(_Y_) { if (_Y_) { SET1 (DATA_PORTx, DATA_PIN); } else { SET0 (DATA_PORTx, DATA_PIN); } }

#define intFastOn  EIMSK |= (1<<CLK_INTx)
#define intFastOff EIMSK &= ~(1<<CLK_INTx)

inline void ioCfgData (bool input) {
    if (input) {
        SET0 (DATA_DDRx, DATA_PIN);  // input
        SET1 (DATA_PORTx, DATA_PIN); // enable pull up
    } else {
        SET1 (DATA_DDRx, DATA_PIN);  // output
        SET0 (DATA_PORTx, DATA_PIN); // default to HIGH
    }
}

inline void ioCfgBusy (bool input) {
    if (input) {
        SET0 (DDRx, BUSY_PIN);  // input
        SET1 (PORTx, BUSY_PIN); // disable pull up
    } else {
        SET1 (DDRx, BUSY_PIN);  // output
        SET1 (PORTx, BUSY_PIN); // default to HIGH
    }
}

inline void ioCfgClock (bool input) {
    if (input) {
        SET0 (DDRx, CLK_PIN); //input
        SET1 (PORTx, CLK_PIN); //pull up
    } else {
        SET1 (DDRx, CLK_PIN); //output
        SET1 (PORTx, CLK_PIN); //default to HIGH
    }
}
