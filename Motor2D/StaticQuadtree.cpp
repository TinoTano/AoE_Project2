#include "StaticQuadtree.h"
#include "Application.h"
#include "Map.h"
#include <cmath>

StaticQuadTree::StaticQuadTree() {

	for (int i = 0; i < (NODES_FOR_ROW * NODES_FOR_ROW); i++) {
		list<Collider*> list;
		nodes.push_back(list);
	}
}

StaticQuadTree::~StaticQuadTree()
{
	for (int i = 0; i < (NODES_FOR_ROW * NODES_FOR_ROW); i++) {
		delete &nodes.at(i);
	}
}
void StaticQuadTree::ClearTree() {

	for (int i = 0; i < (NODES_FOR_ROW * NODES_FOR_ROW); i++) {
		nodes.at(i).clear();
	}

}
void StaticQuadTree::UpdateCol(Collider* col)
{
	int current_node = Locate(col);
	if (current_node != col->quadtree_node && current_node >= 0 && current_node < 64) {
		nodes.at(col->quadtree_node).remove(col);
		nodes.at(col->quadtree_node = current_node).push_back(col);
	}
	
}

void StaticQuadTree::Insert(Collider* col) 
{
	col->quadtree_node = Locate(col);
	if(col->quadtree_node < 64 && col->quadtree_node >= 0)
		nodes.at(col->quadtree_node).push_back(col);
}

int StaticQuadTree::Locate(Collider* col) {
	iPoint MapPos = App->map->WorldToMap(col->pos.x, col->pos.y);   
	return ((trunc((float)(MapPos.y / NODES_FOR_ROW)) * NODES_FOR_ROW) + (trunc((float)(MapPos.x / NODES_FOR_ROW))));

}
void StaticQuadTree::Remove(Collider* col)
{
	nodes.at(col->quadtree_node).remove(col);
}
void StaticQuadTree::Retrieve(list<Collider*> &potentialColliders, Collider* col)
{
	potentialColliders = nodes.at(col->quadtree_node);
}