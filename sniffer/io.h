 /* 
  *  CLK: Digital Pin 2 PD1 (SDA/INT1)       1/1
  * DATA: Digital Pin 3 PD0 (OC0B/SCL/INT0)  0
  * BUSY: Digital Pin 4 PD4 (ICP1/ADC8)      4
  */

#define DDRx DDRD
#define PORTx PORTD
#define PINx PIND
#define DATA_PIN 0
#define BUSY_PIN 4
#define CLK_PIN 1
#define CLK_INT 1

#define SET1(_REG_, _BIT_) _REG_ |= (0x1 << _BIT_)
#define SET0(_REG_, _BIT_) _REG_ &= ~(0x1 << _BIT_)

#define readPin(_X_) (PINx & (0x1 << _X_))
#define writePin(_X_, _Y_) { if (_Y_) { SET1 (PORTx, _X_); } else { SET0 (PORTx, _X_);} }

void ioCfgData (bool input) {
    if (input) {
        SET0 (DDRx, DATA_PIN);  // input
        SET1 (PORTx, DATA_PIN); // enable pull up
    } else {
        SET1 (DDRx, DATA_PIN);  // output
        SET1 (PORTx, DATA_PIN); // default to HIGH
    }
}

void ioCfgBusy (bool input) {
    if (input) {
        SET0 (DDRx, BUSY_PIN);  // input
        SET1 (PORTx, BUSY_PIN); // disable pull up
    } else {
        SET1 (DDRx, BUSY_PIN);  // output
        SET1 (PORTx, BUSY_PIN); // default to HIGH
    }
}

void ioCfgClock () {
    SET0 (DDRx, CLK_PIN); //input
    SET1 (PORTx, CLK_PIN); //pull up
}