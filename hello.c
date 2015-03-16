#include <stdio.h>
#include <dos.h>

/*

for the assembly for setting up and tearing down of direct font access

Copyright (C) 1990, 1991 by Dave Kirsch.  You may use this program, or
code or tables extracted from it, as desired without restriction.
I can not and will not be held responsible for any damage caused from
the use of this software.

Author: a563@mindlink.UUCP  -or-  Zoid@cc.sfu.ca

*/

main()
{
    int x, y, i, j, c, row, lol;
    int buffer[32 * 240];

/*    outportb(0x3c4, 0x402);*/ /* Mask reg; enable write to map 2 */
/*    outportb(0x3c4, 0x704);*/ /* Memory Mode reg ; alpha, ext mem, non-interleaved */
/*    outportb(0x3ce, 0x005);*/ /* Graphics Mode reg; non-interleaved access */
/*    outportb(0x3ce, 0x406);*/ /* Graphics Misc reg; map char gen RAM to a000:0 */
/*    outportb(0x3ce, 0x204);*/ /* Graphics ReadMapSelect reg; enable read chargen RAM */
/*    memset(MK_FP(0xa000, 'C' * 32), 0, 32);*/

    for (j = 0; j < 9; j++) {
        for (i = 1; i < 247; i++) {
            if (i > 6 && i < 11) continue;
            if (i == 13) continue;
            if (i == 27) continue;
            printf("%c", i);
            if (j == 7 && i > 244) break;
        }
    }

    asm pushf; /* Disable interrupts */
    asm cli;
    asm mov dx, 3c4h; /* Sequencer port address */
    asm mov ax, 0704h; /* Sequential addressing */
    asm out dx, ax;

    /* Program the Graphics Controller */
    asm mov dx, 3ceh; /* Graphics Controller port address */
    asm mov ax, 0204h; /* Select map 2 for CPU reads */
    asm out dx, ax;
    asm mov ax, 0005h; /* Disable odd-even addressing */
    asm out dx, ax;
    asm mov ax, 0406h; /* Map starts at A000:0000 (64K mode) */
    asm out dx, ax;
    asm popf;

    /* backup font */
    row = 0;
    for (c = 1; c < 247; c++) {
        if (c > 6 && c < 11) continue;
        if (c == 13) continue;
        if (c == 27) continue;
        for (i=0; i<16; i++) {
            buffer[row * 32 + i] = peekb(0xa000, c*32 + i);
        }
        row++;
    }

    asm mov dx, 3c4h; /* Sequencer port address */
    asm mov ax, 0402h; /* CPU writes only to map 2 */
    asm out dx, ax;

    lol = 0;
    for (i=0; i < 100; i++) {
        for (x=1; x<247; x++) {
            if (x > 6 && x < 11) continue;
            if (x == 13) continue;
            if (x == 27) continue;
            for (y=0; y<16; y++) {
                /*        pokeb(0xa000, 's'*32 + i, peekb(0xa000, 'F'*32 + i));*/
                pokeb(0xa000, x * 32 + y, lol);
                /*printf("0x%x\n", 0xa000 + ' '*32 + i);*/
            }
        }
        lol++;
        if (lol > 0xff) lol = 0;
        /* wait until any previous retrace has ended */
        do { } while (inportb(0x3DA) & 8);
        /* wait until a new retrace has just begun */
        do { } while (!(inportb(0x3DA) & 8));
    }

/*    getch();*/
    /* restore font */
    row = 0;
    for (c = 1; c < 247; c++) {
        if (c > 6 && c < 11) continue;
        if (c == 13) continue;
        if (c == 27) continue;
        for (i=0; i<16; i++) {
            pokeb(0xa000, c*32 + i, buffer[row * 32 + i]);
        }
        row++;
    }

    /* Program the Sequencer */
    asm pushf; /* Disable interrupts */
    asm cli;
    asm mov dx, 3c4h; /* Sequencer port address */
    asm mov ax, 0302h; /* CPU writes to maps 0 and 1 */
    asm out dx, ax;
    asm mov ax, 0304h; /* Odd-even addressing */
    asm out dx, ax;

    /* Program the Graphics Controller */
    asm mov dx, 3ceh; /* Graphics Controller port address */
    asm mov ax, 0004h; /* Select map 0 for CPU reads */
    asm out dx, ax;
    asm mov ax, 1005h; /* Enable odd-even addressing */
    asm out dx, ax;
    asm sub ax, ax;
    asm mov es, ax; /* Segment 0 */
    asm mov ax, 0e06h; /* Map starts at B800:0000 */
    asm mov bl, 7;
    asm cmp es:[49h], bl; /* Get current video mode */
    asm jne notmono;
    asm mov ax, 0806h; /* Map starts at B000:0000 */
notmono:
    asm out dx, ax;
    asm popf;


/*    outportb(0x3c4, 0x0302);*/ /* Mask reg; disable write to map 2*/
/*    outportb(0x3c4, 0x0304);*/ /* Memory Mode reg; alpha, ext mem, interleaved*/
/*    outportb(0x3ce, 0x1005);*/ /* Graphics Mode reg; interleaved access*/
/*    outportb(0x3ce, 0x0e06);*/ /* Graphics Misc reg; regen buffer to b800:0*/
/*    outportb(0x3ce, 0x0004);*/ /* Graphics ReadMapSelect reg; disable read chargen RAM*/

}
