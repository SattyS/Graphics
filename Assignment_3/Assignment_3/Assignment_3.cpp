// Assignment_3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <GL/glut.h>
#include "assignment3.h"
#include "init.h"
#include <math.h>
#include <vector>
#include <iostream>
#include <queue>
#include <time.h>
#include <windows.h>

vector<Matrix> iat;

vector<Pt> cs;


float determinant2D(float a, float b, float c, float d){
	return ((a * d) - (b * c));
}

Matrix inverse(Matrix M1){
	
	float A = 0;
	A = M1.data[0][0] * determinant2D(M1.data[1][1], M1.data[1][2], M1.data[2][1], M1.data[2][2]) 
		- M1.data[0][1] * determinant2D(M1.data[1][0], M1.data[1][2], M1.data[2][0], M1.data[2][2])
		+ M1.data[0][2] * determinant2D(M1.data[1][0], M1.data[1][1], M1.data[2][0], M1.data[2][1]);

	cout << "A " << A << endl;
	Matrix rvalue;

	rvalue.data[0][0] = determinant2D(M1.data[1][1], M1.data[1][2], M1.data[2][1], M1.data[2][2]) * (1/A);
	rvalue.data[0][1] = determinant2D(M1.data[0][2], M1.data[0][1], M1.data[2][2], M1.data[2][1]) * (1/A);
	rvalue.data[0][2] = determinant2D(M1.data[0][1], M1.data[0][2], M1.data[1][1], M1.data[1][2]) * (1/A);

	rvalue.data[1][0] = determinant2D(M1.data[1][2], M1.data[1][0], M1.data[2][2], M1.data[2][0]) * (1/A);
	rvalue.data[1][1] = determinant2D(M1.data[0][0], M1.data[0][2], M1.data[2][0], M1.data[2][2]) * (1/A);
	rvalue.data[1][2] = determinant2D(M1.data[0][2], M1.data[0][0], M1.data[1][2], M1.data[1][0]) * (1/A);

	rvalue.data[2][0] = determinant2D(M1.data[1][0], M1.data[1][1], M1.data[2][0], M1.data[2][1]) * (1/A);
	rvalue.data[2][1] = determinant2D(M1.data[0][1], M1.data[0][0], M1.data[2][1], M1.data[2][0]) * (1/A);
	rvalue.data[2][2] = determinant2D(M1.data[0][0], M1.data[0][1], M1.data[1][0], M1.data[1][1]) * (1/A);

	return rvalue;

}
Matrix translate ( Vec v )
{
	Matrix rvalue;

	rvalue.data[0][0] = 1;
	rvalue.data[1][1] = 1;
	rvalue.data[2][2] = 1;

	rvalue.data[0][2] = v.x;
	rvalue.data[1][2] = v.y;

	return rvalue;
}

Matrix rotate ( Pt p, float theta )
{
	Matrix rvalue;

	rvalue.data[0][0] = cos(theta);
	rvalue.data[0][1] = -sin(theta);
	
	rvalue.data[1][0] = sin(theta);
	rvalue.data[1][1] = cos(theta);
	
	rvalue.data[0][2] = p.x + p.y * sin(theta) - p.x * cos(theta);
	rvalue.data[1][2] = p.y - p.y * cos(theta) - p.x * sin(theta);

	rvalue.data[2][2] = 1;

	return rvalue;
}

Matrix scale ( Pt p, float alpha )
{
	Matrix rvalue;

	rvalue.data[0][0] = alpha;
	rvalue.data[1][1] = alpha;

	rvalue.data[2][2] = 1;

	rvalue.data[0][2] = (1 - alpha) * p.x;
	rvalue.data[1][2] = (1 - alpha) * p.y;

	return rvalue;
}

Matrix nscale ( Pt p, Vec v, float alpha )
{
	Matrix rvalue;

	rvalue.data[0][0] = 1 + (alpha - 1) * pow(v.x,2);
	rvalue.data[0][1] = (alpha - 1) * v.x * v.y;
	rvalue.data[0][2] = v.x * (p.x * v.x + p.y * v.y) * (1 - alpha);
	
	rvalue.data[1][0] = (alpha - 1) * v.x * v.y;
	rvalue.data[1][1] = 1 + (alpha - 1) * pow(v.y,2);
	rvalue.data[1][2] = v.y * (p.x * v.x + p.y * v.y) * (1 - alpha);

	rvalue.data[2][2] = 1;
	
	return rvalue;
}

Matrix image ( Pt p1, Pt p2, Pt p3, Pt q1, Pt q2, Pt q3 )
{
	Matrix rvalue;
	Matrix M1,M2;
	
	M1.data[0][0] = p1.x;
	M1.data[0][1] = p2.x;
	M1.data[0][2] = p3.x;

	M1.data[1][0] = p1.y;
	M1.data[1][1] = p2.y;
	M1.data[1][2] = p3.y;

	M1.data[2][0] = 1;
	M1.data[2][1] = 1;
	M1.data[2][2] = 1;

	M2.data[0][0] = q1.x;
	M2.data[0][1] = q2.x;
	M2.data[0][2] = q3.x;

	M2.data[1][0] = q1.y;
	M2.data[1][1] = q2.y;
	M2.data[1][2] = q3.y;

	M2.data[2][0] = 1;
	M2.data[2][1] = 1;
	M2.data[2][2] = 1;

	M1 = inverse(M1);

	rvalue = compose(M2,M1);
	
	return rvalue;
}

Matrix compose ( Matrix m2, Matrix m1 )
{
	Matrix rvalue;
	float sum = 0;
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			for(int k=0; k<3; k++){
				sum += m2.data[i][k] * m1.data[k][j];
			}
			rvalue.data[i][j] = sum;
			sum = 0;
		}
	}
	return rvalue;
}

void setCondensationSet ( vector<Pt> pts )
{
	cs = pts;
}

void setIATTransformations ( vector<Matrix> transformations )
{
	iat = transformations;
}

// Draws the current IAT
void display ( void )
{
	glFlush();
	
}

void dispFractal(){
	glColor3f(1.0f,0.0f,0.0f); //blue color
	queue<vector<Matrix>> wp;
	vector<Matrix> initialPoints;
	//cout << iat.size();
	for(int i=0; i< 3; i++){
		Matrix temp;
		temp.data[0][0] = -1+2*((float)rand())/RAND_MAX;
		temp.data[1][0] = -1+2*((float)rand())/RAND_MAX;
		//cout << temp.data[0][0] << " " << temp.data[1][0];
		temp.data[2][0] = 1.0f;
		//glBegin(GL_LINE_LOOP);
		//glVertex3f(temp.data[0][0],temp.data[1][0],0.0f);
		initialPoints.push_back(temp);
	}
	wp.push(initialPoints);
	vector<Matrix> cp;
	for(int i=0; i<cs.size(); i++){
		Matrix t;
		t.data[0][0] = cs.at(i).x;
		t.data[1][0] = cs.at(i).y;
		t.data[2][0] = 1.0f;
		cp.push_back(t);
	}
	//cout << "CS size " << cp.size() << endl;
	//glEnd();
	//glFlush();
	int levelCount;
	levelCount = (iat.size() > 3) ? 5 : 8;
	for(int level = 0; level < levelCount; level++){
		int powCond = pow((float)iat.size(), level);
		//cout << powCond << endl;
		powCond = wp.size();
		cout << "Level check " << wp.size() << endl;
		for(int perLevel = 0; perLevel < powCond; perLevel++){
			//cout << "q size " << wp.size() << endl;
			vector<Matrix> tempVec = wp.front();
			wp.pop();
			for(int iatIterator=0; iatIterator < iat.size(); iatIterator++){
				vector<Matrix> newVec;
				for(int npoints=0; npoints < tempVec.size(); npoints++){
					Matrix tempMat = tempVec.at(npoints);
					tempMat = compose(iat.at(iatIterator), tempMat);
					newVec.push_back(tempMat);
				}
				wp.push(newVec);
			}	
			if(cp.size() > 0){
				wp.push(cp);
				//cout << "CS check " << wp.size() << endl;
			}
		}
	}
	cout << wp.size() << endl;
	for(int i=0; i< wp.size(); i++){
		if(cp.size() > 0){
			glBegin(GL_LINE_LOOP);
		}else{
			glBegin(GL_POINTS);
		}
		vector<Matrix> tempV = wp.front();
		for(int k=0; k<tempV.size(); k++){
			//cout << i << endl;
			//cout << tempV.at(k).data[0][0] << ":" << tempV.at(k).data[1][0] << endl;
			glVertex3f(tempV.at(k).data[0][0],tempV.at(k).data[1][0],0.0f);
		}
		wp.pop();
		glEnd();
		glFlush();
		if(cp.size() > 0){
			if(i == (wp.size() - 1)){
				break;
			}
		}
	}
	if(cs.size() == 1){
		glBegin(GL_POINTS);
		glVertex3f(cs.at(0).x,cs.at(0).y,0.0f);
		glEnd();
		glFlush();
	}else if(cs.size() == 2){
		glBegin(GL_LINES);
		glVertex3f(cs.at(0).x,cs.at(0).y,0.0f);
		glVertex3f(cs.at(1).x,cs.at(1).y,0.0f);
		glEnd();
		glFlush();
	}else{
		glBegin(GL_LINE_LOOP);
		for(int i=0; i<cs.size(); i++){
			glVertex3f(cs.at(i).x,cs.at(i).y,0.0f);
		}
		glEnd();
		glFlush();
	}
}	
/* do not modify the reshape function */
void reshape ( int width, int height )
{
	glViewport ( 0, 0, width, height );
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ( );    
	gluOrtho2D (-1, 1, -1, 1);
	glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );
}
// Keyboard callback
void keyboard(unsigned char key, int x, int y){

	switch(key){
	
		case '1':
			dispFractal();
			break;
	}

}
int main ( int argc, char** argv )
{
	glutInit ( &argc, argv );
	glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGB );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition ( 100, 100 );
	glutCreateWindow ( "Sathish Sekar - Homework 3" );
	srand(time(NULL));
	init();	
	/*
	Matrix test;
	test.data[0][0] = 1;
	test.data[0][1] = 2;
	test.data[0][2] = 3;

	test.data[1][0] = 3;
	test.data[1][1] = 2;
	test.data[1][2] = 1;

	test.data[2][0] = 2;
	test.data[2][1] = 1;
	test.data[2][2] = 3;

	test = inverse(test);

	for(int i=0;i < 3; i++){
		for(int j=0; j < 3; j++){
			cout << test.data[i][j] << " ";
		}
		cout << endl;
	}
	*/
	glutDisplayFunc ( display );
	glutReshapeFunc ( reshape );
	glutKeyboardFunc(keyboard);
	glutMainLoop ( );
	return 0;
}


