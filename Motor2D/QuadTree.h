#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include <list>
#include <vector>
#include "Collision.h"

#define MAX_OBJECTS 15 //Number of obects in a node before split
#define NODE_COUNT 4 //4 nodes. NW, NE, SW, SE. 
#define MAX_LEVELS 5

using namespace std;

class QuadTree
{
public:
	QuadTree(SDL_Rect nodeBounds, int level, int node_index = 0);
	~QuadTree();
	void ClearTree();
	void SplitNode();
	void MergeNode();
	void UpdateCol(Collider* col);
	void UpdateTree();
	int GetNodeIndex(Collider* col, int start_from = 0);
	void Insert(Collider* col);
	void Retrieve(list<Collider*> &potentialColliders, Collider* col);
	bool Contains(Collider* col);
	bool IsInList(Collider* col);

public:
	list<Collider*> collidersList;
	SDL_Rect nodeRect;
	QuadTree* node[4];
	int nodeIndex;
	int level;
};

#endif // !_QUAD_TREE_H_

