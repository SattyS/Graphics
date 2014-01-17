#include "stdafx.h"
#include <gl/glut.h>
#include <gl/gl.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PI 3.14159265

static float size = 1;
static int brushType = 0;
static float theta = 0;
static int red = 0;
static int green = 0;
static int blue = 0;

void display ( void )
{
	/*
	glClear ( GL_COLOR_BUFFER_BIT );
	glColor3f ( 1, 1, 1 );
	glBegin ( GL_LINES );
		glVertex2f ( 0, 0 );
		glVertex2f ( WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1 );
	glEnd ( );
	glColor3f ( 1, 0, 0 );
	glBegin ( GL_QUADS );
		glVertex2f ( 0.1 * WINDOW_WIDTH, 0.1 * WINDOW_HEIGHT );
		glVertex2f ( 0.9 * WINDOW_WIDTH, 0.1 * WINDOW_HEIGHT );
		glVertex2f ( 0.9 * WINDOW_WIDTH, 0.9 * WINDOW_HEIGHT );
		glVertex2f ( 0.1 * WINDOW_WIDTH, 0.9 * WINDOW_HEIGHT );
	glEnd ( );
	glFlush ( );

	*/
}


void init ( void )
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WINDOW_WIDTH-1, WINDOW_HEIGHT-1, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}
void changeBrushColor(int colorCode){
	red = colorCode & 4;
	green = colorCode & 2;
	blue = colorCode & 1;
	if((brushType % 4 == 3) && glIsEnabled(GL_BLEND)){
		glColor4f (blue, green, red,0.5);
	}else{
		glColor3f (blue, green, red);
	}
}
void keyboard ( unsigned char key, int x, int y )
{
	int keyI = key - 48;
	if(keyI > 0 && keyI < 8){
		changeBrushColor(keyI);
	}else{
		switch (key)
		{
			case '+':
				if(size < 128)
				size = size * 2;
				if(size > 128)
				size = 128;
				break;
			case 'c':
				glClearColor (0.0, 0.0, 0.0, 0.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glFlush();
				break;
			case '-':
				if(size > 0)
				size = size / 2;
				if(size < 1)
				size = 1;
				break;
			case 'b':
				glDisable(GL_BLEND);
				brushType++;
				break;
			case 'r':
				theta = theta + 10 * PI/180;
				break;
			case 'a':
				if(glIsEnabled(GL_BLEND)){
					glDisable(GL_BLEND);
				}else{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glShadeModel(GL_FLAT);
					glColor4f(blue, green, red,0.5);
				}
				break;
			default:
				break;
		}
		glutPostRedisplay( );
	}
}
void motionMove(int x, int y){
	int choice = brushType % 4;
	switch(choice){
		//p'x = cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox
		//p'y = sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy
		case 0:
			//glRectf(x-size,y-size, x+size, y+size);
			glBegin(GL_QUADS);
			glVertex3f(cos(theta) * (-size) - sin(theta) * size + x,
				sin(theta) * (-size) + cos(theta) * size + y,0);
			glVertex3f(cos(theta) * (size) - sin(theta) * size + x,
				sin(theta) * (size) + cos(theta) * size + y,0);
			glVertex3f(cos(theta) * (size) - sin(theta) * (-size) + x,
				sin(theta) * (size) + cos(theta) * (-size) + y,0);
			glVertex3f(cos(theta) * (-size) - sin(theta) * (-size) + x,
				sin(theta) * (-size) + cos(theta) * (-size) + y,0);
			glEnd();
			break;
		case 1:
			glBegin(GL_TRIANGLES);
			//glVertex3f(x-size, y-size, 0);
			glVertex3f(cos(theta) * (-size) - sin(theta) * (-size) + x
				, sin(theta) * (-size) + cos(theta) * (-size) + y, 0);
			//glVertex3f(x+size, y-size, 0);
			glVertex3f(cos(theta) * (size) - sin(theta) * (-size) + x,
				sin(theta) * (size) + cos(theta) * (-size) + y, 0);
			//glVertex3f(x, y+size, 0);
			glVertex3f(- sin(theta) * (size) + x,
				cos(theta) * (size) + y, 0);
			glEnd();
			break;
		case 2:
			glBegin(GL_LINES);
			glVertex2f(cos(theta) - (-size) * sin(theta) + x,sin(theta) + (-size) * cos(theta) + y);
			glVertex2f(cos(theta) - (size) * sin(theta) + x,sin(theta) + (size) * cos(theta) + y);
			glEnd();
			break;
		case 3:
			float angle = 0;
			int i=0;
			/*for(angle=0; angle<=360; angle=angle+1){
				glVertex2f(x + sin(angle) * size, y + cos(angle) * size);
			}
			glEnd();
			*/
			int noTriangle = 30;
			
			float twoPi = 2.0 * PI;
			
			glBegin(GL_TRIANGLE_FAN);
				glVertex2f(x, y);
				for(i = 0; i <= noTriangle;i++) { 
					glVertex2f(
							x + (size * cos(i *  twoPi / noTriangle)), 
						y + (size * sin(i * twoPi / noTriangle))
					);
				}
			glEnd();
			break;
	}
	
	glFlush ( );

}

void reshape(int w, int h){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, w-1, h-1, 0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

int main (int argc, char *argv[])
{
	glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT); 
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Sathish Sekar - Assignment 1");
    init();
    glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutMotionFunc(motionMove);
	glutMainLoop();
	return 0;
}