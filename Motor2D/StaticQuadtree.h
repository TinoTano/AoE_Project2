#ifndef _STATIC_QUADTREE_H_
#define _STATIC_QUADTREE_H_

#include <list>
#include <vector>
#include "Collision.h"

#define NODES_FOR_ROW 8

class StaticQuadTree
{
public:
	StaticQuadTree();
	~StaticQuadTree();
	void ClearTree();
	void UpdateCol(Collider* col);
	void Insert(Collider* col);
	void Remove(Collider* col);
	void Retrieve(list<Collider*> &potentialColliders, Collider* col);
	int Locate(Collider* col);

public:
	vector<list<Collider*>> nodes;
};

#endif // !_QUAD_TREE_H_