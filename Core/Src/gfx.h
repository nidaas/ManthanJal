#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include "gfxfont.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "GUI.h"
#define DEG2RAD 0.0174532925

  void gfx_init(int16_t w, int16_t h); // Constructor

  /**********************************************************************/
  /*!
    @brief  Draw to the screen/framebuffer/etc.
    Must be overridden in subclass.
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
    @param color  16-bit pixel color.
  */
  /**********************************************************************/
  void gfx_drawPixel(int16_t x, int16_t y, uint16_t color) ;

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  void gfx_startWrite(void);
  void gfx_writePixel(int16_t x, int16_t y, uint16_t color);
  void gfx_writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color);
  void gfx_writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void gfx_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
  void gfx_endWrite(void);

  // CONTROL API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  void gfx_setRotation(uint8_t r);
  void gfx_invertDisplay(bool i);

  // BASIC DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.

  // It's good to implement those, even if using transaction API
  void gfx_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void gfx_fillScreen(uint16_t color);
  // Optional and probably not necessary to change
  void gfx_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void gfx_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void gfx_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
  void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
  void gfx_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void gfx_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void gfx_drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,  int16_t radius, uint16_t color);
  void gfx_fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,  int16_t radius, uint16_t color);
  int gfx_fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
  void gfx_fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);
  void gfx_drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);
  void gfx_drawRectWithAngle(int x, int y, int w, int h,int angle,int color);
  void gfx_FillRectWithAngle(int x, int y, int w, int h,int angle,int color);

  void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
  void gfx_drawBitmap1(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void gfx_drawBitmap2(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  void gfx_drawBitmap3(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void gfx_drawXBitmap4(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
  void gfx_drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h);
  void gfx_drawGrayscaleBitmap1(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
  void gfx_drawGrayscaleBitmap2(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
  void gfx_drawGrayscaleBitmap3(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
  void gfx_drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void gfx_drawRGBBitmap1(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
  void gfx_drawRGBBitmap2(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
  void gfx_drawRGBBitmap3(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
  void gfx_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
  void gfx_drawChar1(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);
  void gfx_getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  //void gfx_getTextBounds1(const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
 // void gfx_getTextBounds2(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  void gfx_setTextSize(uint8_t s);
  void gfx_setTextSize1(uint8_t sx, uint8_t sy);
  void gfx_setFont(const GFXfont *f);
  void gfx_setCursor(int16_t x, int16_t y);
  void gfx_setTextColor(uint16_t c);
  void gfx_setTextColor1(uint16_t c, uint16_t bg);
  void gfx_setTextWrap(bool w);
  void gfx_cp437(bool x );
  void gfx_write(uint8_t);
  void gfx_print(char * prt);
  int16_t gfx_width(void);
  int16_t gfx_height(void);
  uint8_t gfx_getRotation(void);
  int16_t gfx_getCursorX(void);
  int16_t gfx_getCursorY(void);
  void gfx_charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx,int16_t *miny, int16_t *maxx, int16_t *maxy);


  void GFX_Button(void);
  // "Classic" initButton() uses center & size
  void gfx_btn_initButton( int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
  void gfx_btn_initButton1(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,uint16_t textcolor, char *label, uint8_t textsize_x, uint8_t textsize_y);
  // New/alt initButton() uses upper-left corner & size
  void gfx_btn_initButtonUL( int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
  void gfx_btn_initButtonUL1( int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize_x, uint8_t textsize_y);
  void gfx_btn_drawButton(bool inverted );
  bool gfx_btn_contains(int16_t x, int16_t y);
  void gfx_btn_press(bool p);
  bool gfx_btn_justPressed();
  bool gfx_btn_justReleased();
  bool gfx_btn_isPressed(void);




  void gfx_canvas1_init(uint16_t w, uint16_t h);
  void gfx_canvas1_drawPixel(int16_t x, int16_t y, uint16_t color);
  void gfx_canvas1_fillScreen(uint16_t color);
  void gfx_canvas1_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas1_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  bool gfx_canvas1_getPixel(int16_t x, int16_t y);
  uint8_t *gfx_canvas1_getBuffer(void) ;
  bool gfx_canvas1_getRawPixel(int16_t x, int16_t y);
  void gfx_canvas1_drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas1_drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);




/// A GFX 8-bit canvas context for graphics
  void gfx_canvas8_init(uint16_t w, uint16_t h);
  void gfx_canvas8_drawPixel(int16_t x, int16_t y, uint16_t color);
  void gfx_canvas8_fillScreen(uint16_t color);
  void gfx_canvas8_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas8_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  uint8_t gfx_canvas8_getPixel(int16_t x, int16_t y) ;
  uint8_t *gfx_canvas8_getBuffer(void);
  uint8_t gfx_canvas8_getRawPixel(int16_t x, int16_t y) ;
  void gfx_canvas8_drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas8_drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);




///  A GFX 16-bit canvas context for graphics

  void gfx_canvas16_init(uint16_t w, uint16_t h);
  void gfx_canvas16_drawPixel(int16_t x, int16_t y, uint16_t color);
  void gfx_canvas16_fillScreen(uint16_t color);
  void gfx_canvas16_byteSwap(void);
  void gfx_canvas16_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas16_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  uint16_t gfx_canvas16_getPixel(int16_t x, int16_t y);
  uint16_t *gfx_canvas16_getBuffer(void);
  uint16_t gfx_canvas16_getRawPixel(int16_t x, int16_t y) ;
  void gfx_canvas16_drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gfx_canvas16_drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);






#endif // _ADAFRUIT_GFX_H
