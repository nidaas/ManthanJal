/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#include "Adafruit_GFX.h"

#include "glcdfont.c"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif


int16_t WIDTH;        ///< This is the 'raw' display width - never changes
int16_t HEIGHT;       ///< This is the 'raw' display height - never changes
int16_t _width;       ///< Display width as modified by current rotation
int16_t _height;      ///< Display height as modified by current rotation
int16_t cursor_x;     ///< x location to start print()ing text
int16_t cursor_y;     ///< y location to start print()ing text
uint16_t textcolor;   ///< 16-bit background color for print()
uint16_t textbgcolor; ///< 16-bit text color for print()
uint8_t textsize_x;   ///< Desired magnification in X-axis of text to print()
uint8_t textsize_y;   ///< Desired magnification in Y-axis of text to print()
uint8_t rotation;     ///< Display rotation (0 thru 3)
bool wrap;            ///< If set, 'wrap' text at right edge of display
bool _cp437;          ///< If set, use correct CP437 charset (default is off)
GFXfont *gfxFont;     ///< Pointer to special font


//BTN
int16_t _x1, _y1; // Coordinates of top-left corner
uint16_t _w, _h;
uint8_t _textsize_x;
uint8_t _textsize_y;
uint16_t _outlinecolor, _fillcolor, _textcolor;
char _label[10];
bool currstate, laststate;
//BTN

// 1bit canvas
uint8_t *gfx_canvas1_buffer;
//8 bit canvas
uint8_t *gfx_canvas8_buffer;
//16 bit canvas
uint16_t *gfx_canvas16_buffer;

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
  return gfxFont->glyph + c;
}

uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
  return gfxFont->bitmap;
}


/**********************************************************************/
/*!
  @brief  Set text cursor location
  @param  x    X coordinate in pixels
  @param  y    Y coordinate in pixels
*/
/**********************************************************************/
void gfx_setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

/**********************************************************************/
 /*!
   @brief   Set text font color with transparant background
   @param   c   16-bit 5-6-5 Color to draw text with
   @note    For 'transparent' background, background and foreground
            are set to same color rather than using a separate flag.
 */
 /**********************************************************************/
 void gfx_setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

 /**********************************************************************/
  /*!
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   bg  16-bit 5-6-5 Color to draw background/fill with
  */
  /**********************************************************************/
  void gfx_setTextColor1(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }

  /**********************************************************************/
  /*!
  @brief  Set whether text that is too long for the screen width should
          automatically wrap around to the next line (else clip right).
  @param  w  true for wrapping, false for clipping
  */
  /**********************************************************************/
  void gfx_setTextWrap(bool w) { wrap = w; }

  /**********************************************************************/
   /*!
     @brief  Enable (or disable) Code Page 437-compatible charset.
             There was an error in glcdfont.c for the longest time -- one
             character (#176, the 'light shade' block) was missing -- this
             threw off the index of every character that followed it.
             But a TON of code has been written with the erroneous
             character indices. By default, the library uses the original
             'wrong' behavior and old sketches will still work. Pass
             'true' to this function to use correct CP437 character values
             in your code.
     @param  x  true = enable (new behavior), false = disable (old behavior)
   */
   /**********************************************************************/
   void gfx_cp437(bool x ) { _cp437 = x; }

   /************************************************************************/
   /*!
     @brief      Get width of the display, accounting for current rotation
     @returns    Width in pixels
   */
   /************************************************************************/
   int16_t gfx_width(void) { return _width; };

   /************************************************************************/
   /*!
     @brief      Get height of the display, accounting for current rotation
     @returns    Height in pixels
   */
   /************************************************************************/
   int16_t gfx_height(void) { return _height; }

   /************************************************************************/
   /*!
     @brief      Get rotation setting for display
     @returns    0 thru 3 corresponding to 4 cardinal rotations
   */
   /************************************************************************/
   uint8_t gfx_getRotation(void) { return rotation; }

   // get current cursor position (get rotation safe maximum values,
   // using: width() for x, gfx_height() for y)
   /************************************************************************/
   /*!
     @brief  Get text cursor X location
     @returns    X coordinate in pixels
   */
   /************************************************************************/
   int16_t gfx_getCursorX(void)  { return cursor_x; }

   /************************************************************************/
   /*!
     @brief      Get text cursor Y location
     @returns    Y coordinate in pixels
   */
   /************************************************************************/
   int16_t gfx_getCursorY(void)  { return cursor_y; }

   /**********************************************************************/
     /*!
       @brief    Sets button state, should be done by some touch function
       @param    p  True for pressed, false for not.
     */
     /**********************************************************************/
     void gfx_btn_press(bool p) {
       laststate = currstate;
       currstate = p;
     }

/**********************************************************************/
/*!
@brief    Query whether the button is currently pressed
@returns  True if pressed
*/
/**********************************************************************/
bool gfx_btn_isPressed(void) { return currstate; }


/**********************************************************************/
/*!
@brief    Get a pointer to the internal buffer memory
@returns  A pointer to the allocated buffer
*/
/**********************************************************************/
uint8_t *gfx_canvas1_getBuffer(void) { return gfx_canvas1_buffer; }

/**********************************************************************/
/*!
@brief    Get a pointer to the internal buffer memory
@returns  A pointer to the allocated buffer
*/
/**********************************************************************/
uint8_t *gfx_canvas8_getBuffer(void)  { return gfx_canvas8_buffer; }

/**********************************************************************/
 /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
 */
 /**********************************************************************/
 uint16_t *gfx_canvas16_getBuffer(void) { return gfx_canvas16_buffer; }



/**************************************************************************/
/*!
   @brief    Instatiate a GFX context for graphics! Can only be done by a
   superclass
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
void gfx_init(int16_t w, int16_t h) {
  _width = w;
  _height = h;
  rotation = 0;
  cursor_y = cursor_x = 0;
  textsize_x = textsize_y = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap = true;
  _cp437 = false;
  gfxFont = 0;
}

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
    	gfx_writePixel(y0, x0, color);
    } else {
    	gfx_writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

/**************************************************************************/
/*!
   @brief    Start a display-writing routine, overwrite in subclasses.
*/
/**************************************************************************/
void gfx_startWrite() {}

/**************************************************************************/
/*!
   @brief    Write a pixel, overwrite in subclasses if startWrite is defined!
    @param   x   x coordinate
    @param   y   y coordinate
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_writePixel(int16_t x, int16_t y, uint16_t color) {
	Draw_Pixe(x, y, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly vertical line, overwrite in subclasses if
   startWrite is defined!
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Overwrite in subclasses if startWrite is defined!
  // Can be just writeLine(x, y, x, y+h-1, color);
  // or writeFillRect(x, y, 1, h, color);
	gfx_drawFastVLine(x, y, h, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly horizontal line, overwrite in subclasses if
   startWrite is defined!
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Overwrite in subclasses if startWrite is defined!
  // Example: writeLine(x, y, x+w-1, y, color);
  // or writeFillRect(x, y, w, 1, color);
	gfx_drawFastHLine(x, y, w, color);
}

/**************************************************************************/
/*!
   @brief    Write a rectangle completely with one color, overwrite in
   subclasses if startWrite is defined!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                 uint16_t color) {
  // Overwrite in subclasses if desired!
	Fill_Rect(x, y, w, h, color);
}

/**************************************************************************/
/*!
   @brief    End a display-writing routine, overwrite in subclasses if
   startWrite is defined!
*/
/**************************************************************************/
void gfx_endWrite() {}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a
   subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	gfx_startWrite();
    gfx_writeLine(x, y, x, y + h - 1, color);
    gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a
   subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    gfx_startWrite();
    gfx_writeLine(x, y, x + w - 1, y, color);
    gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color. Update in subclasses if
   desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
     gfx_startWrite();
     for (int16_t i = x; i < x + w; i++) {
    	 gfx_writeFastVLine(i, y, h, color);
     }
     gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one color. Update in subclasses if
   desired!
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_fillScreen(uint16_t color) {
	Fill_Rect(0, 0, _width, _height, color);
}

/**************************************************************************/
/*!
   @brief    Draw a line
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  // Update in subclasses if desired!
  if (x0 == x1) {
    if (y0 > y1)
      _swap_int16_t(y0, y1);
    gfx_drawFastVLine(x0, y0, y1 - y0 + 1, color);
  } else if (y0 == y1) {
    if (x0 > x1)
      _swap_int16_t(x0, x1);
    gfx_drawFastHLine(x0, y0, x1 - x0 + 1, color);
  } else {
	  gfx_startWrite();
	  gfx_writeLine(x0, y0, x1, y1, color);
	  gfx_endWrite();
  }
}

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  gfx_startWrite();
  gfx_writePixel(x0, y0 + r, color);
  gfx_writePixel(x0, y0 - r, color);
  gfx_writePixel(x0 + r, y0, color);
  gfx_writePixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    gfx_writePixel(x0 + x, y0 + y, color);
    gfx_writePixel(x0 - x, y0 + y, color);
    gfx_writePixel(x0 + x, y0 - y, color);
    gfx_writePixel(x0 - x, y0 - y, color);
    gfx_writePixel(x0 + y, y0 + x, color);
    gfx_writePixel(x0 - y, y0 + x, color);
    gfx_writePixel(x0 + y, y0 - x, color);
    gfx_writePixel(x0 - y, y0 - x, color);
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of
   the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
    	gfx_writePixel(x0 + x, y0 + y, color);
    	gfx_writePixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
    	gfx_writePixel(x0 + x, y0 - y, color);
    	gfx_writePixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
    	gfx_writePixel(x0 - y, y0 + x, color);
    	gfx_writePixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
    	gfx_writePixel(x0 - y, y0 - x, color);
    	gfx_writePixel(x0 - x, y0 - y, color);
    }
  }
}

/**************************************************************************/
/*!
   @brief    Draw a circle with filled color
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	gfx_startWrite();
	gfx_writeFastVLine(x0, y0 - r, 2 * r + 1, color);
	gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
	gfx_endWrite();
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
    	  gfx_writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
    	  gfx_writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
    	  gfx_writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
    	  gfx_writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	gfx_startWrite();
	gfx_writeFastHLine(x, y, w, color);
	gfx_writeFastHLine(x, y + h - 1, w, color);
	gfx_writeFastVLine(x, y, h, color);
	gfx_writeFastVLine(x + w - 1, y, h, color);
	gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  gfx_startWrite();
  gfx_writeFastHLine(x + r, y, w - 2 * r, color);         // Top
  gfx_writeFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
  gfx_writeFastVLine(x, y + r, h - 2 * r, color);         // Left
  gfx_writeFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  gfx_drawCircleHelper(x + r, y + r, r, 1, color);
  gfx_drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  gfx_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  gfx_drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw/fill with
*/
/**************************************************************************/
void gfx_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  gfx_startWrite();
  gfx_writeFillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  gfx_fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  gfx_fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	gfx_drawLine(x0, y0, x1, y1, color);
	gfx_drawLine(x1, y1, x2, y2, color);
	gfx_drawLine(x2, y2, x0, y0, color);
}

/**************************************************************************/
/*!
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
*/
/**************************************************************************/
void gfx_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }

  gfx_startWrite();
  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    gfx_writeFastHLine(a, y0, b - a + 1, color);
    gfx_endWrite();
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    gfx_writeFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    gfx_writeFastHLine(a, y, b - a + 1, color);
  }
  gfx_endWrite();
}

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

/**************************************************************************/
/*!
   @brief      Draw a PROGMEM-resident 1-bit image at the specified (x,y)
   position, using the specified foreground color (unset bits are transparent).
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = (bitmap[j * byteWidth + i / 8]);
      if (byte & 0x80)
    	  gfx_writePixel(x + i, y, color);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief      Draw a PROGMEM-resident 1-bit image at the specified (x,y)
   position, using the specified foreground (for set bits) and background (unset
   bits) colors.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw pixels with
    @param    bg 16-bit 5-6-5 Color to draw background with
*/
/**************************************************************************/
void gfx_drawBitmap1(int16_t x, int16_t y, const uint8_t bitmap[],int16_t w, int16_t h, uint16_t color,uint16_t bg) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = (bitmap[j * byteWidth + i / 8]);
      gfx_writePixel(x + i, y, (byte & 0x80) ? color : bg);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief      Draw a RAM-resident 1-bit image at the specified (x,y) position,
   using the specified foreground color (unset bits are transparent).
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void gfx_drawBitmap2(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = bitmap[j * byteWidth + i / 8];
      if (byte & 0x80)
    	  gfx_writePixel(x + i, y, color);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief      Draw a RAM-resident 1-bit image at the specified (x,y) position,
   using the specified foreground (for set bits) and background (unset bits)
   colors.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw pixels with
    @param    bg 16-bit 5-6-5 Color to draw background with
*/
/**************************************************************************/
void gfx_drawBitmap3(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = bitmap[j * byteWidth + i / 8];
      gfx_writePixel(x + i, y, (byte & 0x80) ? color : bg);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief      Draw PROGMEM-resident XBitMap Files (*.xbm), exported from GIMP.
   Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
   C Array can be directly used with this function.
   There is no RAM-resident version of this function; if generating bitmaps
   in RAM, use the format defined by drawBitmap() and call that instead.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw pixels with
*/
/**************************************************************************/
void gfx_drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[],int16_t w, int16_t h, uint16_t color) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte >>= 1;
      else
        byte = (bitmap[j * byteWidth + i / 8]);
      // Nearly identical to drawBitmap(), only the bit order
      // is reversed here (left-to-right = LSB to MSB):
      if (byte & 0x01)
    	  gfx_writePixel(x + i, y, color);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 8-bit image (grayscale) at the specified
   (x,y) pos. Specifically for 8-bit display devices such as IS31FL3731; no
   color reduction/expansion is performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with grayscale bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) {
	gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
    	gfx_writePixel(x + i, y, (uint8_t)pgm_read_byte(&bitmap[j * w + i]));
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a RAM-resident 8-bit image (grayscale) at the specified (x,y)
   pos. Specifically for 8-bit display devices such as IS31FL3731; no color
   reduction/expansion is performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with grayscale bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawGrayscaleBitmap1(int16_t x, int16_t y, uint8_t *bitmap,int16_t w, int16_t h) {
	gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
    	gfx_writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 8-bit image (grayscale) with a 1-bit mask
   (set bits = opaque, unset bits = clear) at the specified (x,y) position.
   BOTH buffers (grayscale and mask) must be PROGMEM-resident.
   Specifically for 8-bit display devices such as IS31FL3731; no color
   reduction/expansion is performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with grayscale bitmap
    @param    mask  byte array with mask bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawGrayscaleBitmap2(int16_t x, int16_t y,const uint8_t bitmap[],const uint8_t mask[], int16_t w, int16_t h) {
  int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = (mask[j * bw + i / 8]);
      if (byte & 0x80) {
    	  gfx_writePixel(x + i, y, (uint8_t)pgm_read_byte(&bitmap[j * w + i]));
      }
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a RAM-resident 8-bit image (grayscale) with a 1-bit mask
   (set bits = opaque, unset bits = clear) at the specified (x,y) position.
   BOTH buffers (grayscale and mask) must be RAM-residentt, no mix-and-match
   Specifically for 8-bit display devices such as IS31FL3731; no color
   reduction/expansion is performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with grayscale bitmap
    @param    mask  byte array with mask bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawGrayscaleBitmap3(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
  int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
    	  gfx_writePixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified
   (x,y) position. For 16-bit display devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[],int16_t w, int16_t h) {
	gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
    	gfx_writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a RAM-resident 16-bit image (RGB 5/6/5) at the specified (x,y)
   position. For 16-bit display devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawRGBBitmap1(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
	gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
    	gfx_writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask
   (set bits = opaque, unset bits = clear) at the specified (x,y) position. BOTH
   buffers (color and mask) must be PROGMEM-resident. For 16-bit display
   devices; no color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    mask  byte array with monochrome mask bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawRGBBitmap2(int16_t x, int16_t y, const uint16_t bitmap[],const uint8_t mask[], int16_t w, int16_t h) {
  int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = (mask[j * bw + i / 8]);
      if (byte & 0x80) {
    	  gfx_writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
      }
    }
  }
  gfx_endWrite();
}

/**************************************************************************/
/*!
   @brief   Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask (set
   bits = opaque, unset bits = clear) at the specified (x,y) position. BOTH
   buffers (color and mask) must be RAM-resident. For 16-bit display devices; no
   color reduction performed.
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with 16-bit color bitmap
    @param    mask  byte array with monochrome mask bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
*/
/**************************************************************************/
void gfx_drawRGBBitmap4(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {
  int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  gfx_startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
    	  gfx_writePixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
  gfx_endWrite();
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param    size  Font magnification level, 1 is 'original' size
*/
/**************************************************************************/
void gfx_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
	gfx_drawChar1(x, y, c, color, bg, size, size);
}

// Draw a character
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param    size_x  Font magnification level in X-axis, 1 is 'original' size
    @param    size_y  Font magnification level in Y-axis, 1 is 'original' size
*/
/**************************************************************************/
void gfx_drawChar1(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) {

  if (!gfxFont) { // 'Classic' built-in font

    if ((x >= _width) ||              // Clip right
        (y >= _height) ||             // Clip bottom
        ((x + 6 * size_x - 1) < 0) || // Clip left
        ((y + 8 * size_y - 1) < 0))   // Clip top
      return;

    if (!_cp437 && (c >= 176))
      c++; // Handle 'classic' charset behavior

    gfx_startWrite();
    for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font[c * 5 + i]);
      for (int8_t j = 0; j < 8; j++, line >>= 1) {
        if (line & 1) {
          if (size_x == 1 && size_y == 1)
        	  gfx_writePixel(x + i, y + j, color);
          else
        	  gfx_writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y,
                          color);
        } else if (bg != color) {
          if (size_x == 1 && size_y == 1)
        	  gfx_writePixel(x + i, y + j, bg);
          else
        	  gfx_writeFillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
        }
      }
    }
    if (bg != color) { // If opaque, draw vertical line for last column
      if (size_x == 1 && size_y == 1)
    	  gfx_writeFastVLine(x + 5, y, 8, bg);
      else
    	  gfx_writeFillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
    }
    gfx_endWrite();

  } else { // Custom font

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    gfx_startWrite();
    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            gfx_writePixel(x + xo + xx, y + yo + yy, color);
          } else {
            gfx_writeFillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                          size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }
    gfx_endWrite();

  } // End classic vs custom font
}
/**************************************************************************/
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
*/
/**************************************************************************/
void gfx_write(uint8_t c) {
  if (!gfxFont) { // 'Classic' built-in font

    if (c == '\n') {              // Newline?
      cursor_x = 0;               // Reset x to zero,
      cursor_y += textsize_y * 8; // advance y one line
    } else if (c != '\r') {       // Ignore carriage returns
      if (wrap && ((cursor_x + textsize_x * 6) > _width)) { // Off right?
        cursor_x = 0;                                       // Reset x to zero,
        cursor_y += textsize_y * 8; // advance y one line
      }
      gfx_drawChar1(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
               textsize_y);
      cursor_x += textsize_x * 6; // Advance x one char
    }

  } else { // Custom font

    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          gfx_drawChar1(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }
}

void gfx_print(char * prt){
	while(*prt!=0){
		gfx_write(*prt);
		prt++;
	}
}

/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel
   that much bigger.
    @param  s  Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
*/
/**************************************************************************/
void gfx_setTextSize(uint8_t s) { gfx_setTextSize1(s, s); }

/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel
   that much bigger.
    @param  s_x  Desired text width magnification level in X-axis. 1 is default
    @param  s_y  Desired text width magnification level in Y-axis. 1 is default
*/
/**************************************************************************/
void gfx_setTextSize1(uint8_t s_x, uint8_t s_y) {
  textsize_x = (s_x > 0) ? s_x : 1;
  textsize_y = (s_y > 0) ? s_y : 1;
}

/**************************************************************************/
/*!
    @brief      Set rotation setting for display
    @param  x   0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
void gfx_setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}

/**************************************************************************/
/*!
    @brief Set the font to display when print()ing, either custom or default
    @param  f  The GFXfont object, if NULL use built in 6x8 font
*/
/**************************************************************************/
void gfx_setFont(const GFXfont *f) {
  if (f) {          // Font struct pointer passed in?
    if (!gfxFont) { // And no current font struct?
      // Switching from classic to new font behavior.
      // Move cursor pos down 6 pixels so it's on baseline.
      cursor_y += 6;
    }
  } else if (gfxFont) { // NULL passed.  Current font struct defined?
    // Switching from new to classic font behavior.
    // Move cursor pos up 6 pixels so it's at top-left of char.
    cursor_y -= 6;
  }
  gfxFont = (GFXfont *)f;
}

/**************************************************************************/
/*!
    @brief  Helper to determine size of a character with current font/size.
            Broke this out as it's used by both the PROGMEM- and RAM-resident
            getTextBounds() functions.
    @param  c     The ASCII character in question
    @param  x     Pointer to x location of character. Value is modified by
                  this function to advance to next character.
    @param  y     Pointer to y location of character. Value is modified by
                  this function to advance to next character.
    @param  minx  Pointer to minimum X coordinate, passed in to AND returned
                  by this function -- this is used to incrementally build a
                  bounding rectangle for a string.
    @param  miny  Pointer to minimum Y coord, passed in AND returned.
    @param  maxx  Pointer to maximum X coord, passed in AND returned.
    @param  maxy  Pointer to maximum Y coord, passed in AND returned.
*/
/**************************************************************************/
void gfx_charBounds(unsigned char c, int16_t *x, int16_t *y,
                              int16_t *minx, int16_t *miny, int16_t *maxx,
                              int16_t *maxy) {

  if (gfxFont) {

    if (c == '\n') { // Newline?
      *x = 0;        // Reset x to zero, advance y by one line
      *y += textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') { // Not a carriage return; is normal char
      uint8_t first = pgm_read_byte(&gfxFont->first),
              last = pgm_read_byte(&gfxFont->last);
      if ((c >= first) && (c <= last)) { // Char present in this font?
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t gw = pgm_read_byte(&glyph->width),
                gh = pgm_read_byte(&glyph->height),
                xa = pgm_read_byte(&glyph->xAdvance);
        int8_t xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
        if (wrap && ((*x + (((int16_t)xo + gw) * textsize_x)) > _width)) {
          *x = 0; // Reset x to zero, advance y by one line
          *y += textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        int16_t tsx = (int16_t)textsize_x, tsy = (int16_t)textsize_y,
                x1 = *x + xo * tsx, y1 = *y + yo * tsy, x2 = x1 + gw * tsx - 1,
                y2 = y1 + gh * tsy - 1;
        if (x1 < *minx)
          *minx = x1;
        if (y1 < *miny)
          *miny = y1;
        if (x2 > *maxx)
          *maxx = x2;
        if (y2 > *maxy)
          *maxy = y2;
        *x += xa * tsx;
      }
    }

  } else { // Default font

    if (c == '\n') {        // Newline?
      *x = 0;               // Reset x to zero,
      *y += textsize_y * 8; // advance y one line
      // min/max x/y unchaged -- that waits for next 'normal' character
    } else if (c != '\r') { // Normal char; ignore carriage returns
      if (wrap && ((*x + textsize_x * 6) > _width)) { // Off right?
        *x = 0;                                       // Reset x to zero,
        *y += textsize_y * 8;                         // advance y one line
      }
      int x2 = *x + textsize_x * 6 - 1, // Lower-right pixel of char
          y2 = *y + textsize_y * 8 - 1;
      if (x2 > *maxx)
        *maxx = x2; // Track max x, y
      if (y2 > *maxy)
        *maxy = y2;
      if (*x < *minx)
        *minx = *x; // Track min x, y
      if (*y < *miny)
        *miny = *y;
      *x += textsize_x * 6; // Advance x one char
    }
  }
}

/**************************************************************************/
/*!
    @brief  Helper to determine size of a string with current font/size.
            Pass string and a cursor position, returns UL corner and W,H.
    @param  str  The ASCII string to measure
    @param  x    The current cursor X
    @param  y    The current cursor Y
    @param  x1   The boundary X coordinate, returned by function
    @param  y1   The boundary Y coordinate, returned by function
    @param  w    The boundary width, returned by function
    @param  h    The boundary height, returned by function
*/
/**************************************************************************/
void gfx_getTextBounds(const char *str, int16_t x, int16_t y,
                                 int16_t *x1, int16_t *y1, uint16_t *w,
                                 uint16_t *h) {

  uint8_t c; // Current character
  int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
  // Bound rect is intentionally initialized inverted, so 1st char sets it

  *x1 = x; // Initial position is value passed in
  *y1 = y;
  *w = *h = 0; // Initial size is zero

  while ((c = *str++)) {
    // charBounds() modifies x/y to advance for each character,
    // and min/max x/y are updated to incrementally build bounding rect.
    gfx_charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
  }

  if (maxx >= minx) {     // If legit string bounds were found...
    *x1 = minx;           // Update x1 to least X coord,
    *w = maxx - minx + 1; // And w to bound rect width
  }
  if (maxy >= miny) { // Same for height
    *y1 = miny;
    *h = maxy - miny + 1;
  }
}

/**************************************************************************/
/*!
    @brief    Helper to determine size of a string with current font/size. Pass
   string and a cursor position, returns UL corner and W,H.
    @param    str    The ascii string to measure (as an arduino String() class)
    @param    x      The current cursor X
    @param    y      The current cursor Y
    @param    x1     The boundary X coordinate, set by function
    @param    y1     The boundary Y coordinate, set by function
    @param    w      The boundary width, set by function
    @param    h      The boundary height, set by function
*/
/**************************************************************************/
void gfx_getTextBounds1(const char* str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  if (strlen(str)!= 0) {
    gfx_getTextBounds(str, x, y, x1, y1, w, h);
  }
}

/**************************************************************************/
/*!
    @brief      Invert the display (ideally using built-in hardware command)
    @param   i  True if you want to invert, false to make 'normal'
*/
/**************************************************************************/
void gfx_invertDisplay(bool i) {
  // Do nothing, must be subclassed if supported by hardware
  (void)i; // disable -Wunused-parameter warning
}

/***************************************************************************/

/**************************************************************************/
/*!
   @brief    Create a simple drawn button UI element
*/
/**************************************************************************/
void gfx_btn_init(void) {  }

/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x       The X coordinate of the center of the button
   @param    y       The Y coordinate of the center of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize The font magnification of the label text
*/
/**************************************************************************/
// Classic initButton() function: pass center & size
void gfx_btn_initButton(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor,
                                     char *label, uint8_t textsize) {
  // Tweak arguments and pass to the newer initButtonUL() function...
	gfx_btn_initButtonUL( x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x       The X coordinate of the center of the button
   @param    y       The Y coordinate of the center of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize_x The font magnification in X-axis of the label text
   @param    textsize_y The font magnification in Y-axis of the label text
*/
/**************************************************************************/
// Classic initButton() function: pass center & size
void gfx_btn_initButton1(int16_t x, int16_t y,
                                     uint16_t w, uint16_t h, uint16_t outline,
                                     uint16_t fill, uint16_t textcolor,
                                     char *label, uint8_t textsize_x,
                                     uint8_t textsize_y) {
  // Tweak arguments and pass to the newer initButtonUL() function...
	gfx_btn_initButtonUL1( x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor,
               label, textsize_x, textsize_y);
}

/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings, with
   upper-left coordinates
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x1       The X coordinate of the Upper-Left corner of the button
   @param    y1       The Y coordinate of the Upper-Left corner of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize The font magnification of the label text
*/
/**************************************************************************/
void gfx_btn_initButtonUL( int16_t x1,
                                       int16_t y1, uint16_t w, uint16_t h,
                                       uint16_t outline, uint16_t fill,
                                       uint16_t textcolor, char *label,
                                       uint8_t textsize) {
	gfx_btn_initButtonUL1( x1, y1, w, h, outline, fill, textcolor, label, textsize,
               textsize);
}

/**************************************************************************/
/*!
   @brief    Initialize button with our desired color/size/settings, with
   upper-left coordinates
   @param    gfx     Pointer to our display so we can draw to it!
   @param    x1       The X coordinate of the Upper-Left corner of the button
   @param    y1       The Y coordinate of the Upper-Left corner of the button
   @param    w       Width of the buttton
   @param    h       Height of the buttton
   @param    outline  Color of the outline (16-bit 5-6-5 standard)
   @param    fill  Color of the button fill (16-bit 5-6-5 standard)
   @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
   @param    label  Ascii string of the text inside the button
   @param    textsize_x The font magnification in X-axis of the label text
   @param    textsize_y The font magnification in Y-axis of the label text
*/
/**************************************************************************/
void gfx_btn_initButtonUL1( int16_t x1,
                                       int16_t y1, uint16_t w, uint16_t h,
                                       uint16_t outline, uint16_t fill,
                                       uint16_t textcolor, char *label,
                                       uint8_t textsize_x, uint8_t textsize_y) {
  _x1 = x1;
  _y1 = y1;
  _w = w;
  _h = h;
  _outlinecolor = outline;
  _fillcolor = fill;
  _textcolor = textcolor;
  _textsize_x = textsize_x;
  _textsize_y = textsize_y;

  strncpy(_label, label, 9);
}

/**************************************************************************/
/*!
   @brief    Draw the button on the screen
   @param    inverted Whether to draw with fill/text swapped to indicate
   'pressed'
*/
/**************************************************************************/
void gfx_btn_drawButton(bool inverted) {
  uint16_t fill, outline, text;

  if (!inverted) {
    fill = _fillcolor;
    outline = _outlinecolor;
    text = _textcolor;
  } else {
    fill = _textcolor;
    outline = _outlinecolor;
    text = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  gfx_fillRoundRect(_x1, _y1, _w, _h, r, fill);
  gfx_drawRoundRect(_x1, _y1, _w, _h, r, outline);

  gfx_setCursor(_x1 + (_w / 3.5) - (strlen(_label) * 3 * _textsize_x),
                  _y1 + (_h / 1.6) - (4 * _textsize_y));
  gfx_setTextColor(text);
  gfx_setTextSize1(_textsize_x, _textsize_y);
  gfx_print(_label);
}

/**************************************************************************/
/*!
    @brief    Helper to let us know if a coordinate is within the bounds of the
   button
    @param    x       The X coordinate to check
    @param    y       The Y coordinate to check
    @returns  True if within button graphics outline
*/
/**************************************************************************/
bool gfx_btn_contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (int16_t)(_x1 + _w)) && (y >= _y1) &&
          (y < (int16_t)(_y1 + _h)));
}

/**************************************************************************/
/*!
   @brief    Query whether the button was pressed since we last checked state
   @returns  True if was not-pressed before, now is.
*/
/**************************************************************************/
bool gfx_btn_justPressed() { return (currstate && !laststate); }

/**************************************************************************/
/*!
   @brief    Query whether the button was released since we last checked state
   @returns  True if was pressed before, now is not.
*/
/**************************************************************************/
bool gfx_btn_justReleased() { return (!currstate && laststate); }

// -------------------------------------------------------------------------

// GFXcanvas1, GFXcanvas8 and GFXcanvas16 (currently a WIP, don't get too
// comfy with the implementation) provide 1-, 8- and 16-bit offscreen
// canvases, the address of which can be passed to drawBitmap() or
// pushColors() (the latter appears only in a couple of GFX-subclassed TFT
// libraries at this time).  This is here mostly to help with the recently-
// added proportionally-spaced fonts; adds a way to refresh a section of the
// screen without a massive flickering clear-and-redraw...but maybe you'll
// find other uses too.  VERY RAM-intensive, since the buffer is in MCU
// memory and not the display driver...GXFcanvas1 might be minimally useful
// on an Uno-class board, but this and the others are much more likely to
// require at least a Mega or various recent ARM-type boards (recommended,
// as the text+bitmap draw can be pokey).  GFXcanvas1 requires 1 bit per
// pixel (rounded up to nearest byte per scanline), GFXcanvas8 is 1 byte
// per pixel (no scanline pad), and GFXcanvas16 uses 2 bytes per pixel (no
// scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.

#ifdef __AVR__
// Bitmask tables of 0x80>>X and ~(0x80>>X), because X>>Y is slow on AVR
const uint8_t PROGMEM GFXcanvas1::GFXsetBit[] = {0x80, 0x40, 0x20, 0x10,
                                                 0x08, 0x04, 0x02, 0x01};
const uint8_t PROGMEM GFXcanvas1::GFXclrBit[] = {0x7F, 0xBF, 0xDF, 0xEF,
                                                 0xF7, 0xFB, 0xFD, 0xFE};
#endif

/**************************************************************************/
/*!
   @brief    Instatiate a GFX 1-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
void gfx_canvas1_init(uint16_t w, uint16_t h)  {
	gfx_init(w,h);
  uint16_t bytes = ((w + 7) / 8) * h;
  if ((gfx_canvas1_buffer = (uint8_t *)malloc(bytes))) {
    memset(gfx_canvas1_buffer, 0, bytes);
  }
}

/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
void gfx_canvas1_deinit(void) {
  if (gfx_canvas1_buffer)
    free(gfx_canvas1_buffer);
}

/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x     x coordinate
    @param  y     y coordinate
    @param  color Binary (on or off) color to fill with
*/
/**************************************************************************/
void gfx_canvas1_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (gfx_canvas1_buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    int16_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }

    uint8_t *ptr = &gfx_canvas1_buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
#ifdef __AVR__
    if (color)
      *ptr |= pgm_read_byte(&GFXsetBit[x & 7]);
    else
      *ptr &= pgm_read_byte(&GFXclrBit[x & 7]);
#else
    if (color)
      *ptr |= 0x80 >> (x & 7);
    else
      *ptr &= ~(0x80 >> (x & 7));
#endif
  }
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's binary color value, either 0x1 (on) or 0x0
   (off)
*/
/**********************************************************************/
bool gfx_canvas1_getPixel(int16_t x, int16_t y)  {
  int16_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return gfx_canvas1_getRawPixel(x, y);
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's binary color value, either 0x1 (on) or 0x0
   (off)
*/
/**********************************************************************/
bool gfx_canvas1_getRawPixel(int16_t x, int16_t y)  {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if( gfx_canvas1_getBuffer()) {
    uint8_t *buffer = gfx_canvas1_getBuffer();
    uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];

#ifdef __AVR__
    return ((*ptr) & pgm_read_byte(&GFXsetBit[x & 7])) != 0;
#else
    return ((*ptr) & (0x80 >> (x & 7))) != 0;
#endif
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color Binary (on or off) color to fill with
*/
/**************************************************************************/
void gfx_canvas1_fillScreen(uint16_t color) {
  if (gfx_canvas1_buffer) {
    uint16_t bytes = ((WIDTH + 7) / 8) * HEIGHT;
    memset(gfx_canvas1_buffer, color ? 0xFF : 0x00, bytes);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized vertical line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  h      Length of vertical line to be drawn, including first point
   @param  color  Color to fill with
*/
/**************************************************************************/
void gfx_canvas1_drawFastVLine(int16_t x, int16_t y, int16_t h,
                               uint16_t color) {

  if (h < 0) { // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= gfx_width()) || (y >= gfx_height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) { // Clip top
    h += y;
    y = 0;
  }
  if (y + h > gfx_height()) { // Clip bottom
    h = gfx_height() - y;
  }

  if (gfx_getRotation() == 0) {
	  gfx_canvas1_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    gfx_canvas1_drawFastRawHLine(x, y, h, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    gfx_canvas1_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    gfx_canvas1_drawFastRawHLine(x, y, h, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      Length of horizontal line to be drawn, including first point
   @param  color  Color to fill with
*/
/**************************************************************************/
void gfx_canvas1_drawFastHLine(int16_t x, int16_t y, int16_t w,
                               uint16_t color) {
  if (w < 0) { // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y < 0) || (y >= gfx_height()) || (x >= gfx_width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) { // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= gfx_width()) { // Clip right
    w = gfx_width() - x;
  }

  if (gfx_getRotation() == 0) {
	  gfx_canvas1_drawFastRawHLine(x, y, w, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    gfx_canvas1_drawFastRawVLine(x, y, w, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    gfx_canvas1_drawFastRawHLine(x, y, w, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    gfx_canvas1_drawFastRawVLine(x, y, w, color);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   Binary (on or off) color to fill with
*/
/**************************************************************************/
void gfx_canvas1_drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  int16_t row_bytes = ((WIDTH + 7) / 8);
  uint8_t *buffer = gfx_canvas1_getBuffer();
  uint8_t *ptr = &buffer[(x / 8) + y * row_bytes];

  if (color > 0) {
#ifdef __AVR__
    uint8_t bit_mask = pgm_read_byte(&GFXsetBit[x & 7]);
#else
    uint8_t bit_mask = (0x80 >> (x & 7));
#endif
    for (int16_t i = 0; i < h; i++) {
      *ptr |= bit_mask;
      ptr += row_bytes;
    }
  } else {
#ifdef __AVR__
    uint8_t bit_mask = pgm_read_byte(&GFXclrBit[x & 7]);
#else
    uint8_t bit_mask = ~(0x80 >> (x & 7));
#endif
    for (int16_t i = 0; i < h; i++) {
      *ptr &= bit_mask;
      ptr += row_bytes;
    }
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized horizontal line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    w   length of horizontal line to be drawn, including first point
   @param    color   Binary (on or off) color to fill with
*/
/**************************************************************************/
void gfx_canvas1_drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  int16_t rowBytes = ((WIDTH + 7) / 8);
  uint8_t *buffer = gfx_canvas1_getBuffer();
  uint8_t *ptr = &buffer[(x / 8) + y * rowBytes];
  size_t remainingWidthBits = w;

  // check to see if first byte needs to be partially filled
  if ((x & 7) > 0) {
    // create bit mask for first byte
    uint8_t startByteBitMask = 0x00;
    for (int8_t i = (x & 7); ((i < 8) && (remainingWidthBits > 0)); i++) {
#ifdef __AVR__
      startByteBitMask |= pgm_read_byte(&GFXsetBit[i]);
#else
      startByteBitMask |= (0x80 >> i);
#endif
      remainingWidthBits--;
    }
    if (color > 0) {
      *ptr |= startByteBitMask;
    } else {
      *ptr &= ~startByteBitMask;
    }

    ptr++;
  }

  // do the next remainingWidthBits bits
  if (remainingWidthBits > 0) {
    size_t remainingWholeBytes = remainingWidthBits / 8;
    size_t lastByteBits = remainingWidthBits % 8;
    uint8_t wholeByteColor = color > 0 ? 0xFF : 0x00;

    memset(ptr, wholeByteColor, remainingWholeBytes);

    if (lastByteBits > 0) {
      uint8_t lastByteBitMask = 0x00;
      for (size_t i = 0; i < lastByteBits; i++) {
#ifdef __AVR__
        lastByteBitMask |= pgm_read_byte(&GFXsetBit[i]);
#else
        lastByteBitMask |= (0x80 >> i);
#endif
      }
      ptr += remainingWholeBytes;

      if (color > 0) {
        *ptr |= lastByteBitMask;
      } else {
        *ptr &= ~lastByteBitMask;
      }
    }
  }
}

/**************************************************************************/
/*!
   @brief    Instatiate a GFX 8-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
void gfx_canvas8_init(uint16_t w, uint16_t h) {
  gfx_init(w,h);
  uint32_t bytes = w * h;
  if ((gfx_canvas8_buffer = (uint8_t *)malloc(bytes))) {
    memset(gfx_canvas8_buffer, 0, bytes);
  }
}

/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
void gfx_canvas8_deinit(void) {
  if (gfx_canvas8_buffer)
    free(gfx_canvas8_buffer);
}

/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 8-bit Color to fill with. Only lower byte of uint16_t is used.
*/
/**************************************************************************/
void gfx_canvas8_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (gfx_canvas8_buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    int16_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }

    gfx_canvas8_buffer[x + y * WIDTH] = color;
  }
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 8-bit color value
*/
/**********************************************************************/
uint8_t gfx_canvas8_getPixel(int16_t x, int16_t y)  {
  int16_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return gfx_canvas8_getRawPixel(x, y);
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 8-bit color value
*/
/**********************************************************************/
uint8_t gfx_canvas8_getRawPixel(int16_t x, int16_t y)  {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (gfx_canvas8_buffer) {
    return gfx_canvas8_buffer[x + y * WIDTH];
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 8-bit Color to fill with. Only lower byte of uint16_t is used.
*/
/**************************************************************************/
void gfx_canvas8_fillScreen(uint16_t color) {
  if (gfx_canvas8_buffer) {
    memset(gfx_canvas8_buffer, color, WIDTH * HEIGHT);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized vertical line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  h      Length of vertical line to be drawn, including first point
   @param  color  8-bit Color to fill with. Only lower byte of uint16_t is
                  used.
*/
/**************************************************************************/
void gfx_canvas8_drawFastVLine(int16_t x, int16_t y, int16_t h,
                               uint16_t color) {
  if (h < 0) { // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= gfx_width()) || (y >= gfx_height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) { // Clip top
    h += y;
    y = 0;
  }
  if (y + h > gfx_height()) { // Clip bottom
    h = gfx_height() - y;
  }

  if (gfx_getRotation() == 0) {
	  gfx_canvas8_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    gfx_canvas8_drawFastRawHLine(x, y, h, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    gfx_canvas8_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    gfx_canvas8_drawFastRawHLine(x, y, h, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      Length of horizontal line to be drawn, including 1st point
   @param  color  8-bit Color to fill with. Only lower byte of uint16_t is
                  used.
*/
/**************************************************************************/
void gfx_canvas8_drawFastHLine(int16_t x, int16_t y, int16_t w,
                               uint16_t color) {

  if (w < 0) { // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y < 0) || (y >= gfx_height()) || (x >= gfx_width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) { // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= gfx_width()) { // Clip right
    w = gfx_width() - x;
  }

  if (gfx_getRotation() == 0) {
	  gfx_drawFastHLine(x, y, w, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    gfx_drawFastVLine(x, y, w, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    gfx_drawFastHLine(x, y, w, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    gfx_drawFastVLine(x, y, w, color);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   8-bit Color to fill with. Only lower byte of uint16_t is
   used.
*/
/**************************************************************************/
void gfx_canvas8_drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint8_t *buffer_ptr = gfx_canvas8_buffer + y * WIDTH + x;
  for (int16_t i = 0; i < h; i++) {
    (*buffer_ptr) = color;
    buffer_ptr += WIDTH;
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized horizontal line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    w   length of horizontal line to be drawn, including first point
   @param    color   8-bit Color to fill with. Only lower byte of uint16_t is
   used.
*/
/**************************************************************************/
void gfx_canvas8_drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                  uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  memset(gfx_canvas8_buffer + y * WIDTH + x, color, w);
}

/**************************************************************************/
/*!
   @brief    Instatiate a GFX 16-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
void gfx_canvas16_init(uint16_t w, uint16_t h) {
	gfx_init(w,h);
  uint32_t bytes = w * h * 2;
  if ((gfx_canvas16_buffer = (uint16_t *)malloc(bytes))) {
    memset(gfx_canvas16_buffer, 0, bytes);
  }
}

/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
void gfx_canvas16_GFXcanvas16_deinit(void) {
  if (gfx_canvas16_buffer)
    free(gfx_canvas16_buffer);
}

/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_canvas16_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (gfx_canvas16_buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    int16_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }

    gfx_canvas16_buffer[x + y * WIDTH] = color;
  }
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t gfx_canvas16_getPixel(int16_t x, int16_t y) {
  int16_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return gfx_canvas16_getRawPixel(x, y);
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t gfx_canvas16_getRawPixel(int16_t x, int16_t y)  {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (gfx_canvas16_buffer) {
    return gfx_canvas16_buffer[x + y * WIDTH];
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void gfx_canvas16_fillScreen(uint16_t color) {
  if (gfx_canvas16_buffer) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(gfx_canvas16_buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++)
    	  gfx_canvas16_buffer[i] = color;
    }
  }
}

/**************************************************************************/
/*!
    @brief  Reverses the "endian-ness" of each 16-bit pixel within the
            canvas; little-endian to big-endian, or big-endian to little.
            Most microcontrollers (such as SAMD) are little-endian, while
            most displays tend toward big-endianness. All the drawing
            functions (including RGB bitmap drawing) take care of this
            automatically, but some specialized code (usually involving
            DMA) can benefit from having pixel data already in the
            display-native order. Note that this does NOT convert to a
            SPECIFIC endian-ness, it just flips the bytes within each word.
*/
/**************************************************************************/
void gfx_canvas16_byteSwap(void) {
  if (gfx_canvas16_buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++)
    	gfx_canvas16_buffer[i] = __builtin_bswap16(gfx_canvas16_buffer[i]);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void gfx_canvas16_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (h < 0) { // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= gfx_width()) || (y >= gfx_height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) { // Clip top
    h += y;
    y = 0;
  }
  if (y + h > gfx_height()) { // Clip bottom
    h = gfx_height() - y;
  }

  if (gfx_getRotation() == 0) {
	  gfx_canvas16_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    gfx_canvas16_drawFastRawHLine(x, y, h, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    gfx_canvas16_drawFastRawVLine(x, y, h, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    gfx_canvas16_drawFastRawHLine(x, y, h, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      Length of horizontal line to be drawn, including 1st point
   @param  color  Color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void gfx_canvas16_drawFastHLine(int16_t x, int16_t y, int16_t w,
                                uint16_t color) {
  if (w < 0) { // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y < 0) || (y >= gfx_height()) || (x >= gfx_width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) { // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= gfx_width()) { // Clip right
    w = gfx_width() - x;
  }

  if (gfx_getRotation() == 0) {
	  gfx_canvas16_drawFastRawHLine(x, y, w, color);
  } else if (gfx_getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    gfx_canvas16_drawFastRawVLine(x, y, w, color);
  } else if (gfx_getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    gfx_canvas16_drawFastRawHLine(x, y, w, color);
  } else if (gfx_getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    gfx_canvas16_drawFastRawVLine(x, y, w, color);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void gfx_canvas16_drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                   uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint16_t *buffer_ptr = gfx_canvas16_buffer + y * WIDTH + x;
  for (int16_t i = 0; i < h; i++) {
    (*buffer_ptr) = color;
    buffer_ptr += WIDTH;
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized horizontal line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    w   length of horizontal line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void gfx_canvas16_drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                   uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint32_t buffer_index = y * WIDTH + x;
  for (uint32_t i = buffer_index; i < buffer_index + w; i++) {
	  gfx_canvas16_buffer[i] = color;
  }
}
