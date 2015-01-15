QuadTree
========

A quad tree is a data structure specializing in the storage and lookup of points contained within a rectangular region of a 2d plane. This demo allows a user to insert points into a quadtree and shows the dynamic breakdown of the node structure. Inserts, deletes, and searches are performed in logarithmic time. Quad tree is dynamic and allocates the minimum number of nodes necessary to partition the point set, because of this it is capable of high precision even on large regions without requiring enormous memory overhead. The number of potential leaf nodes in a QuadTree is determined by the max depth attribute, in general MAX_LEAF_NODES = (2^MAX_DEPTH)^2. The side length of the smallest possible leaf node is (RANGE)/(2^(MAX_DEPTH-1)).  
  
Controls
========

* 'left mouse' - place new point
* 'right mouse' - move selection window
* 'w' - pan up
* 'a' - pan left
* 's' - pan down
* 'd' - pan right
* 'b' - reduce bucket size
* 'B' - increase bucket size
* 'k' - remove points in selection window
* 'r' - place 100 random points
* '-' - zoom in
* '+' - zoom out
* '~' - reset tree/delete points  
