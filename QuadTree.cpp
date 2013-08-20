#include "QuadTree.h"

QuadTree::QuadTree (vertex center, vertex range, unsigned bucketSize)
{
	root = new QTNode(center, range);
	maxDepth = 16;
	maxBucketSize = bucketSize;
}

QuadTree::~QuadTree ()
{
	delete root;
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

QTNode* QuadTree::childNode (vertex v, QTNode* node)
{
	unsigned dir = direction (v, node);
		
	if (node->child[dir]){
		return node->child[dir];
	}
	else{
		node->child[dir] = new QTNode (newCenter (dir, node), 
								{node->range.x/2.0, node->range.y/2.0});
		return node->child[dir];
	}
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
	if (node->leaf){
		if (node->bucket.size() < maxBucketSize){
			node->bucket.push_back (v);
		}
		else if (depth < maxDepth){
			node->leaf = false;
			insert (v, childNode (v, node), depth+1);
			for (int i=0; i < node->bucket.size(); ++i){
				insert (node->bucket[i], childNode(node->bucket[i], node), depth+1);
			}
			node->bucket.clear();
		}
	}
	else{
		insert (v, childNode (v, node), depth+1);
	}
}

bool QuadTree::remove (vertex v)
{
	stack <QTNode*> nodes;
	nodes.push (root);
	QTNode* top = nodes.top();
	unsigned dir;
	while (!top->leaf){
		dir = direction (v, top);
		if (top->child[dir]){
			nodes.push (top->child[dir]);
			top = nodes.top();
		}
		else{
			return false;
		}
	}	
	for (int i=0; i < top->bucket.size(); ++i){
		if (top->bucket[i] == v){
			top->bucket.erase(top->bucket.begin()+i);
			return true;
		}
		else{
			return false;
		}
	}	
}

bool QuadTree::contains (vertex v)
{
	return false;
}

string QuadTree::print ()
{
	stringstream ss("");
	print (root, ss);
	return ss.str();
}

void QuadTree::print (QTNode* node, stringstream& ss)
{
	for (int i=0; i < 4; ++i){
		if (node->child[i]){
			print (node->child[i], ss);
			for (int i = 0; i < node->bucket.size(); i++){
				ss << '{' << node->bucket[i].x << ','
						 << node->bucket[i].y << '}' << ' ';
			}
		}
	}
	
	return;

}

void QuadTree::draw ()
{
	if (root){
		draw (root);
	}
}

void QuadTree::draw (QTNode* node)
{
	/*
	glBegin (GL_LINE_LOOP);
		glVertex2f (node->center.x + node->range.x, node->center.y + node->range.y);
		glVertex2f (node->center.x + node->range.x, node->center.y - node->range.y);
		glVertex2f (node->center.x - node->range.x, node->center.y - node->range.y);
		glVertex2f (node->center.x - node->range.x, node->center.y + node->range.y);
	glEnd();
	*/
	glBegin (GL_LINES);

		glVertex2f (node->center.x, node->center.y);
		glVertex2f (node->center.x + node->range.x, node->center.y + node->range.y);

		glVertex2f (node->center.x, node->center.y);
		glVertex2f (node->center.x + node->range.x, node->center.y - node->range.y);

		glVertex2f (node->center.x, node->center.y);
		glVertex2f (node->center.x - node->range.x, node->center.y - node->range.y);

		glVertex2f (node->center.x, node->center.y);
		glVertex2f (node->center.x - node->range.x, node->center.y + node->range.y);

		for (int i=0; i < node->bucket.size(); ++i){
			glVertex2f (node->center.x, node->center.y);
			glVertex2f (node->bucket[i].x, node->bucket[i].y);
		}

	glEnd();

	for (int i=0; i < 4; ++i){
		if (node->child[i]){
			draw(node->child[i]);
		}
	}
}