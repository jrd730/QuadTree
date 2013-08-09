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

static float graphXMin = -9;
static float graphXMax = 9;
float graphXRange = graphXMax - graphXMin;
float pixToXCoord = graphXRange/width;

static float graphYMin = -9;
static float graphYMax = 9;
float graphYRange = graphYMax - graphYMin;
float pixToYCoord = graphYRange/height;

vector <vertex> targetPoint;

QuadTree qtree({0, 0}, {8, 8});

bool going (false);

long double randomFloat ();

void  initializeViewMatrix ()
{
    pixToXCoord = graphXRange/width;
    pixToYCoord = graphYRange/height;
}

long double randomFloat (){
    long double x=((long double)rand()/(long double)RAND_MAX);
    return x;
}


static void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    /*
    // x/y axis
    glColor3f (1, 1, 1);
    glBegin (GL_LINES);
        glVertex2f (0, graphYMax);
        glVertex2f (0, graphYMin);
        glVertex2f (graphXMax, 0);
        glVertex2f (graphXMin, 0);
    glEnd();
    */
    glColor3f (1, 1, 1);
    qtree.draw();

    // target points
    glColor3f (0, 0, 1);
    glPointSize (6.0);
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

    }
    //glutPostRedisplay();
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
    //targetPoint.push_back(newpoint);
    // cout << "mouse clicked at " << x << " " << y << endl;
    // cout << "new point at " << newpoint.x << " " << newpoint.y << endl;
    // cout << "points: " << targetPoint.size() << endl;
}

static void idle(void)
{

    glutPostRedisplay();
    //sleep (15);
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

int main(int argc, char *argv[])
{
    srand (time (0));
    ifstream fin;
    cout << "Loading target points...";
    fin.open(argv[1]);
    if (!fin.fail()){
        while (fin.good()){
            long double newX, newY;
            fin >> newX >> newY;
            vertex newTargetPoint (newX, newY);
            //targetPoint.push_back(newTargetPoint);
        }
        cout << "complete!\n";
    }
    else{
        cout << "\nError: input file not found\n";
    }

   

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
    glutIdleFunc(idle);
    glClearColor(0,0,0,0);
    glutMainLoop();
    return EXIT_SUCCESS;
}

