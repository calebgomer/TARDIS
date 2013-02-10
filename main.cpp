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

bool wireToggle;
int rotate_offset = 0;
const int num_triforces = 1000;
GLfloat triforces[num_triforces][3];

GLuint tardis_face_list;
GLuint tardis_window_list;
GLuint tardis_panel_list;
GLuint tardis_list;
GLuint triforce_list;

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


void triforce_bomb() {
  rotate_offset++;
  
  glPushMatrix();
  
  for (int i = 0; i < 1000; i++) {
    glPushMatrix();
    
    //randomize color
    int r = (int)abs(triforces[i][0]*2.56)%256;
    int g = (int)abs(triforces[i][1]*2.56)%256;
    int b = (int)abs(triforces[i][2]*2.56)%256;
    glColor3ub(r, g, b);
    
    //randomize position
    glTranslatef(triforces[i][0], triforces[i][1], triforces[i][2]);
    
    //randomize rotation
    glRotatef(((int)triforces[i][0])%360, ((int)triforces[i][0])%2,((int)triforces[i][0])%2,((int)triforces[i][0])%2);
    glRotatef(((int)triforces[i][0])%360+rotate_offset, 0, 1, 0);
    
    //randomize scale
    float x = ((int)abs(triforces[i][0])%100)/50.0;
    glScalef(x,x,x);
    
    //draw the triforce
    glCallList(triforce_list);
    
    glPopMatrix();
  }
  
  glPopMatrix();
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
	glColor3ub(0, 104, 10);
	glBegin(GL_QUADS);
  glVertex3f(-100.0f, -0.1f, -100.0f);
  glVertex3f(-100.0f, -0.1f,  100.0f);
  glVertex3f( 100.0f, -0.1f,  100.0f);
  glVertex3f( 100.0f, -0.1f, -100.0f);
	glEnd();
  
  //TRIFORCE-BOMB
  triforce_bomb();
  
//  glRotatef((rotate_offset%360)*2, 0, 1, 0);
  
  //TARDIS
  glScalef(0.05f, 0.05f, 0.05f);
  glTranslatef(-125, 0, 125);
  
 glCallList(tardis_list);
  
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
   //toggle wireframes
	  case 't':
      glPolygonMode(GL_FRONT_AND_BACK, (wireToggle)?GL_LINE:GL_FILL);
      wireToggle = !wireToggle;
		  break;
		case 'a' :
		case 'd' : deltaAngle = 0.0f;break;
		case 'w' :
		case 's' : deltaMove = 0;break;
      
    default:
      break;
  }
}

static void triforce() {
  
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  
  glPushMatrix();
  
  glTranslatef(-2, 0, -0.175);
  
  glBegin(GL_TRIANGLES); //make both triforce faces
  
  glVertex3f(1, 1, 0);
  glVertex3f(2, 0, 0);
  glVertex3f(0, 0, 0);
  
  glVertex3f(2, 2, 0);
  glVertex3f(3, 1, 0);
  glVertex3f(1, 1, 0);
  
  glVertex3f(3, 1, 0);
  glVertex3f(4, 0, 0);
  glVertex3f(2, 0, 0);
  
  glVertex3f(0, 0, 0.35);
  glVertex3f(2, 0, 0.35);
  glVertex3f(1, 1, 0.35);
  
  glVertex3f(1, 1, 0.35);
  glVertex3f(3, 1, 0.35);
  glVertex3f(2, 2, 0.35);
  
  glVertex3f(2, 0, 0.35);
  glVertex3f(4, 0, 0.35);
  glVertex3f(3, 1, 0.35);
  
  glEnd(); //end triforce faces
  
  glColor3ub(50, 50, 50);
  
  glBegin(GL_QUADS); //panels
  
  //make side panels
  //left panel
  glVertex3f(2, 2, 0.35);
  glVertex3f(2, 2, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 0.35);
  //right panel
  glVertex3f(4, 0, 0.35);
  glVertex3f(4, 0, 0);
  glVertex3f(2, 2, 0);
  glVertex3f(2, 2, 0.35);
  //bottom panel
  glVertex3f(0, 0, 0.35);
  glVertex3f(0, 0, 0);
  glVertex3f(4, 0, 0);
  glVertex3f(4, 0, 0.35);
  
  //make inside panels
  //left panel
  glVertex3f(2, 0, 0.35);
  glVertex3f(2, 0, 0);
  glVertex3f(1, 1, 0);
  glVertex3f(1, 1, 0.35);
  //right panel
  glVertex3f(3, 1, 0.35);
  glVertex3f(3, 1, 0);
  glVertex3f(2, 0, 0);
  glVertex3f(2, 0, 0.35);
  //top panel
  glVertex3f(3, 1, 0.35);
  glVertex3f(1, 1, 0.35);
  glVertex3f(1, 1, 0);
  glVertex3f(3, 1, 0);
  
  glEnd();
  
  glPopMatrix();
  
  glDisable(GL_CULL_FACE);
}

static void tardis_panel(){
	glBegin(GL_QUADS);
 
	glVertex3f(0, 0, 0);
	glVertex3f(0, 60, 0);  
	glVertex3f(62.5, 60, 0);
	glVertex3f(62.5, 0, 0); 
	glEnd();
}

static void tardis_window(){
	glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex3f(70, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 75, 0);
    glVertex3f(70, 75, 0);
    glEnd();

    glTranslatef(0, 0, 0.1f);
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
  glLineWidth(1);
}

static void tardis_face() {
  
  //panels inside main box
  
  glColor3ub(33, 71, 120);
  
  glTranslatef(45.0f, 50.0f, 1.0f);
  glCallList(tardis_panel_list);
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glCallList(tardis_panel_list);
  
  glTranslatef(-100.0f, 75.0f, 0.0f);
  glCallList(tardis_panel_list);
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glCallList(tardis_panel_list);
  
  glTranslatef(-100.0f, 75.0f, 0.0f);
  glCallList(tardis_panel_list);
  
  glTranslatef(100.0f, 0.0f, 0.0f);
  glCallList(tardis_panel_list);
  
  //WindowPanes
  
  glTranslatef(-105.0f, 75.0f, 0.0f);
  glCallList(tardis_window_list);

  glTranslatef(102.5f, 0.0f, 0.0f);
  glCallList(tardis_window_list);
}

static void tardis(){
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
  glTranslatef(-250, 0, 230);
  glCallList(tardis_face_list);
  glPopMatrix();
}

// setup lists
void init() {
tardis_panel_list = glGenLists(1);
  glNewList(tardis_panel_list,GL_COMPILE);
  tardis_panel();
  glEndList();

  tardis_window_list = glGenLists(1);
  glNewList(tardis_window_list,GL_COMPILE);
  tardis_window();
  glEndList();

  tardis_face_list = glGenLists(1);
  glNewList(tardis_face_list, GL_COMPILE);
  tardis_face();
  glEndList();
  
  tardis_list = glGenLists(1);
  glNewList(tardis_list,GL_COMPILE);
  tardis();
  glEndList();

  
  triforce_list = glGenLists(1);
  glNewList(triforce_list, GL_COMPILE);
  triforce();
  glEndList();
}

int main(int argc, char **argv) {
	wireToggle = true;
  for (int i = 0; i < num_triforces; i++) {
    triforces[i][0]= rand()%200-100;
    triforces[i][1]= rand()%100;
    triforces[i][2]= rand()%200-100;
  }
  
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1280,800);
	glutCreateWindow("TATARDIS - Triforces and Time and Relative Dimmension in Space");
  
  glPolygonMode (GL_FRONT, GL_FILL);
  glPolygonMode (GL_BACK, GL_LINE);
  
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