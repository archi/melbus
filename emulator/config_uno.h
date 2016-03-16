//make sure to include just a single config file
#ifndef CONFIGURED
#define CONFIGURED
#else
#error JUST INCLUDE A SINGLE CONFIG FILE
#endif

/**
 * My config on a Arduino Uno/ATmega 168
 *  DATA: Digital Pin 3 = PD3 (PCINT19/OC2B/INT1) OLD: PD1 (SDA/INT1)
 *   CLK: Digital Pin 2 = PD2 (PCINT18/INT0)      OLD: PD0 (OC0B/SCL/INT0)
 *  BUSY: Digital Pin 4 = PD4 (PCINT20/XCK/T0)    OLD: PD4 (ICP1/ADC8)
 */
#define DDRx DDRD
#define PORTx PORTD
#define PINx PIND
#define DATA_PIN 3
#define BUSY_PIN 4
#define CLK_PIN 2
#define CLK_INT 0
#define CLK_INTx INT0

#define TXLED0 digitalWrite (13, LOW)
#define TXLED1 digitalWrite (13, HIGH)
#define RXLED0
#define RXLED1
