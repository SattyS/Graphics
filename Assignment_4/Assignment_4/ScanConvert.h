#ifndef SCAN_CONVERT_H
#define SCAN_CONVERT_H

#define ImageW 400
#define ImageH 400

struct color {
	float r, g, b;		// Color (R,G,B values)
};

class _Pt
{
public:
	float x, y, z;

	_Pt ( void )
	{
		x =0;
		y =0;
		z =0;
	}

	_Pt ( float nX, float nY, float nZ )
	{
		x = nX;
		y = nY;
		z = nZ;
	}
};

// Clears framebuffer to black
void clearFramebuffer();

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B);

struct color calcLighting(_Pt normal);

#endif