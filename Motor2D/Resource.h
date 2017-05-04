#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Entity.h"
#include "Collision.h"
#include <vector>

enum resourceType {
	NONE, WOOD, GOLD, FOOD, STONE
};

enum resourceItem {
	BLACK_TREE, GREEN_TREE, GOLD_MINE, BUSH, STONE_MINE
};


class Resource : public Entity
{
public:
	Resource();
	Resource(int posX, int posY, Resource* resource);
	~Resource();

	bool Update(float dt);
	
	bool Draw();
	void Destroy();
	void Damaged();

private:

public:
	resourceItem visual = BLACK_TREE;
	resourceType type = WOOD;
	SDL_Texture* resourceIdleTexture = nullptr;
	SDL_Texture* resourceGatheringTexture = nullptr;
	vector<SDL_Rect> resourceRectVector;
	SDL_Rect resourceRect;
};

#endif
