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

//Iterated Affine transformation matrices contained in a vector
vector<Matrix> iat;

// Condensation sets as points contained in a vector
vector<Pt> cs;

// Calculate determinant of 2*2 matrix
float determinant2D(float a, float b, float c, float d){
	return ((a * d) - (b * c));
}
// Calculates inverse of a 3*3 matrix
Matrix inverse(Matrix M1){
	
	float A = 0;
	A = M1.data[0][0] * determinant2D(M1.data[1][1], M1.data[1][2], M1.data[2][1], M1.data[2][2]) 
		- M1.data[0][1] * determinant2D(M1.data[1][0], M1.data[1][2], M1.data[2][0], M1.data[2][2])
		+ M1.data[0][2] * determinant2D(M1.data[1][0], M1.data[1][1], M1.data[2][0], M1.data[2][1]);

	//cout << "A " << A << endl;

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
// Translate
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
// Rotate
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
// Uniform scale
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
// Non-uniform scale
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
// Image - Best function ever written
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
// Multiplies 2 3*3 matrices
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
// Set condensation set
void setCondensationSet ( vector<Pt> pts )
{
	cs = pts;
}
// Set IAT 
void setIATTransformations ( vector<Matrix> transformations )
{
	iat = transformations;
}
// Function to display the fractal once iat and cs if any are set
void dispFractal(){
	// Queue that stores the final points that are to be rendered as fractal points
	queue<vector<Matrix>> wp;
	vector<Matrix> initialPoints;
	// 5 Random initial points to start with
	for(int i=0; i< 5; i++){
		Matrix temp;
		temp.data[0][0] = -1+2*((float)rand())/RAND_MAX;
		temp.data[1][0] = -1+2*((float)rand())/RAND_MAX;
		temp.data[2][0] = 1.0f;
		initialPoints.push_back(temp);
	}
	wp.push(initialPoints);
	// Storing condensation set points as matrices as I've considered everything as 3*3 matrix
	vector<Matrix> cp;
	for(int i=0; i<cs.size(); i++){
		Matrix t;
		t.data[0][0] = cs.at(i).x;
		t.data[1][0] = cs.at(i).y;
		t.data[2][0] = 1.0f;
		cp.push_back(t);
	}
	int levelCount;
	// Fractals with IAT > 3 will have a tree of depth 5, else it will be 10
	levelCount = (iat.size() > 3) ? 5 : 10;
	for(int level = 0; level < levelCount; level++){
		int powCond;
		powCond = wp.size();
		// Breadth first method
		for(int perLevel = 0; perLevel < powCond; perLevel++){
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
			}
		}
	}
	// Display stored points
	for(int i=0; i< wp.size(); i++){
		// Fractals with CS are displayed using line loop
		if(cp.size() > 0){
			glBegin(GL_LINE_LOOP);
		}else{
			glBegin(GL_POINTS);
		}
		// just changing R,G,B colors alternatively
		if(i % 3 == 0){
			glColor3f(1.0f,0.0f,0.0f);
		}else if(i % 3 == 1){
			glColor3f(0.0f,1.0f,0.0f);
		}else{
			glColor3f(0.0f,0.0f,1.0f);
		}
		vector<Matrix> tempV = wp.front();
		for(int k=0; k<tempV.size(); k++){
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
	// Display the CS accordingly
	if(cs.size() == 1){
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_POINTS);
		glVertex3f(cs.at(0).x,cs.at(0).y,0.0f);
		glEnd();
		glFlush();
	}else if(cs.size() == 2){
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_LINES);
		glVertex3f(cs.at(0).x,cs.at(0).y,0.0f);
		glVertex3f(cs.at(1).x,cs.at(1).y,0.0f);
		glEnd();
		glFlush();
	}else{
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_LINE_LOOP);
		for(int i=0; i<cs.size(); i++){
			glVertex3f(cs.at(i).x,cs.at(i).y,0.0f);
		}
		glEnd();
		glFlush();
	}
}
// Draws the current IAT
void display ( void )
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!iat.empty()){
		cout << "IAT size " << iat.size() << endl;
		cout << "Fractal with IAT size > 3 will run for 5 iterations and others will run for 10 iterations " << endl;
		cout << "Loading..." << endl;
		dispFractal();
	}
	glFlush();
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
// Clear the IAT and CS vectors
void clearIAT(){
	while(!iat.empty()){
		iat.pop_back();
	}
	while(!cs.empty()){
		cs.pop_back();
	}
}
// Keyboard callback
void keyboard(unsigned char key, int x, int y){

	switch(key){
		// Used image function and transformation functions alternatively, according to which one is easy
		case '1':
			clearIAT();
			iat.push_back( image(Pt(-0.45,0), Pt(0.45,0), Pt(0.45,0.9), Pt(-0.45,0), Pt(0,0), Pt(0,0.45)) );
			iat.push_back( image(Pt(-0.45,0), Pt(0.45,0), Pt(0.45,0.9), Pt(0,0), Pt(0.45,0), Pt(0.45,0.45)) );
			iat.push_back( image(Pt(-0.45,0), Pt(0.45,0), Pt(0.45,0.9), Pt(0.45,0.45), Pt(0.45,0.9), Pt(0,0.9)) );
			break;
		case '2':
			clearIAT();
			iat.push_back ( compose(scale( Pt(0,0.45) , 0.5), translate(Vec(0,0.45)) ));
			iat.push_back ( compose(scale( Pt(0.45,0) , 0.5), translate(Vec(0.45,0)) ));
			cs.push_back(Pt(0,0));
			cs.push_back(Pt(0.45,0));
			cs.push_back(Pt(0.45,0.45));
			cs.push_back(Pt(0,0.45));
			break;
		case '3':
			clearIAT();
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(0,0.3268), Pt(-0.1718,0.2020), Pt(0.1718,0.2020)) );
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(-0.278,0.1248), Pt(-0.45,0), Pt(-0.1062,0)) );
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(0.278,0.1248), Pt(0.1062,0), Pt(0.45,0)) );
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(-0.1718,-0.2020), Pt(-0.3437,-0.3268), Pt(0,-0.3268)) );
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(0.1718,-0.2020), Pt(0,-0.3268), Pt(0.3437,-0.3268)) );
			iat.push_back( image(Pt(0,0.3268), Pt(-0.45,0), Pt(0.45,0), Pt(0,-0.3268), Pt(0.1718,-0.2020), Pt(-0.1718,-0.2020)) );
			break;
		case '4':
			clearIAT();
			iat.push_back ( scale( Pt(-.9, 0), 0.33) );
			iat.push_back ( scale( Pt (-.45, -.7794), 0.33 ) );
			iat.push_back ( scale( Pt (.45, -.7794), 0.33 ) );
			iat.push_back ( scale( Pt (.9, 0), 0.33) );
			iat.push_back ( scale( Pt (.45, .7794), 0.33 ) );
			iat.push_back ( scale( Pt (-.45, .7794), 0.33 ) );
			break;
		// Square Carpet
		case '5':
			clearIAT();
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(-.9,.9), Pt(-0.3,0.9), Pt(-.3,.3)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(-0.3,0.9), Pt(0.3,0.9), Pt(0.3,0.3)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(0.3,0.9), Pt(0.9,0.9), Pt(0.9,0.3)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(-.9,0.3), Pt(-0.3,0.3), Pt(-0.3,-0.3)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(0.3,0.3), Pt(0.9,0.3), Pt(0.9,-0.3)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(-0.9,-0.3), Pt(-0.3,-0.3), Pt(-0.3,-0.9)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(-0.3,-0.3), Pt(0.3,-0.3), Pt(0.3,-.9)) );
			iat.push_back( image(Pt(-.9,.9), Pt(.9,.9), Pt(.9,-.9), Pt(0.3,-0.3), Pt(0.9,-0.3), Pt(0.9,-0.9)) );
			// Partial snow flake image given in class, decided to go with carpet instead
			/*
				iat.push_back( image(Pt(-0.9,0), Pt(0,0.45), Pt(0.9,0), Pt(-0.9,0), Pt(-0.6,0.15), Pt(-0.3,0)) );
				iat.push_back( image(Pt(-0.9,0), Pt(0,0.45), Pt(0.9,0), Pt(-0.3,0), Pt(-0.3,0.3), Pt(0,0.45)) );
				iat.push_back( image(Pt(-0.9,0), Pt(0,0.45), Pt(0.9,0), Pt(0,0.45), Pt(0.3,0.3), Pt(0.3,0)) );
				iat.push_back( image(Pt(-0.9,0), Pt(0,0.45), Pt(0.9,0), Pt(0.3,0), Pt(0.6,0.15), Pt(0.9,0)) );
			*/
			break;
		// Tree
		case '6':
			clearIAT();
			iat.push_back ( compose(rotate (Pt(-0.225,-0.45) , 0.7853), compose(scale( Pt(-0.225,-0.45) , 0.5 * 1.414), translate(Vec(0,0.45)) )));
			iat.push_back ( compose(rotate (Pt(0.225,-0.45) , -0.7853), compose(scale( Pt(0.225,-0.45) , 0.5 * 1.414), translate(Vec(0,0.45)) )));
			cs.push_back(Pt(-0.225,-0.9));
			cs.push_back(Pt(0.225,-0.9));
			cs.push_back(Pt(0.225,-0.45));
			cs.push_back(Pt(-0.225,-0.45));
			break;
	}
	glutPostRedisplay();

}
//Main
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
	glutDisplayFunc ( display );
	glutReshapeFunc ( reshape );
	glutKeyboardFunc(keyboard);
	glutMainLoop ( );
	return 0;
}

