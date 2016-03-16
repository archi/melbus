#define SET1(_REG_, _BIT_) _REG_ |= (0x1 << _BIT_)
#define SET0(_REG_, _BIT_) _REG_ &= ~(0x1 << _BIT_)

#define readPin(_X_) (PINx & (0x1 << _X_))
#define writePin(_X_, _Y_) { if (_Y_) { SET1 (PORTx, _X_); } else { SET0 (PORTx, _X_);} }

#define intFastOn  EIMSK |= (1<<CLK_INTx)
#define intFastOff EIMSK &= ~(1<<CLK_INTx)

inline void ioCfgData (bool input) {
    if (input) {
        SET0 (DDRx, DATA_PIN);  // input
        SET1 (PORTx, DATA_PIN); // enable pull up
    } else {
        SET1 (DDRx, DATA_PIN);  // output
        SET0 (PORTx, DATA_PIN); // default to HIGH
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
