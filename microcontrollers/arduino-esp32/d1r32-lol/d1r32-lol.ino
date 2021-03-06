/*
 * LoL shield demo.
 *
 * LoL Shield designed by Jimmie Rodgers:
 * http://jimmieprodgers.com/kits/lolshield/
 *
 * Copyright (C) 2013-2018 Serge Vakulenko, <serge@besm6.org>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

/* Number of control pins for LoL Shield. */
#define LOL_NPINS   12

/* Number of rows on LoL Shield. */
#define LOL_NROW    9

/* Number of columns on LoL Shield. */
#define LOL_NCOL    14

/* Remap pixels to pin indexes. */
static const unsigned char charlieplexing[LOL_NROW*LOL_NCOL*2] = {
    0,8,0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,9,9,0,0,10,10,0,0,11,11,0,
    1,8,1,7,1,6,1,5,1,4,1,3,1,2,1,0,1,9,9,1,1,10,10,1,1,11,11,1,
    2,8,2,7,2,6,2,5,2,4,2,3,2,1,2,0,2,9,9,2,2,10,10,2,2,11,11,2,
    3,8,3,7,3,6,3,5,3,4,3,2,3,1,3,0,3,9,9,3,3,10,10,3,3,11,11,3,
    4,8,4,7,4,6,4,5,4,3,4,2,4,1,4,0,4,9,9,4,4,10,10,4,4,11,11,4,
    5,8,5,7,5,6,5,4,5,3,5,2,5,1,5,0,5,9,9,5,5,10,10,5,5,11,11,5,
    6,8,6,7,6,5,6,4,6,3,6,2,6,1,6,0,6,9,9,6,6,10,10,6,6,11,11,6,
    7,8,7,6,7,5,7,4,7,3,7,2,7,1,7,0,7,9,9,7,7,10,10,7,7,11,11,7,
    8,7,8,6,8,5,8,4,8,3,8,2,8,1,8,0,8,9,9,8,8,10,10,8,8,11,11,8,
};

/*
 * Pins for D1 R32 board.
 */
static const int pinmap[14] = {
/* D0 D1  D2  D3  D4  D5  D6  D7  D8  D9 D10 D11 D12 D13 */
    3, 1, 26, 25, 17, 16, 27, 14, 12, 13, 5, 23, 19, 18,
};

void all_pins_tristate()
{
    int pin;

    /* Set all pins D2...D13 as inputs. */
    for (pin = 2; pin < 14; pin++)
        pinMode(pinmap[pin], INPUT);
}

void all_pins_low()
{
    int pin;

    /* Set all pins D2...D13 low. */
    for (pin = 2; pin < 14; pin++)
        digitalWrite(pinmap[pin], LOW);
}

void set_pin_high(int row)
{
    /* Set one of pins D2...D13 as output high. */
    pinMode(pinmap[13-row], OUTPUT);
    digitalWrite(pinmap[13-row], HIGH);
}

void set_pins_low(int mask)
{
    int pin;

    /* Set some of pins D2...D13 as output low. */
    for (pin = 13; pin >= 2; pin--) {
        if (mask & 1) {
            pinMode(pinmap[pin], OUTPUT);
            digitalWrite(pinmap[pin], LOW);
        }
        mask >>= 1;
    }
}

/*
 * Display a picture on LoL shield.
 * Duration in milliseconds is specified.
 */
void lol(unsigned msec, const unsigned short *data)
{
    unsigned row, mask;
    const unsigned char *map;
    unsigned low[LOL_NPINS];

    /* Clear pin masks. */
    for (row = 0; row < LOL_NPINS; row++) {
        low[row] = 0;
    }

    /* Convert image to array of pin masks. */
    for (row = 0; row < LOL_NROW; row++) {
        mask = *data++ & ((1 << LOL_NCOL) - 1);
        map = &charlieplexing[row * LOL_NCOL * 2];
        while (mask != 0) {
            if (mask & 1) {
                low[map[0]] |= 1 << map[1];
            }
            map += 2;
            mask >>= 1;
        }
    }

    /* Display the image. */
    if (msec < LOL_NPINS)
        msec = LOL_NPINS;
    while (msec >= LOL_NPINS) {
        for (row = 0; row < LOL_NPINS; row++) {
            all_pins_tristate();

            /* Set one pin to high. */
            set_pin_high(row);

            /* Set other pins to low. */
            set_pins_low(low[row]);

            /* Pause to make it visible. */
            delay(1);
            msec--;

            all_pins_low();
        }
    }

    /* Turn display off. */
    all_pins_tristate();
}

/*
 * Demo 1:
 * 1) vertical lines;
 * 2) horizontal lines;
 * 3) all LEDs on.
 */
void demo1()
{
    static unsigned short picture[9];
    int y, frame;

    for (frame = 0; frame<14; frame++) {
        memset(picture, 0, sizeof(picture));

        for (y=0; y<9; y++)
            picture[y] |= 1 << frame;

        /* Display a frame. */
        lol(100, picture);
    }
    for (frame = 0; frame<9; frame++) {
        memset(picture, 0, sizeof(picture));

        picture[frame] = (1 << 14) - 1;

        /* Display a frame. */
        lol(100, picture);
    }
    memset(picture, 0xFF, sizeof(picture));

    /* Display a frame. */
    lol(250, picture);
    lol(250, picture);
}

/*
 * Demo 2: bouncing ball.
 */
void demo2()
{
    static unsigned short picture[9];
    int x, y, dx, dy, msec;

    memset(picture, 0, sizeof(picture));
    x = 0;
    y = 0;
    dx = 1;
    dy = 1;
    for (msec = 0; msec < 3000; msec += 50) {
        /* Draw ball. */
        picture[y] |= 1 << x;
        lol(50, picture);
        picture[y] &= ~(1 << x);

        /* Move the ball. */
        x += dx;
        if (x < 0 || x >= 14) {
            dx = -dx;
            x += dx + dx;
        }
        y += dy;
        if (y < 0 || y >= 9) {
            dy = -dy;
            y += dy + dy;
        }
    }
}

#define ROW(a,b,c,d,e,f,g) (a | b<<1 | c<<2 | d<<3 | e<<4 | f<<5 | g<<6)
#define _ 0
#define O 1

#define __  ROW(_,_,_,_,_,_,_)
#define _space __,__,__
#define _comma \
            ROW(O,O,_,_,_,_,_), \
            ROW(_,O,O,_,_,_,_), \
            __,__
#define _exclam __,\
            ROW(O,_,O,O,O,O,O), \
            __,__
#define _dot \
            ROW(O,O,_,_,_,_,_), \
            ROW(O,O,_,_,_,_,_), \
            __,__,__
#define _A  ROW(O,O,O,O,O,_,_), \
            ROW(_,_,O,_,_,O,_), \
            ROW(_,_,O,_,_,_,O), \
            ROW(_,_,O,_,_,O,_), \
            ROW(O,O,O,O,O,_,_), \
            __
#define _B  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,_,O,O,_), \
            __
#define _C  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,_,_,_,O,_), \
            __
#define _D  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,_,_,_,O,_), \
            ROW(_,_,O,O,O,_,_), \
            __
#define _E  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            __
#define _F  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            __
#define _G  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,O,_,O,_), \
            __
#define _H  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _I  ROW(O,_,_,_,_,_,O), \
            ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,O), \
            __
#define _J  ROW(_,O,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,O,O,O,O,O), \
            ROW(_,_,_,_,_,_,O), \
            __
#define _K  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,O,_,_), \
            ROW(_,_,O,_,_,O,_), \
            ROW(O,O,_,_,_,_,O), \
            __
#define _L  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            __
#define _M  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,_,O,_), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,_,_,O,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _N  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,O,_,_,_,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _O  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,O,O,O,O,_), \
            __
#define _P  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,_,O,O,_), \
            __
#define _R  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,O,O,_,_,O), \
            ROW(_,O,_,O,_,_,O), \
            ROW(O,_,_,_,O,O,_), \
            __
#define _S  ROW(_,_,_,_,O,O,_), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,_,_,_,_), \
            __
#define _T  ROW(_,_,_,_,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            __
#define _U  ROW(_,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,O), \
            __
#define _V  ROW(_,_,O,O,O,O,O), \
            ROW(_,O,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,_,_,_,_,_), \
            ROW(_,_,O,O,O,O,O), \
            __
#define _W  ROW(_,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,O), \
            __
#define _X  ROW(O,O,_,_,_,O,O), \
            ROW(_,_,O,_,O,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,O,_,O,_,_), \
            ROW(O,O,_,_,_,O,O), \
            __
#define _Y  ROW(_,_,_,_,_,O,O), \
            ROW(_,_,_,_,O,_,_), \
            ROW(O,O,O,O,_,_,_), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,_,_,O,O), \
            __
#define _2  ROW(O,_,_,_,_,O,_), \
            ROW(O,O,_,_,_,_,O), \
            ROW(O,_,O,_,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,_,O,O,_), \
            __
#define _3  ROW(_,O,_,_,_,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,_,O,O,_), \
            __
//
// ESP32 board
//
static const unsigned char phrase[] = {
    __,_E,_S,_P,_3,_2,_space,_B,_O,_A,_R,_D,
    __,__,__,__,__,__,__,__,__,__,__,__,__,
    0xff
};

void shift_picture(unsigned short *picture, int next)
{
    int y;

    for (y=0; y<9; y++) {
        picture[y] >>= 1;
        if (y > 0 && ((next << y) & 0x80))
            picture[y] |= 1 << 13;
    }
}

/*
 * Demo 3: moving text.
 */
void demo3()
{
    static unsigned short picture[9];
    const unsigned char *next;

    memset(picture, 0, sizeof(picture));
    for (next=phrase; *next!=0xff; next++) {
        shift_picture(picture, *next);
        lol(70, picture);
    }
}

void setup()
{
    /* Empty. */
}

void loop()
{
    demo1();
    demo2();
    demo3();
}
