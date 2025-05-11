#include "include.h"
#include "display.h"

TFT_eSPI tft = TFT_eSPI();

int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;

//---------------------------------------------------
void tftSetup()
{

#ifdef TFT_DISPLAY
  tft.init();
  tft.setRotation(1);
  tft.setTextDatum(MC_DATUM);

#ifdef GREEN_TFT
  // Remove the following lines if colors are displayed incorrectly
  tft.invertDisplay(1);
  tft.writecommand(ILI9341_GAMMASET); // Gamma curve selected
  tft.writedata(2);
  delay(120);
  tft.writecommand(ILI9341_GAMMASET); // Gamma curve selected
  tft.writedata(1);
#endif // GREEN_TFT
#endif // TFT_DISPLAY
}

//---------------------------------------------------
void tftClear()
{
  tft.fillScreen(TFT_BLUE); // Clear the screen before writing to it
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawCentreString(CODENAME, centerX, 30, FONT_SIZE_MEDIUM);
}

//---------------------------------------------------
void tftLine1(const char *msg)
{
#ifdef TFT_DISPLAY
  tft.drawCentreString(msg, centerX, centerY, FONT_SIZE_SMALL);
#endif // TFT_DISPLAY
}

//---------------------------------------------------
void tftLine2(const char *msg)
{
#ifdef TFT_DISPLAY

  tft.drawCentreString(msg, centerX, centerY + 20, FONT_SIZE_SMALL);
#endif // T
}

//---------------------------------------------------
void tftLine3(const char *msg)
{
#ifdef TFT_DISPLAY
  tft.drawCentreString(msg, centerX, centerY + 40, FONT_SIZE_SMALL);
#endif // TFT_DISPLAY
}