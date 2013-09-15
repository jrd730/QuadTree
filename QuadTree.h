/**
	QuadTree.h
	
	QTNode: the recursive building block of a quad tree

	Quadtree: a dynamic 2d space partitioning structure

**/

#ifndef QUADTREE_H
#define QUADTREE_H

#include <cstdlib>
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

#include "QTNode.h"
#include "Vertex.h"

using namespace std;

#define LOWER_LEFT_QUAD 0
#define UPPER_LEFT_QUAD 1
#define LOWER_RIGHT_QUAD 2
#define UPPER_RIGHT_QUAD 3

template <typename T>
class QuadTree
{
	public:

		QuadTree <T>(vertex center, vertex range, unsigned bucketSize=1);
		~QuadTree ();

		void 	insert (vertex v);
		bool 	contains (vertex v);
		bool 	remove (vertex v);
		void 	draw ();
		string 	print ();
		
		//vector <vertex> getPointsInRegion ();
		//vector <vertex> getPointsInRadius ();

	private:

		void 	print (QTNode <T>* node, stringstream& ss);
		int 	direction (const vertex& point, QTNode <T>* node);
		vertex 	newCenter (int direction, QTNode <T>* node);
		QTNode<T>* childNode (const vertex& v, QTNode<T>* node);
		void 	insert (vertex v, QTNode<T>* node, unsigned depth);
		void	reduce (stack <QTNode<T>*>& node);
		bool	remove (vertex v, QTNode<T>* parent, QTNode<T>* child);
		void 	draw (QTNode<T>* node);

		QTNode<T>* root;

		unsigned maxDepth,
			maxBucketSize;


};


#include "QuadTree.cpp"
#endif //#ifdef QUADTREE_H