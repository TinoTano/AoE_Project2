#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Entity.h"

enum resourceType {
	WOOD, GOLD_MINE, FOOD, STONE
};

enum resourceState {
	RESOURCE_IDLE, RESOURCE_GATHERING
};

class Resource : public Entity
{
public:
	Resource();
	Resource(int posX, int posY, Resource* resource);
	~Resource();

	bool Update(float dt);
	bool Draw();

private:

public:
	resourceState state = RESOURCE_IDLE;
	resourceType type = WOOD;
	int resourceLife = 0;
	SDL_Texture* resourceIdleTexture = nullptr;
	SDL_Texture* resourceGatheringTexture = nullptr;
	bool isVisible = true;
	bool isSelected = false;
};

#endif
