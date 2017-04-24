#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include <list>
#include <vector>
#include "Collision.h"

#define MAX_OBJECTS 2 //Number of obects in a node before split
#define NODE_COUNT 4 //4 nodes. NW, NE, SW, SE. 
#define MAX_LEVELS 5

using namespace std;

class QuadTree
{
public:
	QuadTree(SDL_Rect nodeBounds, int level);
	~QuadTree();
	void ClearTree();
	void SplitNode();
	int GetNodeIndex(Collider* col);
	void Insert(Collider* col);
	void Retrieve(list<Collider*> &potentialColliders, Collider* col);
	bool Contains(Collider* col);
	void GetNodes(vector<QuadTree*>& node);

public:
	list<Collider*> colllidersList;
	SDL_Rect nodeRect;
	QuadTree* node[4];
	int nodeIndex;
	int level;
};

#endif // !_QUAD_TREE_H_

