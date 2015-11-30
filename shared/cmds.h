enum Cmd {
    Init,
    TrackInfo,
    CartInfo,
    NextTrack,
    PrevTrack,
    NextDisc,
    PrevDisc,
    NopAck,
    Wait //pseudo CMD -> waiting for a CMD
};

struct cmd_t {
    Cmd cmd;
    unsigned char length;
    const char code[6];
};

#define g_cmdTableSize 14
const struct cmd_t g_cmdTable[g_cmdTableSize] = {
    {Init,         3, {0x07, 0x1a, 0xee}},
    {Init,         3, {0x00, 0x1c, 0xed}},
    {TrackInfo,    5, {0x1b, 0xe0, 0x01, 0x08, 0x10}}, //evtl no 0x10?
    {CartInfo,    2, {0x1e, 0xef}},
    {NextTrack,    4, {0x1b, 0x2d, 0x40, 0x01}},
    {PrevTrack,    4, {0x1b, 0x2d, 0x00, 0x01}},
    {NextDisc,    3, {0x1a, 0x50, 0x41}},
    {PrevDisc,    3, {0x1a, 0x50, 0x01}},
    {NopAck,    2, {0x19, 0x2f}},    //PowerUp
    {NopAck,    2, {0x19, 0x22}},    //PowerDown
    {NopAck,    2, {0x19, 0x29}},    //FastFwd
    {NopAck,    2, {0x19, 0x26}},    //FastRew
    {NopAck,    2, {0x19, 0x2e}},    //ScanMode
    {NopAck,    2, {0x19, 0x52}}    //RndMode
};
