#include "QuadTree.h"

QuadTree::QuadTree (vertex center, vertex range)
{
	root = new QTNode(center, range, true);
	maxDepth = 16;
	maxBucketSize = 1;
}

QuadTree::~QuadTree ()
{

}

void QuadTree::insert (vertex v)
{
	insert (v, root, 0);
}

int QuadTree::direction (vertex point, QTNode* node)
{
	unsigned X=0, Y=0;
	X |= ((point.x >= node->center.x)<<1);
	Y |= ((point.y >= node->center.y)<<0);
	return (X|Y);
}

vertex QuadTree::newCenter (int direction, QTNode* node)
{
	vertex v(node->center.x, node->center.y); 
	switch (direction){
		case LOWER_LEFT_QUAD:
			v.x -= node->range.x/2.0;
			v.y -= node->range.y/2.0;
		break;
		case UPPER_LEFT_QUAD:
			v.x -= node->range.x/2.0;
			v.y += node->range.y/2.0;
		break;
		case LOWER_RIGHT_QUAD:
			v.x += node->range.x/2.0;
			v.y -= node->range.y/2.0;
		break;
		case UPPER_RIGHT_QUAD:
			v.x += node->range.x/2.0;
			v.y += node->range.y/2.0;
		break;
	}
	return v;
}

void QuadTree::insert (vertex v, QTNode* node, unsigned depth)
{
	// there is room in this node's bucket, or max depth has been reached
	if (node->bucket && (node->bucket->size() < maxBucketSize || depth > maxDepth)){
		node->bucket->push_front (v);
	}

	// no room in bucket, move down tree
	else{
		unsigned dir = direction (v, node);
		
		if (node->child[dir]){
			insert (v, node->child[dir], depth+1);
		}
		else{
			node->child[dir] = new QTNode (newCenter (dir, node), 
									{node->range.x/2.0, node->range.y/2.0});
			node->child[dir]->bucket = new list <vertex>;
			insert (v, node->child[dir], depth+1);
		}
	}
}

bool QuadTree::contains (vertex v)
{
	return false;
}

string QuadTree::print ()
{
	return "";
}

void QuadTree::draw ()
{
	if (root){
		draw (root);
	}
}

void QuadTree::draw (QTNode* node)
{
	glBegin (GL_LINE_LOOP);
		glVertex2f (node->center.x + node->range.x, node->center.y + node->range.y);
		glVertex2f (node->center.x + node->range.x, node->center.y - node->range.y);
		glVertex2f (node->center.x - node->range.x, node->center.y - node->range.y);
		glVertex2f (node->center.x - node->range.x, node->center.y + node->range.y);
	glEnd();

	for (int i=0; i < 4; ++i){
		if (node->child[i]){
			draw(node->child[i]);
		}
	}
}