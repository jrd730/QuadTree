/**
	QuadTree.h
	
	QTNode: the recursive building block of a quad tree

	Quadtree: a dynamic 2d space partitioning structure

**/
#include "Vertex.h"
#include <cstdlib>
#include <list>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
using namespace std;

#define LOWER_LEFT_QUAD 0
#define UPPER_LEFT_QUAD 1
#define LOWER_RIGHT_QUAD 2
#define UPPER_RIGHT_QUAD 3

class QTNode{
	
	public:
		
		QTNode (vertex newCenter, vertex newRange) { 
			child[0] = NULL;
			child[1] = NULL;
			child[2] = NULL; 
			child[3] = NULL;
			center = newCenter;
			range = newRange;
			leaf = true;
		}
		~QTNode (){ for (int i=0; i < 4; ++i) if (child[i]){delete child[i];}}

		bool leaf;
		QTNode* child[4];
		vertex center, range;
		vector <vertex> bucket;
	
	private:
		
};

class QuadTree
{
	public:

		QuadTree (vertex center, vertex range, unsigned bucketSize=1);
		~QuadTree ();

		void 	insert (vertex v);
		bool 	contains (vertex v);
		bool 	remove (vertex v);
		void 	draw ();
		string 	print ();

	private:

		void 	print (QTNode* node, stringstream& ss);
		int 	direction (vertex point, QTNode* node);
		vertex 	newCenter (int direction, QTNode* node);
		QTNode* childNode (vertex v, QTNode* node);
		void 	insert (vertex v, QTNode* node, unsigned depth);
		bool	remove (vertex v, QTNode* parent, QTNode* child);
		void 	draw (QTNode* node);

		QTNode* root;

		unsigned maxDepth,
			maxBucketSize;


};