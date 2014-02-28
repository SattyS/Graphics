// Assignment 2 - Polygon scan conversion and Rectangle clipping


#include "stdafx.h"

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <list>
using namespace std;


#define ImageW 400
#define ImageH 400
#define max(A,B)	((A) >= (B) ? (A):(B))
#define min(A,B)	((A) <= (B) ? (A):(B))

float framebuffer[ImageH][ImageW][3]; // Original polygon buffer
float clipFrameBuffer[ImageH][ImageW][3]; // Clipped polygon buffer

// Point
struct point{
	int x;
	int y;
};


static struct point p[20][20]; // [Polygon][Point]

static int dispClip = 0; // Becomes 1 after 's' is pressed on keyboard

static int currentPolygon; // Tracks the current polygon - starts from 0
static int currentPoint; // Tracks current point
static int polygonCount;
static int currentEdge; // Tracks current edge
static int clippingMode; // Becomes 1 after 'c' is pressed

static int clipRectStartX; // Clipping rectangle's StartX
static int clipRectStartY; // Clipping rectangle's StartY

static int clipRectEndX; // Clipping rectangle's EndX
static int clipRectEndY; // Clipping rectangle's EndY

static int maxScreenY = INT_MIN; // Track polygon's maxY
static int minScreenY = INT_MAX; // Track polygon's minY

struct color {
	float r, g, b;		// Color (R,G,B values)
};

static struct color col[20];

// Edge struct from polygon scan conversion
struct edge {
	int maxY;
	float currentX;
	float xIncr;
	int minY;
};

static struct edge e[20][20];

// Copy
struct polygon{
	struct edge pe[20][20];
	struct point pp[20][20];
	int pcurrentPolygon;
	struct color pc[20];
};

static struct polygon one;

// Clear Edge struct
void clearEdge(){
	for(int i=0 ;i < 20; i++){
		for(int j=0;j < 20; j++){
			e[i][j].currentX = 0;
			e[i][j].maxY = 0.0;
			e[i][j].xIncr = 0.0;
			e[i][j].minY = 0;

		}
	}
}
// Clear Point
void clearPoint(){
	for(int i=0 ;i < 20; i++){
		for(int j=0;j < 20; j++){
			p[i][j].x = 0;
			p[i][j].y = 0;
		}
	}
}
// Draws the scene
void drawit(float frame[400][400][3])
{
   glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,frame);
}

// Clears framebuffer to black
void clearFramebuffer(float frame[400][400][3])
{
	int i,j;

	for(i=0;i<ImageH;i++) {
		for (j=0;j<ImageW;j++) {
			frame[i][j][0] = 0.0;
			frame[i][j][1] = 0.0;
			frame[i][j][2] = 0.0;
		}
	}
}

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B, float frame[400][400][3])
{
	// changes the origin from the lower-left corner to the upper-left corner
	y = ImageH - 1 - y;
	if (R<=1.0)
		if (R>=0.0)
			frame[y][x][0]=R;
		else
			frame[y][x][0]=0.0;
	else
		frame[y][x][0]=1.0;
	if (G<=1.0)
		if (G>=0.0)
			frame[y][x][1]=G;
		else
			frame[y][x][1]=0.0;
	else
		frame[y][x][1]=1.0;
	if (B<=1.0)
		if (B>=0.0)
			frame[y][x][2]=B;
		else
			frame[y][x][2]=0.0;
	else
		frame[y][x][2]=1.0;
}

// Sort Compare
bool compare_currentX (const struct edge &first, const struct edge& second)
{
  return (first.currentX < second.currentX);
}

// Render Polygon after Edge list is constructed
void renderPolygon(float frame[400][400][3], struct color colA){

	std::list<struct edge> activeEdgeList;
	int scanLine = minScreenY;
	while(scanLine <= maxScreenY){
		for(int i=0; i<=currentEdge; i++){
			if(e[currentPolygon][i].minY == scanLine){
				activeEdgeList.push_back(e[currentPolygon][i]);
			}
		}
		activeEdgeList.sort(compare_currentX);

		for (std::list<struct edge>::iterator it = activeEdgeList.begin()
			; it != activeEdgeList.end(); ){
	
				struct edge tempEdge = *it;
				if(tempEdge.maxY == scanLine){
					it = activeEdgeList.erase(it);
				}else{
					++it;
				}	
		}

		int k = 0;
		double pointsX[10];
		for (std::list<struct edge>::iterator it = activeEdgeList.begin()
			; it != activeEdgeList.end(); ++it){

				struct edge tempEdge = *it;
				pointsX[k] = tempEdge.currentX;
				tempEdge.currentX = tempEdge.currentX + tempEdge.xIncr;
				*it = tempEdge;
				k++;
	
		}

		if(k & 1){
			k = k-1;
		}

		for(int i=0; i <k; i=i+2){
			
			int fp;
			int lp;

			if(i%2 == 0){
				fp = ceil(pointsX[i]);
				if(i+1 < k){
					lp = floor(pointsX[i+1]);
				}else{
					break;
				}
			}else{
				break;
			}

			if(fp == lp){
				// don't draw the point
			}else{
				for(int l=(int) fp; l<(int)lp; l++){
					setFramebuffer(l,scanLine,colA.r,colA.g,colA.b,frame);
				}
			}
		}
		scanLine++;
	}

}
// Display callback method
void display(void)
{
	// should not be necessary but some GPUs aren't displaying anything until a clear call.
	glClear (GL_COLOR_BUFFER_BIT);
	if(dispClip == 1)
		drawit(clipFrameBuffer);
	else
		drawit(framebuffer);
}
// Construct Edge struct
void populateEdgeStruct(float frame[400][400][3],struct color colA){
	
	int horizontalEdge = 0;
	int i;
	for(i=0; i<currentPoint; i++){
		int xPlus1 = p[currentPolygon][i+1].x;
		int yPlus1 = p[currentPolygon][i+1].y;
		int x = p[currentPolygon][i].x;
		int y = p[currentPolygon][i].y;
		if(y == yPlus1){
			// maxY -1 when its a horizontal edge
			e[currentPolygon][currentEdge].maxY = -1;
			horizontalEdge = 1;
		}else{
			e[currentPolygon][currentEdge].maxY = max(y,yPlus1);
			e[currentPolygon][currentEdge].minY = min(y,yPlus1);
			/* To find max and min Y points on the screen to scan line only those portions  */
			if(maxScreenY == INT_MIN){
				maxScreenY = e[currentPolygon][currentEdge].maxY;
			}
			else{
				if(e[currentPolygon][currentEdge].maxY > maxScreenY){
					maxScreenY = e[currentPolygon][currentEdge].maxY;
				}
			}
			if(minScreenY == INT_MAX){
				minScreenY = e[currentPolygon][currentEdge].minY;
			}
			else{
				if(e[currentPolygon][currentEdge].minY < minScreenY){
					minScreenY = e[currentPolygon][currentEdge].minY;
				}
			}
			/* To find max and min Y points on the screen to scan line only those portions  */
			if(y < yPlus1){
				e[currentPolygon][currentEdge].currentX = x;
			}else{
				e[currentPolygon][currentEdge].currentX = xPlus1;
			}
			e[currentPolygon][currentEdge].xIncr = (float) (xPlus1 - x) / (yPlus1 - y);
		}
		currentEdge++;
	}
	// Last edge
	int xPlus10 = p[currentPolygon][0].x;
	int yPlus10 = p[currentPolygon][0].y;
	int x0 = p[currentPolygon][i].x;
	int y0 = p[currentPolygon][i].y;
	if(y0 == yPlus10){
		e[currentPolygon][currentEdge].maxY = -1;
		horizontalEdge = 1;
	}else{
		e[currentPolygon][currentEdge].maxY = max(y0,yPlus10);
		e[currentPolygon][currentEdge].minY = min(y0,yPlus10);
		/* To find max and min Y points on the screen to scan line only those portions  */
			if(maxScreenY == INT_MIN){
				maxScreenY = e[currentPolygon][currentEdge].maxY;
			}
			else{
				if(e[currentPolygon][currentEdge].maxY > maxScreenY){
					maxScreenY = e[currentPolygon][currentEdge].maxY;
				}
			}
			if(minScreenY == INT_MAX){
				minScreenY = e[currentPolygon][currentEdge].minY;
			}
			else{
				if(e[currentPolygon][currentEdge].minY < minScreenY){
					minScreenY = e[currentPolygon][currentEdge].minY;
				}
			}
			/* To find max and min Y points on the screen to scan line only those portions  */
		if(y0 < yPlus10){
			e[currentPolygon][currentEdge].currentX = x0;
		}else{
			e[currentPolygon][currentEdge].currentX = xPlus10;
		}
		e[currentPolygon][currentEdge].xIncr = (float) (xPlus10 - x0) / (yPlus10 - y0);
	}

	renderPolygon(frame,colA);
	currentPolygon++;
	currentPoint = 0;
	currentEdge = 0;

}
// Clip
std::list<struct point> clip (std::list<struct point> li, int dir, int xy){

	std::list<struct point> retLi;
	struct point lastPoints;
	for (std::list<struct point>::iterator it = li.begin();;){
		if(it == li.end()){
			break;
		}
		struct point p0 = *it;
		struct point p1;
		it++;
		if(it != li.end()){
			p1 = *it;
		}else{
			break;
		}
		if(dir == 0){
			// left
			if(p0.x < xy && p1.x < xy){
				//outside
			}else if(p0.x >= xy && p1.x >= xy){
				//inside
				retLi.push_back(p1);
			}else if(p0.x < xy && p1.x > xy){
				//outside to inside
				int x = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int y;
				y = m*(x-p0.x) + p0.y;
				struct point temp;
				temp.x = x;
				temp.y = y;
				retLi.push_back(temp);
				retLi.push_back(p1);
			}else{
				//inside to outside
				int x = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int y;
				y = m*(x-p0.x) + p0.y;
				struct point temp;
				temp.x = x;
				temp.y = y;
				retLi.push_back(temp);
			}
			it--;
			it = li.erase(it);
			if(li.size() <= 1){
				break;
			}
			it = li.erase(it);
		}

		if(dir == 1){
			// right
			if(p0.x > xy && p1.x > xy){
				//outside
			}else if(p0.x <= xy && p1.x <= xy){
				//inside
				if(retLi.size() == 0){
					lastPoints = p1;
				}
				retLi.push_back(p1);
			}else if(p0.x > xy && p1.x < xy){
				//outside to inside
				int x = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int y = m*(x-p0.x) + p0.y;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
				retLi.push_back(p1);
			}else{
				//inside to outside
				int x = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int y = m*(x-p0.x) + p0.y;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
			}
			it--;
			it = li.erase(it);
			if(li.size() <= 1){
				break;
			}
			it = li.erase(it);
		}

		if(dir == 2){
			// top
			if(p0.y < xy && p1.y < xy){
				//outside
			}else if(p0.y >= xy && p1.y >= xy){
				//inside
				if(retLi.size() == 0){
					lastPoints = p1;
				}
				retLi.push_back(p1);
			}else if(p0.y < xy && p1.y > xy){
				//outside to inside
				int y = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int x = (y-p0.y)/m + p0.x;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
				retLi.push_back(p1);
			}else{
				//inside to outside
				int y = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int x = (y-p0.y)/m + p0.x;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
			}
			it--;
			it = li.erase(it);
			if(li.size() <= 1){
				break;
			}
			it = li.erase(it);
		}

		if(dir == 3){
			// bottom
			if(p0.y >= xy && p1.y >= xy){
				//outside
			}else if(p0.y < xy && p1.y < xy){
				//inside
				if(retLi.size() == 0){
					lastPoints = p1;
				}
				retLi.push_back(p1);
			}else if(p0.y > xy && p1.y < xy){
				//outside to inside
				int y = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int x = (y-p0.y)/m + p0.x;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
				retLi.push_back(p1);
			}else{
				//inside to outside
				int y = xy;
				float m = (float)(p1.y - p0.y) / (p1.x - p0.x);
				int x = (y-p0.y)/m + p0.x;
				struct point temp;
				temp.x = x;
				temp.y = y;
				if(retLi.size() == 0){
					lastPoints = temp;
				}
				retLi.push_back(temp);
			}
			it--;
			it = li.erase(it);
			if(li.size() <= 1){
				break;
			}
			it = li.erase(it);
		}
	}
	return retLi;
}
// Method to print points
void printPoints(std::list<struct point> a){

	for (std::list<struct point>::iterator it = a.begin()
			; it != a.end(); it++){
			struct point t = *it;
			cout << t.x << "-" << t.y << endl;
	}
}
// Clip wrapper
void clipPolygon(){

	// Sutherland - Hodgman Algorithm
	
	clearFramebuffer(clipFrameBuffer);
	int nPolyClip = 0;
	struct color colA[10];
	// All polygons
	for(int j=0 ; j < one.pcurrentPolygon; j++){

		std::list<struct point> clipListOnePoly;
		std::list<struct point> clipListTwoPoly;
		std::list<struct point> clipListThreePoly;
		std::list<struct point> clipListFourPoly;
		std::list<struct point> clipListFivePoly;

		colA[j].r = one.pc[j].r;
		colA[j].g = one.pc[j].g;
		colA[j].b = one.pc[j].b;
	
		//All edges/points
		
		for(int k=0; k < 10; k++){
			
			
			struct point p0,p1;
			p0.x = one.pp[j][k].x;
			p0.y = one.pp[j][k].y;
			if(k != 0){
				if(one.pp[j][k+1].x == 0){
					p1.x = one.pp[j][0].x;
					p1.y = one.pp[j][0].y;
					clipListOnePoly.push_back(p0);
					clipListOnePoly.push_back(p1);
					break;
				}else{
					p1.x = one.pp[j][k+1].x;
					p1.y = one.pp[j][k+1].y;
				}
			}else{
				p1.x = one.pp[j][k+1].x;
				p1.y = one.pp[j][k+1].y;
			}
			clipListOnePoly.push_back(p0);
			clipListOnePoly.push_back(p1);
		}
		/*
		for (std::list<struct point>::iterator it = clipListOnePoly.begin()
			; it != clipListOnePoly.end(); it++){
				struct point tempPoint = *it;
				cout << "List Points " << tempPoint.x << "-" << tempPoint.y << endl;
		}
		*/
		// Start algorithm - 1. inside/outside 2. intersection point 3.insert into list and move
		clipListTwoPoly = clip(clipListOnePoly, 0, clipRectStartX);
		//printPoints(clipListTwoPoly);
		int ctr = 0;
		struct point savePoint;
		if(clipListTwoPoly.size() != 0) // Polygon inside clipspace
		{
			struct point endPoint = clipListTwoPoly.back(); 
			for (std::list<struct point>::iterator it = clipListTwoPoly.begin()
				; it != clipListTwoPoly.end();it++){
					struct point tempPoint = *it;
					if(ctr == 0){
						savePoint = tempPoint;
					}else{
						if(ctr & 1){
							it = clipListTwoPoly.insert(it, tempPoint);
						}
					}
					ctr ++;
			}
			clipListTwoPoly.push_back(savePoint);
			ctr = 0;
			//printPoints(clipListTwoPoly);
			
			clipListThreePoly = clip(clipListTwoPoly, 1, clipRectEndX);
			if(clipListThreePoly.size() != 0){ // Polygon inside clipspace
			
				endPoint = clipListThreePoly.back(); 
				for (std::list<struct point>::iterator it = clipListThreePoly.begin()
					; it != clipListThreePoly.end();it++){
						struct point tempPoint = *it;
						if(ctr == 0){
							savePoint = tempPoint;
						}else{
							if(ctr & 1){
								it = clipListThreePoly.insert(it, tempPoint);
							}
						}
						ctr++;
				}
				clipListThreePoly.push_back(savePoint);
				//printPoints(clipListThreePoly);

				ctr = 0;
				clipListFourPoly = clip(clipListThreePoly, 2, clipRectEndY);
				if(clipListFourPoly.size() != 0){ // Polygon inside clipspace
				
						endPoint = clipListFourPoly.back(); 
						for (std::list<struct point>::iterator it = clipListFourPoly.begin()
							; it != clipListFourPoly.end();it++){
								struct point tempPoint = *it;
								if(ctr == 0){
									savePoint = tempPoint;
								}else{
									if(ctr & 1){
										it = clipListFourPoly.insert(it, tempPoint);
									}
								}
								ctr++;
						}
						clipListFourPoly.push_back(savePoint);
						//printPoints(clipListFourPoly);


						ctr = 0;
						clipListFivePoly = clip(clipListFourPoly, 3, clipRectStartY);
						if(clipListFivePoly.size() != 0){ // Polygon inside clipspace
							endPoint = clipListFivePoly.back(); 
							for (std::list<struct point>::iterator it = clipListFivePoly.begin()
								; it != clipListFivePoly.end();it++){
									struct point tempPoint = *it;
									if(ctr == 0){
										savePoint = tempPoint;
									}else{
										if(ctr & 1){
											it = clipListFivePoly.insert(it, tempPoint);
										}
									}
									ctr++;
							}
							clipListFivePoly.push_back(savePoint);
							//printPoints(clipListFivePoly);
							ctr = 0;
							currentPoint = 0;
							for (std::list<struct point>::iterator it = clipListFivePoly.begin()
								; it != clipListFivePoly.end();){

									struct point tempPoint = *it;
									p[nPolyClip][currentPoint].x = tempPoint.x;
									p[nPolyClip][currentPoint].y = tempPoint.y;
									currentPoint++;
									it++;
									if(it == clipListFivePoly.end())
										break;
									else
										it++;
									
							}
							currentPoint--;
							currentPolygon = nPolyClip;
							nPolyClip++;
							minScreenY = INT_MAX;
							maxScreenY = INT_MIN;
							populateEdgeStruct(clipFrameBuffer,colA[j]);
						}
					}
				}
			}
		}
	drawit(clipFrameBuffer);
	glFlush();
}
// On mouse click callback
void mouseClick(int button,int state,int x, int y){

	switch(button){
		
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
				if(!clippingMode){
					p[currentPolygon][currentPoint].x = x;
					p[currentPolygon][currentPoint].y = y;
					setFramebuffer(x,y,col[currentPolygon].r,col[currentPolygon].g,col[currentPolygon].b,framebuffer);
					drawit(framebuffer);
					glFlush();
					currentPoint++;
				}else{
					clipRectStartX = x;
					clipRectStartY = y;
				}
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){
				if(!clippingMode){
					p[currentPolygon][currentPoint].x = x;
					p[currentPolygon][currentPoint].y = y;
					setFramebuffer(x,y,col[currentPolygon].r,col[currentPolygon].g,col[currentPolygon].b,framebuffer);
					drawit(framebuffer);
					glFlush();
					populateEdgeStruct(framebuffer,col[currentPolygon]);
					drawit(framebuffer);
					glFlush();
					col[currentPolygon].r = ((double) rand() / (RAND_MAX));
					col[currentPolygon].g = ((double) rand() / (RAND_MAX));
					col[currentPolygon].b = ((double) rand() / (RAND_MAX));
				}
			}
			break;
	}

	if(state == GLUT_UP && clippingMode == 1 && dispClip != 1){
		
		clearPoint();
		clearEdge();
		// Changing startXY and endXY depending upon the way in which the clipping rectangle was drawn - Fix
		int tempStartX = clipRectStartX;
		int tempStartY = clipRectStartY;
		int tempEndX = clipRectEndX;
		int tempEndY = clipRectEndY;
		if(clipRectStartX < clipRectEndX){
			if(clipRectStartY < clipRectEndY){
				clipRectStartY = tempEndY;
				clipRectEndY = tempStartY;
			}else{
				//clipRectEndX = x;
				//clipRectEndY = y;
			}
		}else{
			if(clipRectStartY < clipRectEndY){
				clipRectStartX = tempEndX;
				clipRectStartY = tempEndY;
				clipRectEndX = tempStartX;
				clipRectEndY = tempStartY;
			}else{
				clipRectStartX = tempEndX;
				clipRectEndX = tempStartX;
			}
		}
		clipPolygon();
		dispClip = 1;
		
	}

}
// Mouse move callback for clipping rectangle
void motionMove(int x, int y){

	if(clippingMode){
		
		drawit(framebuffer);
		glColor3f(1.0,1.0,1.0);
		glLineStipple(1, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);
		glVertex2d(clipRectStartX, clipRectStartY);
		glVertex2d(x, clipRectStartY);
		glVertex2d(x, y);
		glVertex2d(clipRectStartX, y);
		glEnd();
		glFlush();
		clipRectEndX = x;
		clipRectEndY = y;
		dispClip = 0;
		glutPostRedisplay();
	}

}

// Keyboard callback
void keyboard(unsigned char key, int x, int y){

	switch(key){
	
		case 'c':
			// copy to polygon one
			one.pcurrentPolygon = currentPolygon;
			for(int i=0 ; i<20; i++){
				one.pc[i] = col[i];
				for(int j=0 ; j<20; j++){
					one.pe[i][j] = e[i][j];
					one.pp[i][j] = p[i][j];
				}
			}
			clippingMode = 1;
			break;

	/*
		case 's':
			clearPoint();
			clearEdge();
			clipPolygon();
			dispClip = 1;
	*/
	}

}
// Init
void init(void)
{
	gluOrtho2D ( 0, ImageW - 1, ImageH - 1, 0 );
	clearFramebuffer(framebuffer);
	col[currentPolygon].r = 1.0;
	col[currentPolygon].g = 1.0;
	col[currentPolygon].b = 1.0;
}
// Main
int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(ImageW,ImageH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Sathish Sekar - Homework 2");
	init();	
	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutMotionFunc(motionMove);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}


