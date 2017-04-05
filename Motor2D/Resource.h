#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Entity.h"
#include <vector>

enum resourceType {
	BLACK_TREE, GREEN_TREE, GOLD_MINE, FOOD, STONE
};

enum resourceState {
	RESOURCE_IDLE, RESOURCE_GATHERING
};

class Resource : public Entity
{
public:
	Resource();
	Resource(int posX, int posY, Resource* resource, int resourceRectIndex);
	~Resource();

	bool Update(float dt);
	bool Draw();
	
	void Dead();

private:

public:
	resourceState state = RESOURCE_IDLE;
	resourceType type = BLACK_TREE;
	int resourceLife = 0;
	SDL_Texture* resourceIdleTexture = nullptr;
	SDL_Texture* resourceGatheringTexture = nullptr;
	bool isVisible = true;
	bool isSelected = false;
	vector<SDL_Rect> resourceRectVector;
	SDL_Rect resourceRect;
	int rectIndex;
	bool isInteractable = false;
	SDL_Rect resourceGatheringRect;
};

#endif
