
#include <cstdlib>
#include <vector>
#include "Vertex.h"
using namespace std;

template <class T>
class QTNode{
	
	public:
		
		QTNode <T>(vertex newCenter, vertex newRange){ 
			child[0] = NULL;
			child[1] = NULL;
			child[2] = NULL; 
			child[3] = NULL;
			center = newCenter;
			range = newRange;
			leaf = true;
		}
		~QTNode (){ for (int i=0; i < 4; ++i) if (child[i]){delete child[i];}}

		vertex center, range;

		// used by stem nodes
		bool leaf;
		QTNode* child[4];
		
		// used by leaf nodes
		vector <pair <vertex, T> > bucket;
		
};