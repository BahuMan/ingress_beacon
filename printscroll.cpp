#include "Arduino.h"
#include "printscroll.h"
#include <Badge2020_TFT.h>

using namespace scroll;

ForwardScroll::ForwardScroll(Badge2020_TFT &tft): ForwardScroll::ForwardScroll(tft, ST77XX_WHITE, ST77XX_BLACK)
{
}

ForwardScroll::ForwardScroll(Badge2020_TFT &tft, uint16_t fColor, uint16_t bColor)
{
  fgColor = fColor;
  bgColor = bColor;
  screen = &tft;
}

void ForwardScroll::init()
{

  //very complicated way of getting character height
  int16_t x1, y1 = 0;
  screen->getTextBounds("A", (int16_t) 0, (int16_t) 0, &x1, &y1, &charWidth, &charHeight);
  delay(2000);
  
  //send command to indicate that in future, we want to scroll a window of 240 lines (total screen)
  uint8_t verticalScrollParams[] = {
    0, 
    0, //no "stable" non-scrolling portion at top
    0,
    240, //240 lines in our display; all scrolling. 240 < 255 so fits in a single byte
    0,
    320-240 //chip has memory for 320 lines, but my display only has 240, so keep remaining lines non-scrolling
  };
  screen->sendCommand(0x33, verticalScrollParams, (uint8_t)6);
  scrollNext = false;
}

void ForwardScroll::cls()
{
    screen->fillScreen(bgColor);
    screen->setCursor(0, 0);
    scroll(0); //since we will restart at top, we need to undo all scrolling
    scrollNext = false;
}

void ForwardScroll::scrollIfNecessary()
{
  if (screen->getCursorY() > 239)
  {
    scrollNext = true;
  }

  if (scrollNext && screen->getCursorX() < 1)
  {
    screen->setCursor(screen->getCursorX(), screen->getCursorY() % 240);
    screen->fillRect(0, screen->getCursorY(), 240, this->charHeight, this->bgColor);
    scroll(screen->getCursorY() + charHeight);
  }
}

void ForwardScroll::println(char c) {
  scrollIfNecessary();
  screen->println(c);
}

void ForwardScroll::println(const char *s)
{
  scrollIfNecessary();
  screen->println(s);   
}

void ForwardScroll::println(int n, int b)
{
  scrollIfNecessary();
  screen->println(n, b);   
}

void ForwardScroll::print(char c) {
  scrollIfNecessary();
  screen->print(c);
}

void ForwardScroll::print(const char *s)
{
  scrollIfNecessary();
  screen->print(s);   
}

void ForwardScroll::print(int n, int b)
{
  scrollIfNecessary();
  screen->print(n, b);   
}

void ForwardScroll::scroll(uint8_t pos)
{
  uint8_t scrollparams[2] = {0, pos};
  screen->sendCommand(0x37, scrollparams, (uint8_t)2);
}
