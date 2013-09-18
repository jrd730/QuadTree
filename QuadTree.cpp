#ifdef QUADTREE_H

#include "QuadTree.h"

template <typename T>
QuadTree<T>::QuadTree (vertex center, vertex range, unsigned bucketSize, unsigned depth)
{
	root = new QTNode <T>(center, range);
	maxDepth = depth;
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
vector <pair <vertex, T> > QuadTree<T>::getObjectsInRegion (vertex minXY, vertex maxXY)
{
	vector <pair <vertex, T> > results;
	queue <QTNode<T>*> nodes;
	nodes.push (root);
	
	while (!nodes.empty()){
		QTNode<T>* top = nodes.front();
		if (top->leaf){
			enclosure_status status = getEnclosureStatus(top->center, top->range, minXY, maxXY);
			switch (status){
				// this node is completely contained within the search region
				case NODE_CONTAINED_BY_REGION:
					// add all elements to results
					results.insert (results.end(), top->bucket.begin(), top->bucket.end());
				break;

				// this node is partially contained by the region
				case  NODE_PARTIALLY_IN_REGION:
					// search through this leaf node's bucket
					for (int i=0; i < top->bucket.size(); ++i){
						// check if this point is in the region
						if (pointInRegion(top->bucket[i].first, minXY, maxXY)){
							results.push_back (top->bucket[i]);
						}
					}
				break;

				// this node definitely has no points in the region
				case NODE_NOT_IN_REGION:
					// do nothing
				break;
			}
		}
		else{
			for (int i=0; i < 4; ++i){
				if (top->child[i]){
					// check if this nodes children could have points in the region
					enclosure_status status = getEnclosureStatus (top->child[i]->center, top->child[i]->range, minXY, maxXY);
					switch (status){
						// this node is completely contained by region, add all points within
						case NODE_CONTAINED_BY_REGION:
							addAllPointsToResults (top->child[i], results);
						break;

						// this node might contain points in the region
						case NODE_PARTIALLY_IN_REGION:
							nodes.push (top->child[i]);	
						break;

						// no points in region, discontinue searching this branch
						case NODE_NOT_IN_REGION:
						break;
					}
				}
			}
		}
		nodes.pop();
	}
	return results;
}

template <typename T>
void QuadTree<T>::addAllPointsToResults (QTNode<T>* node, vector <pair <vertex, T> >& results)
{
	if (node->leaf){
		results.insert (results.end(), node->bucket.begin(), node->bucket.end());
	}
	else{
		for (int i=0; i < 4; ++i){
			if (node->child[i]){
				addAllPointsToResults (node->child[i], results);
			}
		}
	}
}

template <typename T>
bool QuadTree<T>::pointInRegion (const vertex& point, const vertex& minXY, const vertex& maxXY)
{
	if ( (point.x >= minXY.x) && (point.x < maxXY.x) && (point.y >= minXY.y) && (point.y < maxXY.y) ) {
		return true;
	}
	else{
		return false;
	}
}

template <typename T>
enclosure_status QuadTree<T>::getEnclosureStatus (const vertex& center, const vertex& range, const vertex& minXY, const vertex& maxXY)
{
	int enclosedPts = 0;
	enclosedPts += pointInRegion ({center.x-range.x, center.y-range.y}, minXY, maxXY);
	enclosedPts += pointInRegion ({center.x-range.x, center.y+range.y}, minXY, maxXY);
	enclosedPts += pointInRegion ({center.x+range.x, center.y-range.y}, minXY, maxXY);
	enclosedPts += pointInRegion ({center.x+range.x, center.y+range.y}, minXY, maxXY);
	
	if (enclosedPts == 4){
		return NODE_CONTAINED_BY_REGION;
	}
	else if (enclosedPts > 0){
		return NODE_PARTIALLY_IN_REGION;
	}
	else {
		vertex nodeMin (center.x-range.x, center.y-range.y);
		vertex nodeMax (center.x+range.x, center.y+range.y);
		enclosedPts += pointInRegion (minXY, nodeMin, nodeMax);
		enclosedPts += pointInRegion ({minXY.x, maxXY.y}, nodeMin, nodeMax);
		enclosedPts += pointInRegion (maxXY, nodeMin, nodeMax);
		enclosedPts += pointInRegion ({maxXY.x, minXY.y}, nodeMin, nodeMax);
		if (enclosedPts > 0){
			return NODE_PARTIALLY_IN_REGION;
		}
	}
		return NODE_NOT_IN_REGION;	
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