#include "QuadTree.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>
//#include <windows.h>
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
static double ZOOM_INC = 64.0;
static double PAN_INC = 64.0;


static float graphXMin = -9;
static float graphXMax = 9;
float graphXRange = graphXMax - graphXMin;
float pixToXCoord = graphXRange/width;

static float graphYMin = -9;
static float graphYMax = 9;
float graphYRange = graphYMax - graphYMin;
float pixToYCoord = graphYRange/height;

vector <vertex> targetPoint;

vertex origin (0, 0);
vertex axis (128.0, 128.0);

QuadTree qtree(origin, axis);

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
    //initializeViewMatrix();
    //glViewport (0,0,(GLsizei)width, (GLsizei)height);

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


static void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f (1, 1, 1);
    qtree.draw();

    // target points 
    glColor3f (0, 0, 1);
    glPointSize (3.0);
    glBegin (GL_POINTS);
        for (unsigned i=0; i<targetPoint.size(); i++){
            glVertex2f (targetPoint[i].x, targetPoint[i].y);
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
            targetPoint.clear();
        break;

        case 'p':
        case 'P':
            cout << qtree.print();
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
                qtree.insert (newpoint);
            }
        break;

        case '+':
        case '=':
            zoom ( graphXRange/ZOOM_INC, graphYRange/ZOOM_INC );
        break; 

        case '-':
        case '_':
            zoom ( -graphXRange/ZOOM_INC, -graphYRange/ZOOM_INC );
        break;        
    }
    glutPostRedisplay();
}

static void mouse (int button, int state, int x, int y)
{
    switch (button){
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_UP){
                vertex newpoint ( x*pixToXCoord + graphXMin,
                                  -y*pixToYCoord + graphYMax);
                targetPoint.push_back(newpoint);
                qtree.insert (newpoint);
            }
            // cout << "mouse clicked at " << x << " " << y << endl;
            // cout << "new point at " << newpoint.x << " " << newpoint.y << endl;
        break;
    }
}

static void motion (int x, int y)
{
    vertex newpoint ( x*pixToXCoord + graphXMin,
                      -y*pixToYCoord + graphYMax);
    qtree.insert (newpoint);
    targetPoint.push_back(newpoint);
    // cout << "mouse clicked at " << x << " " << y << endl;
    // cout << "new point at " << newpoint.x << " " << newpoint.y << endl;
    // cout << "points: " << targetPoint.size() << endl;
}

static void idle(void)
{
    glutPostRedisplay();
    //sleep (15);
}

int main(int argc, char *argv[])
{
    srand (time (0));
    glutInit(&argc, argv);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Quad Tree Demo");
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    //glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);
    glClearColor(0,0,0,0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
