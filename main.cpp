#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx=0.0f, ly=0.0f, lz=-1.0f;
// XYZ position of the camera
float x=0.0f, y=0.0f, yy=0.0f, z=5.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0.0f;
float deltaY = 0.0f;
float deltaYY = 0.0f;

GLuint tardis_face_list;

void changeSize(int w, int h) {
  
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
  
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
  
	// Reset Matrix
	glLoadIdentity();
  
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
  
	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
  
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawSnowMan() {
  
	glColor3f(1.0f, 1.0f, 1.0f);
  
  // Draw Body
  
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(0.75f,20,20);
  
  // Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f,20,20);
  
  // Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();
  
  // Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f,0.5f,10,2);
}

void computePos(float deltaMove) {
  
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
  
  y += deltaY;
  yy += deltaY + deltaYY;
  deltaY = 0.0f;
  deltaYY = 0.0f;
}

void computeDir(float deltaAngle) {
  
	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}

void renderScene(void) {
  
	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);
  
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(	x, y, z,
            x+lx, yy,  z+lz,
            0.0f, 1.0f,  0.0f);
  
  // Draw ground
  
	glColor3f(0.4f, 0.4f, 0.4f);
	glBegin(GL_QUADS);
  glVertex3f(-100.0f, 0.0f, -100.0f);
  glVertex3f(-100.0f, 0.0f,  100.0f);
  glVertex3f( 100.0f, 0.0f,  100.0f);
  glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();
  
  // Draw 36 SnowMen
//  glScalef(3.0f, 3.0f, 3.0f);
  
	for(int i = -3; i < 3; i++)
		for(int j=-3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*10.0,0,j * 10.0);
			drawSnowMan();
			glPopMatrix();
		}

  glScalef(0.05f, 0.05f, 0.05f);
  
  //large top square on the top of the tardis
  //(behind police public call box square)
  glPushMatrix();
  
  glTranslatef(5.0f, 380.0f, 5.0f);
  glBegin(GL_QUAD_STRIP);

  glColor3ub(24, 47, 89);
  
  glVertex3f(240, 0, 0);
  glVertex3f(240, 40, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 40, 0);
  glVertex3f(0, 0, -240);
  glVertex3f(0, 40, -240);
  glVertex3f(240, 0, -240);
  glVertex3f(240, 40, -240);
  glVertex3f(240, 0, 0);
  glVertex3f(240, 40, 0);
  
  glEnd();
  
  glBegin(GL_QUADS);
  
  //top of this box
  glColor3ub(33, 71, 120);
  
  glVertex3f(240, 40, 0);
  glVertex3f(240, 40, -240);
  glVertex3f(0, 40, -240);
  glVertex3f(0, 40, 0);
  
  //bottom of this box
  glColor3ub(33, 71, 120);
  
  glVertex3f(240, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, -240);
  glVertex3f(240, 0, -240);
  
  //black back behind 'police box' text
  glColor3ub(0, 0, 0);
  glVertex3f(20, 35, 5.1f);
  glVertex3f(20, 7, 5.1f);
  glVertex3f(215, 7, 5.1f);
  glVertex3f(215, 35, 5.1f);
  
  glEnd();
  
  //very top boxes
  glTranslatef(15,40,-15);
  glColor3ub(22, 45, 87);


  glBegin(GL_QUAD_STRIP);

  glVertex3f(210, 0, 0);
  glVertex3f(210, 15, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 15, 0);

  glVertex3f(0, 0, -210);
  glVertex3f(0, 15, -210);

  glVertex3f(210, 0, -210);
  glVertex3f(210, 15, -210);

  glVertex3f(210, 0, 0);
  glVertex3f(210, 15, 0);
  glEnd();

  glBegin(GL_QUADS);
  
  //top of this box
  glColor3ub(33, 71, 120);
  
  glVertex3f(210, 15, 0);
  glVertex3f(210, 15, -210);
  glVertex3f(0, 15, -210);
  glVertex3f(0, 15, 0);
  glEnd();


  glTranslatef(10,15,-10);
  glBegin(GL_QUAD_STRIP);
  glColor3ub(22, 45, 87);

  glVertex3f(190, 0, 0);
  glVertex3f(190, 5, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 5, 0);

  glVertex3f(0, 0, -190);
  glVertex3f(0, 5, -190);

  glVertex3f(190, 0, -190);
  glVertex3f(190, 5, -190);

  glVertex3f(190, 0, 0);
  glVertex3f(190, 5, 0);
  glEnd();

  glBegin(GL_QUADS);
  
  //top of this box
  glColor3ub(33, 71, 120);
  
  glVertex3f(190, 5, 0);
  glVertex3f(190, 5, -190);
  glVertex3f(0, 5, -190);
  glVertex3f(0, 5, 0);
  glEnd();


  glPopMatrix();
  
  
  //main box rectangle
  glPushMatrix();
  
  glTranslatef(10.0f, 10.0f, 0.0f);
  glColor3ub(31, 61, 114);
  glBegin(GL_QUAD_STRIP);
  
  glVertex3f(230,0,0);//bottom right
  glVertex3f(230,370,0);//top right

  glVertex3f(0,0,0);//bottom left
  glVertex3f(0,370,0);//top left

  glColor3ub(32, 60, 105);
  
  glVertex3f(0,0,-230);
  glVertex3f(0,370,-230);

  glVertex3f(230,0,-230);
  glVertex3f(230,370,-230);

  glVertex3f(230,0,0);//bottom right
  glVertex3f(230,370,0);//top right
  
  glEnd();
  glPopMatrix();
  
    glColor3ub(40, 80, 132);
  //base platform rectangle
  glPushMatrix();
  
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  
  glVertex3f(245,0,5);
  glVertex3f(245,10,5);
  glVertex3f(5,0,5);
  glVertex3f(5,10,5);
  glVertex3f(5,0,-235);
  glVertex3f(5,10,-235);
  glVertex3f(245,0,-235);
  glVertex3f(245,10,-235);
  glVertex3f(245,0,5);
  glVertex3f(245,10,5);
  glEnd();
  
  
  //triangles around the base of the tardis
  glColor3ub(36, 67, 120);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(245,10,5);
  glVertex3f(240,15,0);
  
  glVertex3f(5,10,5);
  glVertex3f(10,15,0);
  
  glVertex3f(5,10,-235);
  glVertex3f(10,15,-230);

  glVertex3f(245,10,-235);
  glVertex3f(240,15,-230);

  glVertex3f(245,10,5);
  glVertex3f(240,15,0);
  
  glEnd();
  
  glPopMatrix();
  
  
  //front face
  glPushMatrix();
  glCallList(tardis_face_list);
  glPopMatrix();
  
  //left face
  glPushMatrix();
  glRotatef(-90.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(-240, 0, -10);
  glCallList(tardis_face_list);
  glPopMatrix();

  
  //right face
  glPushMatrix();
  glRotatef(90.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(-10, 0, 240);
  glCallList(tardis_face_list);
  glPopMatrix();

  //back face
  glPushMatrix();
  glRotatef(180.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(-250, 0, 240);
  glCallList(tardis_face_list);
  glPopMatrix();
  
	glutSwapBuffers();
}


void pressKey(int key, int xx, int yy) {
  
	switch (key) {
		case GLUT_KEY_LEFT : deltaYY = -0.2f; break;
		case GLUT_KEY_RIGHT : deltaYY = +0.2f; break;
		case GLUT_KEY_UP : deltaY = +5.0f; break;
		case GLUT_KEY_DOWN : deltaY = -5.0f; break;
    case GLUT_KEY_F1 : deltaY = +50.0f; break;
    case GLUT_KEY_F2 : deltaY = -50.0f; break;
	}
}

void releaseKey(int key, int x, int y) {
  
	switch (key) {
		case GLUT_KEY_LEFT : break;
		case GLUT_KEY_RIGHT : break;
		case GLUT_KEY_UP : break;
		case GLUT_KEY_DOWN : break;
	}
}

void keyboardFunc (unsigned char key, int x, int y) {
  switch (key) {
    case 033:
    case 'q':
      exit(1);
      break;

    case 'a' : deltaAngle = -0.01f; break;
		case 'd' : deltaAngle = 0.01f; break;
		case 'w' : deltaMove = 3.0f; break;
		case 's' : deltaMove = -3.0f; break;
      
    default:
      break;
  }
}

void releaseKeyboard (unsigned char key, int x, int y) {
  switch (key) {

		case 'a' :
		case 'd' : deltaAngle = 0.0f;break;
		case 'w' :
		case 's' : deltaMove = 0;break;
      
    default:
      break;
  }
}

static void tardis_face() {
  
  //panels inside main box
  
  glColor3ub(33, 71, 120);
  
  glTranslatef(45.0f, 50.0f, 1.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  glTranslatef(-100.0f, 75.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  glTranslatef(-100.0f, 75.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(62.5, 0, 0);
  glVertex3f(62.5, 60, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 60, 0);
  
  glEnd();
  
  //WindowPanes( will be divided later)
  glColor3ub(255, 255, 255);
  glTranslatef(-105.0f, 75.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(70, 0, 0);
  glVertex3f(70, 75, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 75, 0);
  glEnd();
  
  glTranslatef(102.5f, 0.0f, 0.0f);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(70, 0, 0);
  glVertex3f(70, 75, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 75, 0);
  glEnd();
  
  //Window Divider thingys
  glTranslatef(-102.5f, 0.0f, 1.0f);
  glColor3ub(40, 80, 132);
  glLineWidth(10);

  glBegin(GL_LINES);
  glVertex3f(70, 37.5, 0);
  glVertex3f(0, 37.5, 0);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(22.5, 75, 0);
  glVertex3f(22.5, 0, 0);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(50, 75, 0);
  glVertex3f(50, 0, 0);
  glEnd();
  
  //right window
  glTranslatef(102.0f, 0.0f, 0.0f);
  glColor3ub(40, 80, 132);
  
  glBegin(GL_LINES);
  glVertex3f(71, 37.5, 0);
  glVertex3f(0, 37.5, 0);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(22.5, 75, 0);
  glVertex3f(22.5, 0, 0);
  glEnd();
  
  glBegin(GL_LINES);
  glVertex3f(50, 75, 0);
  glVertex3f(50, 0, 0);
  glEnd();

}

// setup lists
void init() {
  tardis_face_list = glGenLists(1);
  glNewList(tardis_face_list, GL_COMPILE);
  tardis_face();
  glEndList();
}

int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,600);
	glutCreateWindow("TARDIS - Time and Relative Dimmension in Space");
  
  init(); //setup lists
  
	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
  
	glutSpecialFunc(pressKey);
  glutKeyboardFunc(keyboardFunc);
  glutKeyboardUpFunc(releaseKeyboard);
  
	// here are the new entries
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
  
	// OpenGL init
	glEnable(GL_DEPTH_TEST);
  
	// enter GLUT event processing cycle
	glutMainLoop();
  
	return 1;
}
