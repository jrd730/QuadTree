#ifdef QUADTREE_H

#include "QuadTree.h"

template <typename T>
QuadTree<T>::QuadTree (vertex center, vertex range, unsigned bucketSize)
{
	root = new QTNode <T>(center, range);
	maxDepth = 16;
	maxBucketSize = bucketSize;
}

template <typename T>
QuadTree<T>::~QuadTree ()
{
	delete root;
}

template <typename T>
void QuadTree<T>::insert (vertex v, T data)
{
	insert (v, data, root, 0);
}

template <typename T>
int QuadTree<T>::direction (const vertex& point, QTNode<T>* node)
{
	// get the quadrant that would contain the vertex
	// in reference to a given start node
	unsigned X=0, Y=0;
	X |= ((point.x >= node->center.x)<<1);
	Y |= ((point.y >= node->center.y)<<0);
	return (X|Y); 
}

template <typename T>
QTNode<T>* QuadTree<T>::childNode (const vertex& v, QTNode<T>* node)
{
	// get the next node that would contain the vertex
	// in reference to a given start node
	unsigned dir = direction (v, node);
	if (node->child[dir]){
		return node->child[dir];
	}
	// node not found, so create it 
	else{
		vertex r(node->range.x/2.0, node->range.y/2.0);
		node->child[dir] = new QTNode<T>(newCenter (dir, node), r);
		return node->child[dir];
	}
}

template <typename T>
vertex QuadTree<T>::newCenter (int direction, QTNode<T>* node)
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

template <typename T>
void QuadTree<T>::insert (vertex v, T data, QTNode<T>* node, unsigned depth)
{
	// by design, vertices are stored only in leaf nodes
	// newly created nodes are leaf nodes by default
	if (node->leaf){
		// there is room in this node's bucket
		if (node->bucket.size() < maxBucketSize){
			node->bucket.push_back ({v, data});
		}
		// bucket is full, so push all vertices to next depth,
		// clear the current node's bucket and make it a stem
		else if (depth < maxDepth){
			node->leaf = false;
			insert (v, data, childNode (v, node), depth+1);
			for (int i=0; i < node->bucket.size(); ++i){
				insert (node->bucket[i].first, data, childNode(node->bucket[i].first, node), depth+1);
			}
			node->bucket.clear();
		}
	}
	// current node is a stem node used for navigation
	else{
		insert (v, data, childNode (v, node), depth+1);
	}
}

template <typename T>
void QuadTree<T>::reduce (stack <QTNode<T>*>& nodes)
{
	// once a vertex is removed from a leaf node's bucket
	// check to see if that node's parent can consume it
	// and all of it's sibling nodes
	bool canReduce = true;
	nodes.pop();
	while (canReduce && !nodes.empty()) {
		canReduce = true;
		QTNode<T>* top = nodes.top();
		int numKeys = 0;
		for (int i=0; i < 4; ++i){
			if (top->child[i] && !top->child[i]->leaf){
				canReduce = false;
				return;
			}
			else if (top->child[i] && top->child[i]->leaf){
				numKeys += top->child[i]->bucket.size();
			}
		}
		canReduce &= (numKeys <= maxBucketSize);
		if (canReduce){
			for (int i=0; i < 4; ++i){
				if (top->child[i]){
					for (int j=0; j < top->child[i]->bucket.size(); ++j){
						top->bucket.push_back ( top->child[i]->bucket[j] );
					}
					delete top->child[i];
					top->child[i] = NULL;
				}
			}
			top->leaf = true;
		}
		nodes.pop();
	}
	return;
}	

template <typename T>
bool QuadTree<T>::remove (vertex v)
{
	stack <QTNode<T>*> nodes;
	nodes.push (root);
	QTNode<T>* top = nodes.top();
	unsigned dir;

	// navigate to leaf node containing the vertex to be deleted
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
	// linearly search bucket for target vertex
	for (int i=0; i < top->bucket.size(); ++i){
		// vertex found, delete from bucket
		if (top->bucket[i].first == v){
			top->bucket.erase(top->bucket.begin()+i);
			reduce (nodes);
			return true;
		}
		else{
			return false;
		}
	}	
}

template <typename T>
bool QuadTree<T>::contains (vertex v)
{
	return false;
}

template <typename T>
string QuadTree<T>::print ()
{
	stringstream ss("");
	print (root, ss);
	return ss.str();
}

template <typename T>
void QuadTree<T>::print (QTNode<T>* node, stringstream& ss)
{
	for (int i=0; i < 4; ++i){
		if (node->child[i]){
			print (node->child[i], ss);
			for (int i = 0; i < node->bucket.size(); i++){
				ss << '{' << node->bucket[i].first.x << ','
						 << node->bucket[i].first.y << '}' << ' ';
			}
		}
	}	
	return;
}

template <typename T>
void QuadTree<T>::draw ()
{
	if (root){
		draw (root);
	}
}

template <typename T>
void QuadTree<T>::draw (QTNode<T>* node)
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
			glVertex2f (node->bucket[i].first.x, node->bucket[i].first.y);
		}

	glEnd();

	for (int i=0; i < 4; ++i){
		if (node->child[i]){
			draw(node->child[i]);
		}
	}
}

#endif