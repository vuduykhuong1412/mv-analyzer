#ifndef __DEFINES_H__
#define __DEFINES_H__

#define MB_SIZE 16	// width and height of macroblock
#define FOCUS_RANGE 16	// search range in 4 directions
#define FOCUS_SIZE (MB_SIZE+FOCUS_RANGE * 2)	// width and height of focus area in yuv picture
#define FOCUS_ZOOM 8	// zoom factor of focus area, must be the multiply of 4(4, 8, 12, 16, ...)
#define FOCUS_PIX_SIZE (FOCUS_SIZE * FOCUS_ZOOM)	// zoomed focus size in pix
#define FOCUS_BKG_Y 128	// background Y value of focus area

#endif // __DEFINES_H__