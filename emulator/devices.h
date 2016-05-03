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

struct device_t {
    const unsigned char name[4];
    const unsigned char baseId;
    const unsigned char respId;
    bool enabled;
    bool initialized;
    unsigned char trackInfo[9];
    unsigned char cartInfo[6];
};

//init, {TRACK}, {CART}     0     1     2     3     4     5     6     7     8   ||| 9     10    11    12    13    14
#define DEV_DEFAULT false, {0x00, 0x02, 0x00, 0x01, 0x80, 0x01, 0xff, 0x60, 0x60}, {0x00, 0x0f, 0xff, 0x4a, 0xfc, 0xff }
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
struct device_t g_devices[g_devicesSize] = {
      {"MDC", 0xd8, 0xde, ENABLE_MDC, DEV_DEFAULT} //MD-C
    , {"CDC", 0xe8, 0xee, ENABLE_CDC, DEV_DEFAULT} //CD-C
};

unsigned char fakeCI[6] = {0x00, 0x0f, 0xff, 0x4a, 0xfc, 0xff };
unsigned char fakeTI[9] = {0x00, 0x02, 0x00, 0x01, 0x80, 0x01, 0xff, 0x60, 0x60};
