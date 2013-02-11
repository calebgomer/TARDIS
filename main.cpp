#include <cmath>
#include <stdlib.h>
#include <stdio.h>  
#include <stdarg.h> 
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include "SOIL.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
using namespace std;

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
int rotate_offset = 0;
bool draw_wireframes = false;
bool show_more_tardises = false;
bool show_triforces = false;
bool rotate_everything = false;
int camera_mode = 0;

//triforce light thingy
GLUquadric* qobj;

//tardis lists
GLuint tardis_face_list, tardis_window_list, tardis_panel_list, tardis_blackpanel_list, tardis_list;

//triforce stuff
GLuint triforce_list;
GLuint triangle_list;
const int num_triforces = 10000;
const int num_tardises = 10;
GLfloat triforces[num_triforces][3];

//textures
GLuint ground_texture;
GLuint dw_texture;


/**print a word*/
static void printw(string word){
	for(int i=0; i < word.length();i++){
		glutStrokeCharacter(GLUT_STROKE_ROMAN, word[i]);
	}

}
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

static GLuint LoadPNG(char* filename)
{
  GLuint texture = SOIL_load_OGL_texture
  (
   filename,
   SOIL_LOAD_AUTO,
   SOIL_CREATE_NEW_ID,
   SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
   );
  
  if (texture == 0)
    printf("Texture Load Error: %s", filename);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  return texture;
}


void triforce_bomb() {
  if (!show_triforces)
    return;
  
  glPushMatrix();
  
  for (int i = num_tardises; i < num_triforces; i++) {
    glPushMatrix();
    
    //randomize color
    int r = min(255,max(0,((int)abs(triforces[i][0]*2.56)%256+abs(((rotate_offset%50)-25)))));
    int g = min(255,max(0,((int)abs(triforces[i][1]*2.56)%256 + abs(((rotate_offset%50)-25)))));
    int b = min(255,max(0,((int)abs(triforces[i][2]*2.56)%256 + abs(((rotate_offset%50)-25)))));
    glColor3ub(r, g, b);
    
    //randomize position
    glTranslatef(triforces[i][0], triforces[i][1], triforces[i][2]);
    
    //randomize rotation
    glRotatef(((int)triforces[i][0])%360, ((int)triforces[i][0])%2,((int)triforces[i][0])%2,((int)triforces[i][0])%2);
    glRotatef(((int)triforces[i][0])%360+rotate_offset, 0, 1, 0);
    
    //randomize scale
    float x = (50-((int)abs(triforces[i][0])%100))/50.0;
    glScalef(x,x,x);
    
    //draw the triforce
    glCallList(triforce_list);

    glScalef(1, 1, 1);
    
    glPopMatrix();
  }
  
  glPopMatrix();
}

void tardis_bomb() {
  if (!show_more_tardises)
    return;
  glPushMatrix();
  
  for (int i = 0; i < num_tardises; i++) {
    glPushMatrix();
    
    //randomize color
    int r = min(255,max(0,((int)abs(triforces[i][0]*2.56)%256+abs(((rotate_offset%50)-25)))));
    int g = min(255,max(0,((int)abs(triforces[i][1]*2.56)%256 + abs(((rotate_offset%50)-25)))));
    int b = min(255,max(0,((int)abs(triforces[i][2]*2.56)%256 + abs(((rotate_offset%50)-25)))));
    glColor3ub(r, g, b);
    
    //randomize position
    glTranslatef(triforces[i][0], triforces[i][1], triforces[i][2]);
    
    //randomize rotation
    glRotatef(((int)triforces[i][0])%360, ((int)triforces[i][0])%2,((int)triforces[i][0])%2,((int)triforces[i][0])%2);
    glRotatef(((int)triforces[i][0])%360+rotate_offset, 0, 1, 0);
    
    //randomize scale
    glScalef(0.05, 0.05, 0.05);
    
    //draw the tardis
    glTranslatef(-125, 0, 125);
    glCallList(tardis_list);
    
    glScalef(1, 1, 1);
    
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
  if (rotate_everything)
    rotate_offset++;
  
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	// Reset transformations
	glLoadIdentity();
	// Set the camera
  switch (camera_mode) {
    case 0:
      gluLookAt(x, y, z, x+lx, yy,  z+lz, 0.0f, 1.0f,  0.0f);
      break;
    case 1:
      gluLookAt(0, 0, 40, 0, 18, 0, 0, 1, 0);
      break;
    case 2:
      gluLookAt(-50, 30, 40, 0, 15, 0, 0, 1, 0);
      break;
    case 3:
      gluLookAt(0, 80, -30, 0, 0, 0, 0, 0, 1);
      break;
    case 4:
      gluLookAt(0, 40, 0, 20, 50, 20, 0, 1, 0);
      break;
  }
  
  
  glPolygonMode (GL_BACK, GL_LINE);
  
  // Draw ground
  glEnable(GL_TEXTURE_2D);
	glColor3ub(255, 255, 255);
  glBindTexture(GL_TEXTURE_2D, ground_texture);
	glBegin(GL_QUADS);
  glTexCoord3f(0,0,0); glVertex3f(-100.0f, -0.1f, -100.0f);
  glTexCoord3f(1,0,0); glVertex3f(-100.0f, -0.1f,  100.0f);
  glTexCoord3f(1,1,0); glVertex3f( 100.0f, -0.1f,  100.0f);
  glTexCoord3f(0,1,0); glVertex3f( 100.0f, -0.1f, -100.0f);
	glEnd();
  glDisable(GL_TEXTURE_2D);

  
  //TRIFORCE-BOMB
  triforce_bomb();
  
  //FLOATING TARDISES
  tardis_bomb();
  
  /***rotate***/
  glRotatef((rotate_offset%240)*1.5, 0, 1, 0);
  
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
      
    case 'a' : deltaAngle = -0.01f; camera_mode = 0; break;
		case 'd' : deltaAngle = 0.01f; camera_mode = 0; break;
		case 'w' : deltaMove = 3.0f; camera_mode = 0; break;
		case 's' : deltaMove = -3.0f; camera_mode = 0; break;
      
    default:
      break;
  }
}

/******************************************
 * Release Keyboard
 * One of three triangles in a triforce
 *****************************************/
void releaseKeyboard (unsigned char key, int x, int y) {
  switch (key) {
   //toggle wireframes
	  case 't':
      glPolygonMode(GL_FRONT, (draw_wireframes)?GL_LINE:GL_FILL);
      draw_wireframes = !draw_wireframes;
		  break;
    case 'x':
      show_more_tardises = !show_more_tardises;
      break;
    case 'c':
      show_triforces = !show_triforces;
      break;
    case 'r':
      rotate_everything = !rotate_everything;
      break;
    case '1':
      camera_mode = 1;
      break;
    case '2':
      camera_mode = 2;
      break;
    case '3':
      camera_mode = 3;
      break;
    case '4':
      camera_mode = 4;
      break;
		case 'a' :
		case 'd' : deltaAngle = 0.0f;break;
		case 'w' :
		case 's' : deltaMove = 0;break;
      
    default:
      break;
  }
}



/******************************************
 * Triangle
 * One of three triangles in a triforce
 *****************************************/
static void triangle(float thickness) {
  glPushMatrix();
  
  glTranslatef(-2, 0, -0.175);
  
  glBegin(GL_TRIANGLES);
  glVertex3f(1, 1, 0); //front
  glVertex3f(2, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, thickness); //back
  glVertex3f(2, 0, thickness);
  glVertex3f(1, 1, thickness);
  glEnd(); //end triangles
  
  glBegin(GL_QUADS);
  glVertex3f(1, 1, thickness); //left panel
  glVertex3f(1, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, thickness);
  glVertex3f(2, 0, thickness); //right panel
  glVertex3f(2, 0, 0);
  glVertex3f(1, 1, 0);
  glVertex3f(1, 1, thickness);
  glVertex3f(0, 0, thickness); //bottom panel
  glVertex3f(0, 0, 0);
  glVertex3f(2, 0, 0);
  glVertex3f(2, 0, thickness);
  glEnd(); //end triangles
  
  glPopMatrix();
}

/******************************************
 * Triforce
 * A symbol of power from the Legend of Zelda
 * consisting of three triangles
 *****************************************/
static void triforce() {
  glPushMatrix();
  
  //bottom left triangle
  glCallList(triangle_list);
  
  //top middle triangle
  glTranslatef(1, 1, 0);
  glCallList(triangle_list);
  
  //bottom right triangle
  glTranslatef(1, -1, 0);
  glCallList(triangle_list);
  
  glPopMatrix();
}

static void tardis_blackpanel(){
	glBegin(GL_QUADS);
	glColor3ub(0,0,0);
  glVertex3f(15, 35, 5.1f);
  glVertex3f(15, 7, 5.1f);
  glVertex3f(225, 7, 5.1f);
  glVertex3f(225, 35, 5.1f);
  glEnd();
  glColor3ub(255,255,255);

  glPushMatrix();
  glTranslatef(30,10,5.5f);
  glScalef(0.2, 0.2, 0.2);
  printw("POLICE");
  glPopMatrix();

  glPushMatrix();
  glTranslatef(155,10,5.5f);
  glScalef(0.2, 0.2, 0.2);
  printw("BOX");
  glPopMatrix();

  glPushMatrix();
  glTranslatef(120,23,5.5f);
  glScalef(0.07, 0.07, 0.07);
  printw("PUBLIC");
  glPopMatrix();

  glPushMatrix();
  glTranslatef(124,13,5.5f);
  glScalef(0.07, 0.07, 0.07);
  printw("CALL");
  glPopMatrix();
}

static void tardis_panel() {
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(62.5, 0, 0);
	glVertex3f(62.5, 60, 0);
	glVertex3f(0, 60, 0);
	glEnd();
	
}

static void tardis_window() {
	glColor3ub(255, 255, 255);
  glBegin(GL_QUADS);
  glVertex3f(0, 0, 0);
  glVertex3f(70, 0, 0);
  glVertex3f(70, 75, 0);
  glVertex3f(0, 75, 0);
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
  
  glBegin(GL_QUADS);
  glVertex3f(132.5, 10, 1.5);
	glVertex3f(122.5, 10, 1.5);
	glVertex3f(122.5, 400, 1.5);
	glVertex3f(132.5, 400, 1.5);
  glEnd();

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

  glEnd();



  
  //black back behind 'police box' text
  
  glPushMatrix();
    //front face
  glTranslatef(0,0,-5);
  glCallList(tardis_blackpanel_list);
  glPopMatrix();
  glPushMatrix();
  //left face
  glRotatef(-90.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(-240, 0, -5);
  glCallList(tardis_blackpanel_list);
  glPopMatrix();
  glPushMatrix();
  //right face
  glRotatef(90.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(0, 0, 235);
 glCallList(tardis_blackpanel_list);
  glPopMatrix();
  glPushMatrix();
  //back face
  glRotatef(180.0f,0.0f, 1.0f, 0.0f);
  glTranslatef(-240, 0, 235);
  glCallList(tardis_blackpanel_list);
  
  glPopMatrix();




  
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
  
  glColor3ub(32, 60, 105);
  glTranslatef(82.5,5,-82.5);
	glLineWidth(10);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 27.5, 0);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(25, 0, 0);
  glVertex3f(25, 27.5, 0);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(25, 0, -25);
  glVertex3f(25, 27.5, -25);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(0, 0, -25);
  glVertex3f(0, 27.5, -25);
  glEnd();
  glLineWidth(1);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(12.5, 35, -12.5);
  glVertex3f(-2.5, 27.5, 2.5);
  glVertex3f(27.5, 27.5, 2.5);
  glVertex3f(27.5, 27.5, -27.5);
  glVertex3f(-2.5, 27.5, -27.5);
  glVertex3f(-2.5, 27.5, 2.5);
  glEnd();
  glColor3ub(33, 71, 120);
  glBegin(GL_LINES);
  glVertex3f(12.5, 35, -12.5);
  glVertex3f(-2.5, 27.5, 2.5);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(12.5, 35, -12.5);
  glVertex3f(-2.5, 27.5, -27.5);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(12.5, 35, -12.5);
   glVertex3f(27.5, 27.5, -27.5);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f(12.5, 35, -12.5);
  glVertex3f(27.5, 27.5, 2.5);
  glEnd();

  glColor3ub(179,200,255);
  glTranslatef(12.5,0,-12.5);
  glRotatef(-90,1,0,0);
  gluCylinder(qobj, 10,10,27.5,120,160);
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
  
  //base platform rectangle
  glPushMatrix();
  
  glColor3ub(40, 80, 132);
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
  
  //base platform bottom face
  glColor3ub(31, 61, 114);
  glBegin(GL_QUADS);
  glVertex3f(245,0,5);
  glVertex3f(5,0,5);
  glVertex3f(5,0,-235);
  glVertex3f(245,0,-235);
  glEnd();
  
  glEnable(GL_TEXTURE_2D);
	glColor3ub(255, 255, 255);
  glBindTexture(GL_TEXTURE_2D, dw_texture);
	glBegin(GL_QUADS);
  glTexCoord3f(0,0,0); glVertex3f(240, 0.5, -230);
  glTexCoord3f(1,0,0); glVertex3f(10, 0.5, -230);
  glTexCoord3f(1,1,0); glVertex3f(10, 0.5, 0);
  glTexCoord3f(0,1,0); glVertex3f(240, 0.5, 0);
	glEnd();
  glDisable(GL_TEXTURE_2D);
  
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
  glTranslatef(-91,-70,1);
  glColor3ub(208,206,217);
  glBegin(GL_QUADS);
  glVertex3f(50,50,0);
  glVertex3f(0,50,0);
  glVertex3f(0,0,0);
  glVertex3f(50,0,0);
  glEnd();
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
  //load ground texture
  ground_texture = LoadPNG((char *)"/Users/calebgomer/Workspace/TARDIS/ground.png");
  //load dw texture
  dw_texture = LoadPNG((char *)"/Users/calebgomer/Workspace/TARDIS/dw.png");

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
  gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
  
	tardis_blackpanel_list = glGenLists(1);
  glNewList(tardis_blackpanel_list,GL_COMPILE);
  tardis_blackpanel();
  glEndList();
  
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
  
  triangle_list = glGenLists(1);
  glNewList(triangle_list, GL_COMPILE);
  triangle(0.35);
  glEndList();
  
  triforce_list = glGenLists(1);
  glNewList(triforce_list, GL_COMPILE);
  triforce();
  glEndList();
}

int main(int argc, char **argv) {
	draw_wireframes = true;
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
	glutCreateWindow("TARDIS+ - Time and Relative Dimmension in Space + Triforces");
  
  init(); //setup lists and stuff
  
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


