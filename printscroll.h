#ifndef PRINTSCROLL_H
#define PRINTSCROLL_H


#include <Badge2020_TFT.h>

namespace scroll
{

/*
 This class can print lines and text will scroll up if you reach the end of the screen.
 It uses hardware scrolling for very fast updates during scrolling.
 No "smooth" scrolling, though: each scroll is an entire line.

 known issues:
 - code is not re-entrant. ESP32 crashes when normal code and interrupt collide
 - you have to set textSize BEFORE you init this class; changing it afterward will break println()
 - if you print a line that's longer than screen width, wrapping might break the scrolling
 - @TOCHECK: limited combination with println() on Badge2020_TFT or AdaFruit_GFX library
 - don't call setCursor(x,y) on AdaFruit_GFX, you will end up in unexpected places
 - because of how hardware scrolling works, anything else you put on the screen will be at unexpected location.
   If you want to draw other elements, call ForwardScroll:cls() first.
 */
class ForwardScroll {
  public:
    ForwardScroll(Badge2020_TFT &tft);
    ForwardScroll(Badge2020_TFT &tft, uint16_t fColor, uint16_t bColor);
    void init();
    void cls();
    void println(const char *s);
    void println(char c);
    void println(int n, int b=DEC);
    void print(const char *s);
    void print(char c);
    void print(int n, int b=DEC);
    
  private:
    Badge2020_TFT *screen;  
    bool scrollNext;
    uint16_t charWidth, charHeight, fgColor, bgColor;
    void scrollIfNecessary();
    void scroll(uint8_t pos);

};

}
#endif