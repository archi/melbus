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
