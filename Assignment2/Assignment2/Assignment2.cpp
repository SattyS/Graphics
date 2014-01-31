// Assignment2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <list>
using namespace std;
/******************************************************************
	Notes:
	Image size is 400 by 400 by default.  You may adjust this if
		you want to.
	You can assume the window will NOT be resized.
	Call clearFramebuffer to clear the entire framebuffer.
	Call setFramebuffer to set a pixel.  This should be the only
		routine you use to set the color (other than clearing the
		entire framebuffer).  drawit() will cause the current
		framebuffer to be displayed.
	As is, your scan conversion should probably be called from
		within the display function.  There is a very short sample
		of code there now.
	You may add code to any of the subroutines here,  You probably
		want to leave the drawit, clearFramebuffer, and
		setFramebuffer commands alone, though.
  *****************************************************************/

#define ImageW 400
#define ImageH 400
#define max(A,B)	((A) > (B) ? (A):(B))
#define min(A,B)	((A) < (B) ? (A):(B))
//#define INT_MIN		5000
//#define INT_MAX		-1


float framebuffer[ImageH][ImageW][3];

struct point{
	int x;
	int y;
};
static struct point p[10][10];

static int currentPolygon;
static int currentPoint;
static int polygonCount;
static int currentEdge;

static int maxScreenY = INT_MIN;
static int minScreenY = INT_MAX;

struct color {
	float r, g, b;		// Color (R,G,B values)
};

static struct color col;


struct edge {
	int maxY;
	float currentX;
	float xIncr;
	int minY;
};

static struct edge e[10][10];
// Draws the scene
void drawit(void)
{
   glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,framebuffer);
}

// Clears framebuffer to black
void clearFramebuffer()
{
	int i,j;

	for(i=0;i<ImageH;i++) {
		for (j=0;j<ImageW;j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}

// Sets pixel x,y to the color RGB
// I've made a small change to this function to make the pixels match
// those returned by the glutMouseFunc exactly - Scott Schaefer 
void setFramebuffer(int x, int y, float R, float G, float B)
{
	// changes the origin from the lower-left corner to the upper-left corner
	y = ImageH - 1 - y;
	if (R<=1.0)
		if (R>=0.0)
			framebuffer[y][x][0]=R;
		else
			framebuffer[y][x][0]=0.0;
	else
		framebuffer[y][x][0]=1.0;
	if (G<=1.0)
		if (G>=0.0)
			framebuffer[y][x][1]=G;
		else
			framebuffer[y][x][1]=0.0;
	else
		framebuffer[y][x][1]=1.0;
	if (B<=1.0)
		if (B>=0.0)
			framebuffer[y][x][2]=B;
		else
			framebuffer[y][x][2]=0.0;
	else
		framebuffer[y][x][2]=1.0;
}

// Sort Compare
bool compare_currentX (const struct edge &first, const struct edge& second)
{
  return (first.currentX < second.currentX);
}

void renderPolygon(){

	std::list<struct edge> activeEdgeList;
	int scanLine = minScreenY;
	cout << "Minscreen " << minScreenY << endl;
	cout << "Maxscreen " << maxScreenY << endl;
	while(scanLine <= maxScreenY){
		cout << "ScanLine " << scanLine << endl;
		for(int i=0; i<=currentEdge; i++){
			if(e[currentPolygon][i].minY == scanLine){
				cout << "Current edge pushed " << e[currentPolygon][i].minY << endl;
				activeEdgeList.push_back(e[currentPolygon][i]);
			}
		}
		activeEdgeList.sort(compare_currentX);
		cout << "Size of AEL " << activeEdgeList.size() << endl;

		for (std::list<struct edge>::iterator it = activeEdgeList.begin()
			; it != activeEdgeList.end(); ){
			
				struct edge tempEdge = *it;
				if(tempEdge.maxY == scanLine){
					it = activeEdgeList.erase(it);
				}else{
					++it;
				}
		
		}

		// TODO fill pixels
		int k = 0;
		double pointsX[10];
		for (std::list<struct edge>::iterator it = activeEdgeList.begin()
			; it != activeEdgeList.end(); ++it){

				struct edge tempEdge = *it;
				pointsX[k] = tempEdge.currentX;
				tempEdge.currentX = tempEdge.currentX + tempEdge.xIncr;
				*it = tempEdge;
				cout << "Slope " << tempEdge.currentX << " " << tempEdge.xIncr << endl;
				k++;
	
		}
		if(k & 1){
			k = k-1;
		}
		cout << "K " << k << endl;
		// Needs massive change
		for(int i=0; i <k; i=i+2){
			
			int fp;
			int lp;

			if(i%2 == 0){
				fp = ceil(pointsX[i]);
				if(i+1 < k){
					lp = floor(pointsX[i+1]);
				}else{
					cout << "Break Black Sheep" << endl;
					break;
				}
			}else{
				break;
			}

			if(fp == lp){
				cout << "Black Sheep" << endl;
				// don't draw the point
			}else{
				cout << "First point " << fp << "Last Point" << lp << endl;
				for(int l=(int) fp; l<(int)lp; l++){

					setFramebuffer(l,scanLine,col.r,col.g,col.b);
				}
			}


		}



		scanLine++;
	}

}

void display(void)
{
	// should not be necessary but some GPUs aren't displaying anything until a clear call.
	glClear (GL_COLOR_BUFFER_BIT);
}
void populateEdgeStruct(){
	
	int horizontalEdge = 0;
	int i;
	cout << "Current " << currentPoint << endl;
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
			cout << "Min " << e[currentPolygon][currentEdge].minY << endl;
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
		cout << "Min " << e[currentPolygon][currentEdge].minY << endl;
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

	for(int r =0; r<4; r++){
		cout << "Edge " << e[currentPolygon][r].currentX << " " 
			<< e[currentPolygon][r].maxY << " " << e[currentPolygon][r].xIncr << " " << e[currentPolygon][r].minY << endl;
	}
	renderPolygon();
	currentPolygon++;
	currentPoint = 0;
	currentEdge = 0;

	
}
void mouseClick(int button,int state,int x, int y){

	switch(button){
		
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
				p[currentPolygon][currentPoint].x = x;
				p[currentPolygon][currentPoint].y = y;
				setFramebuffer(x,y,col.r,col.g,col.b);
				drawit();
				glFlush();
				cout << "Points " << x << " " << y << endl;
				currentPoint++;
			}
			break;
		case GLUT_RIGHT_BUTTON:
			//currentPolygon++;
			//currentPoint = 0;
			//polygonCount++;
			if(state == GLUT_DOWN){
				p[currentPolygon][currentPoint].x = x;
				p[currentPolygon][currentPoint].y = y;
				setFramebuffer(x,y,col.r,col.g,col.b);
				drawit();
				glFlush();
				//currentPoint++;
				cout << "Points " << x << " " << y << endl;
				populateEdgeStruct();
				drawit();
				glFlush();
				col.r = ((double) rand() / (RAND_MAX));
				col.g = ((double) rand() / (RAND_MAX));
				col.b = ((double) rand() / (RAND_MAX));
				cout << "Colors " << col.r << " " << col.g << " " << col.b;
			}
			break;

	
	}

}

void init(void)
{
	gluOrtho2D ( 0, ImageW - 1, ImageH - 1, 0 );
	clearFramebuffer();
	col.r = 1.0;
	col.g = 1.0;
	col.b = 1.0;
}

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
	glutMainLoop();
	return 0;
}


