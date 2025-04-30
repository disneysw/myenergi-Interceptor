#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SPI.h>
#include <TFT_eSPI.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE_SMALL 2
#define FONT_SIZE_MEDIUM 4
#define FONT_SIZE_LARGE 6

void tftSetup();
void tftClear();
void tftLine1(const char *msg);
void tftLine2(const char *msg);
void tftLine3(const char *msg);

#endif //_DISPLAY_H