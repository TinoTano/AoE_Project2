#include "QuadTree.h"

QuadTree::QuadTree(SDL_Rect nodeBounds, int level, int node_index)
{
	node[0] = node[1] = node[2] = node[3] = nullptr;
	nodeIndex = node_index;
	nodeRect = nodeBounds;
	this->level = level;
}

QuadTree::~QuadTree()
{

}

void QuadTree::ClearTree()
{
	collidersList.clear();

	for (int i = 0; i < NODE_COUNT; i++) {
		if (node[0] != nullptr) {
			node[i]->ClearTree();
			RELEASE(node[i]);
		}
	}
}

void QuadTree::SplitNode()
{
	int halfWidth = nodeRect.w / 2;
	int halfHeight = nodeRect.h / 2;
	int x = nodeRect.x;
	int y = nodeRect.y;

	node[0] = new QuadTree(SDL_Rect{ x, y, halfWidth, halfHeight }, level + 1, 0); //NW Node
	node[1] = new QuadTree(SDL_Rect{ x + halfWidth, y, halfWidth, halfHeight }, level + 1, 1); //NE Node
	node[2] = new QuadTree(SDL_Rect{ x, y + halfHeight, halfWidth, halfHeight }, level + 1, 2); //SW Node
	node[3] = new QuadTree(SDL_Rect{ x + halfWidth, y + halfHeight, halfWidth, halfHeight }, level + 1, 3); //SE Node

	for (int i = 0; i < NODE_COUNT; i++) {

		for (list<Collider*>::iterator it = collidersList.begin(); it != collidersList.end(); it++) {
			if (*it != nullptr) {
				if (node[i]->Contains(*it))
					node[i]->collidersList.push_back(*it);
			}
		}
	}
	collidersList.clear();
}

void QuadTree::MergeNode() {

	for (int i = 0; i < NODE_COUNT; i++) {

		for (list<Collider*>::iterator it = node[i]->collidersList.begin(); it != node[i]->collidersList.end(); it++)
			collidersList.push_back(*it);

		RELEASE(node[i]);
		node[i] = nullptr;
	}
	
}

void QuadTree::UpdateCol(Collider* col) {

	int index = GetNodeIndex(col);
	if (index == -1) {
		if (!IsInList(col)) {
			collidersList.push_back(col);
			if (collidersList.size() > MAX_OBJECTS && level < MAX_LEVELS && node[0] == nullptr)
				SplitNode();
		}
	}
	else {
		for (int i = 0; i < NODE_COUNT; i++) {
			if (col != nullptr) {
				if (i != index && node[i]->IsInList(col) && !node[i]->Contains(col))
					node[i]->collidersList.remove(col);
			}
		}

		node[index]->UpdateCol(col);
	}
}

void QuadTree::UpdateTree() {

	if (node[0] != nullptr) {
		for (int i = 0; i < NODE_COUNT; i++) {

			if (node[i]->collidersList.size() < MAX_OBJECTS && node[i]->node[0] != nullptr)
				node[i]->MergeNode();
			else
				node[i]->UpdateTree();
		}
	}
}

int QuadTree::GetNodeIndex(Collider* col, int start_from)
{
	int index = -1;

	for (int i = start_from; i < NODE_COUNT; i++) {
		if (node[i] != nullptr) {
			if (col != nullptr) {
				if (node[i]->Contains(col)) {
					index = node[i]->nodeIndex;
				}
			}
		}
	}

	return index;
}

void QuadTree::Insert(Collider* col)
{

	if (node[0] != nullptr) {
		int index = GetNodeIndex(col);

		if (index != -1) {
			node[index]->Insert(col);

			return;
		}
	}

	collidersList.push_back(col);

	if (collidersList.size() > MAX_OBJECTS && level < MAX_LEVELS && node[0] == nullptr) 
		SplitNode();
	
}

void QuadTree::Remove(Collider* col) {

	int index = GetNodeIndex(col);
	if (index != -1 && node[0] != nullptr) {
		node[index]->Remove(col);
	}
	else 
		collidersList.remove(col);

}

void QuadTree::Retrieve(list<Collider*> &possibleColliders, Collider* col)
{

	int index = GetNodeIndex(col);
	if (index == -1) {
		for (list<Collider*>::iterator it = collidersList.begin(); it != collidersList.end(); it++) {
			//Skip checking with himself
			if (col != *it) 
				possibleColliders.push_back(*it);
		}
	}
	else {
		while (index != -1) {

			node[index]->Retrieve(possibleColliders, col);
			index = GetNodeIndex(col, (index + 1));
		}
	}
}

bool QuadTree::Contains(Collider * col)
{
	return (col->pos.x >= (nodeRect.x - col->r) && col->pos.x <= (nodeRect.x + nodeRect.w + col->r) &&
		col->pos.y >= (nodeRect.y - col->r) && col->pos.y <= (nodeRect.y + nodeRect.h + col->r));
}

bool QuadTree::IsInList(Collider* col) {

	for (list<Collider*>::iterator it = collidersList.begin(); it != collidersList.end(); it++) {
		if (col == (*it))
			return true;
	}
	return false;
}
