#include "QuadTree.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>
#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
using namespace std;

static int height = 480;
static int width = 480;
static double ZOOM_INC = 32.0;
static double PAN_INC = 32.0;

static float graphXMin = -9;
static float graphXMax = 9;
float graphXRange = graphXMax - graphXMin;
float pixToXCoord = graphXRange/width;

static float graphYMin = -9;
static float graphYMax = 9;
float graphYRange = graphYMax - graphYMin;
float pixToYCoord = graphYRange/height;

static bool leftMouseDown = 0;
static bool rightMouseDown = 0;

vector <vertex> targetPoint;
vector <vertex> foundPoint;

vertex squareCenter (0, 0);
vertex squareRange (10, 10);

vertex origin (0, 0);
vertex axis (128.0, 128.0);
static int bucketSize = 1;
QuadTree <int>* qtree;

bool going (false);

long double randomFloat ();

void  initializeViewMatrix ()
{
    graphXRange = graphXMax - graphXMin;
    graphYRange = graphYMax - graphYMin;
    pixToXCoord = graphXRange/width;
    pixToYCoord = graphYRange/height;
}

static void resize(int w, int h)
{
    width = w;
    height = h;
    initializeViewMatrix();
    glViewport (0,0,(GLsizei)width, (GLsizei)height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void pan (double xAmount, double yAmount)
{
    graphXMin += xAmount;
    graphXMax += xAmount;
    graphYMin += yAmount;
    graphYMax += yAmount;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void zoom (double xAmount, double yAmount)
{
    graphXMin -= xAmount;
    graphXMax += xAmount;
    graphYMin -= yAmount;
    graphYMax += yAmount;

    initializeViewMatrix ();
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D (graphXMin, graphXMax, graphYMin, graphYMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();   
}

long double randomFloat (){
    long double x=((long double)rand()/(long double)RAND_MAX);
    return x;
}

static void findPoints ()
{
  vector <pair <vertex, int> > found;
  found = qtree->getObjectsInRegion (
      {squareCenter.x-squareRange.x, squareCenter.y-squareRange.y}, 
      {squareCenter.x+squareRange.x, squareCenter.y+squareRange.y});

  foundPoint.clear();
  foundPoint.resize(found.size());

  for (int i=0; i < found.size(); ++i){
      foundPoint[i] = found[i].first;
  }
}

static void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f (1, 1, 1);
    
    qtree->draw(); 

    /*
    // target points 
    glColor3f (0, 0, 1);
    glPointSize (3.0);
    glBegin (GL_POINTS);
        for (unsigned i=0; i<targetPoint.size(); ++i){
            glVertex2f (targetPoint[i].x, targetPoint[i].y);
        }
    glEnd();
    */

    glColor3f (0, 1, 0);
    glBegin (GL_LINE_LOOP);
        glVertex2f (squareCenter.x-squareRange.x, squareCenter.y-squareRange.y);
        glVertex2f (squareCenter.x-squareRange.x, squareCenter.y+squareRange.y);
        glVertex2f (squareCenter.x+squareRange.x, squareCenter.y+squareRange.y);
        glVertex2f (squareCenter.x+squareRange.x, squareCenter.y-squareRange.y);
    glEnd();

    // found points 
    glColor3f (0, 1, 0);
    glPointSize (3.0);
    glBegin (GL_POINTS);
        for (unsigned i=0; i<foundPoint.size(); ++i){
            glVertex2f (foundPoint[i].x, foundPoint[i].y);
        }
    glEnd();

    glFlush();
    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key){
        case 'c':
        case 'C':
            for (int i=0; i < targetPoint.size(); ++i){
                qtree->remove (targetPoint[i]);
            }
            targetPoint.clear();
        break;

        case 'k':
            for (int i=0; i < foundPoint.size(); ++i){
                qtree->remove (foundPoint[i]);
            }
            foundPoint.clear();
        break;

        case 'p':
        case 'P':
            cout << qtree->print();
        break;        

        case 'a':
        case 'A':
            pan (-graphXRange/PAN_INC, 0);
        break;

        case 'd':
        case 'D':
            pan (graphXRange/PAN_INC, 0);
        break; 

        case 'w':
        case 'W':
            pan (0, graphYRange/PAN_INC);
        break; 

        case 's':
        case 'S':
            pan (0, -graphYRange/PAN_INC);
        break;

        case 'r':
        case 'R':
            for (int i=0; i < 100; ++i){
                vertex newpoint (  axis.x - ( 2 * axis.x * randomFloat()),
                                   axis.y - ( 2 * axis.y * randomFloat()));
                targetPoint.push_back(newpoint);
                qtree->insert (newpoint, 1);
            }
        break;

        case 'b':
            delete qtree;
            bucketSize--;
            qtree = new QuadTree <int> (origin, axis, bucketSize);
            for (int i=0; i < targetPoint.size(); ++i){
                qtree->insert (targetPoint[i], 1);
            }
        break;

        case 'B':
            delete qtree;
            bucketSize++;
            qtree = new QuadTree <int> (origin, axis, bucketSize);
            for (int i=0; i < targetPoint.size(); ++i){
                qtree->insert (targetPoint[i], 1);
            }
        break;

        case '~':
            delete qtree;
            qtree = new QuadTree <int> (origin, axis, bucketSize);
            targetPoint.clear ();
            foundPoint.clear ();            
        break;

        case '+':
        case '=':
            zoom ( graphXRange/ZOOM_INC, graphYRange/ZOOM_INC );
        break; 

        case '-':
        case '_':
            zoom ( -graphXRange/ZOOM_INC, -graphYRange/ZOOM_INC );
        break;

        case 'f':
            findPoints ();
        break;
    }
    glutPostRedisplay();
}

static void mouse (int button, int state, int x, int y)
{
    vertex newpoint ( x*pixToXCoord + graphXMin, -y*pixToYCoord + graphYMax);

    switch (button){
        case GLUT_LEFT_BUTTON:
            switch (state){
                case GLUT_DOWN:
                    leftMouseDown = 1;
                    targetPoint.push_back(newpoint);
                    qtree->insert (newpoint, 1);
                break;

                case GLUT_UP:
                    leftMouseDown = 0;
                break;
            }
        break;
        case GLUT_RIGHT_BUTTON:
        		switch (state){
                case GLUT_DOWN:
                	rightMouseDown = 1;
                	squareCenter = newpoint;
                  findPoints ();
                break;

                case GLUT_UP:
                    rightMouseDown = 0;
                break;
            }
        break;
    }
    glutPostRedisplay();
}

static void motion (int x, int y)
{
    vertex newpoint ( x*pixToXCoord + graphXMin,
                      -y*pixToYCoord + graphYMax);

    if (leftMouseDown){
    	targetPoint.push_back(newpoint);
    	qtree->insert (newpoint, 1);
    }
    else if (rightMouseDown){
    	squareCenter = newpoint;
    	findPoints ();
    }
    
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    srand (time (0));
    qtree = new QuadTree <int> (origin, axis, 1);
    glutInit(&argc, argv);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Quad Tree Demo");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glClearColor(0,0,0,0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
