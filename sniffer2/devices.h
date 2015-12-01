struct device_t {
    const unsigned char baseId;
    const unsigned char respId;
    bool enabled;
    bool initialized;
    volatile unsigned char info[15];
};

//init, {TRACK + CART}      0     1     2     3     4     5     6     7     8 ||| 9     10    11    12    13    14
#define DEV_DEFAULT false, {0x00, 0x02, 0x00, 0x01, 0x80, 0x01, 0xff, 0x60, 0x60, 0x00, 0x0f, 0xff, 0x4a, 0xfc, 0xff }
/*
0 ?
1 02 stop, 08 play
2 08 ? 0f rand
3 [01..0a] CDx
4 track related?
5 track related?
6 ?
7 time min
8 time sec
*/

#ifdef MDC
#define ENABLE_MDC true
#else
#define ENABLE_MDC false
#endif

#ifdef CDC
#define ENABLE_CDC true
#else
#define ENABLE_CDC false
#endif

#define g_devicesSize 2
volatile struct device_t g_devices[g_devicesSize] = {
      {0xd8, 0xde, ENABLE_MDC, DEV_DEFAULT} //MD-C
    , {0xe8, 0xee, ENABLE_CDC, DEV_DEFAULT} //CD-C
};
