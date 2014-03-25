#ifndef POLYGON_DRAWER_H
#define POLYGON_DRAWER_H

#include <vector>

using namespace std;

class Pt
{
public:
	int x, y;

	Pt ( void )
	{
		x = y = 0;
	}

	Pt ( int nX, int nY )
	{
		x = nX;
		y = nY;
	}
};

void drawPolygon ( vector<Pt> points , struct color singleColor, vector<struct color> polyColor, vector<_Pt> polyNormal, _Pt normal, _Pt point, int type, bool init);

#endif