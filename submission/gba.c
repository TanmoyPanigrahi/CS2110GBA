#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *) 0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
    // TODO: IMPLEMENT - Done

    // (1)
    // Write a while loop that loops until we're NOT in vBlank anymore:
    // (This prevents counting one VBlank more than once if your app is too fast)
    while (*SCANLINECOUNTER > 160);
    // (2)
    // Write a while loop that keeps going until we're in vBlank:
    while (*SCANLINECOUNTER < 160);
    // (3)
    // Finally, increment the vBlank counter:
    vBlankCounter++;
}

static int __qran_seed= 42;
static int qran(void) {
    __qran_seed= 1664525*__qran_seed+1013904223;
    return (__qran_seed>>16) & 0x7FFF;
}

int randint(int min, int max) {
    return (qran()*(max-min)>>15)+min;
}

void setPixel(int x, int y, u16 color) {
    // TODO: IMPLEMENT -  Done
    // UNUSED(x);
    // UNUSED(y);
    // UNUSED(color);
    videoBuffer[OFFSET(x, y, WIDTH)] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color) {
    // TODO: IMPLEMENT - Done
    // UNUSED(x);
    // UNUSED(y);
    // UNUSED(width);
    // UNUSED(height);
    // UNUSED(color);

    for (int row = 0; row < height; row++) {
        DMA[3].src = &color;
        DMA[3].dst = &videoBuffer[OFFSET(y, row + x,  240)];
        DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

void drawFullScreenImageDMA(const u16 *image) {
    // TODO: IMPLEMENT -  Done
    // // UNUSED(image);

    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 240 * 160 | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | DMA_ON;
}

void drawImageDMA(int x, int y, int width, int height, const u16 *image) {
    // TODO: IMPLEMENT - Done
    // UNUSED(x);
    // UNUSED(y);
    // UNUSED(width);
    // UNUSED(height);
    // UNUSED(image);

    for (int row = 0; row < height; row++) {

        DMA[3].src = &image[OFFSET(0, width, row )];

        DMA[3].dst = &videoBuffer[OFFSET( y, x + row, 240)];

        DMA[3].cnt = width | DMA_SOURCE_INCREMENT |
                    DMA_DESTINATION_INCREMENT | DMA_ON;
    }
}

void fillScreenDMA(volatile u16 color) {
    // TODO: IMPLEMENT - Done
    UNUSED(color);
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 240*160 | DMA_ON | DMA_SOURCE_FIXED;
}

void drawChar(int x, int y, char ch, u16 color) {
    for(int i = 0; i<6; i++) {
        for(int j=0; j<8; j++) {
            if(fontdata_6x8[OFFSET(i, j, 6) + ch*48]) {
                setPixel(x+i, y+j, color);
            }
        }
    }
}

void drawString(int x, int y, char *str, u16 color) {
    while(*str) {
        drawChar(x, y, *str++, color);
        x += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}
