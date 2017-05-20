#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Entity.h"
#include "Collision.h"
#include <vector>

enum resourceType {
	NONE, WOOD, GOLD, FOOD, STONE
};

enum resourceItem {
	BLACK_TREE, GREEN_TREE, GOLD_MINE, BUSH, STONE_MINE, FOREST_ROCK, MOUNT_1, MOUNT_2, MOUNT_3, MOUNT_4, MOUNT_5, MOUNT_6, ROCK_MINE
};


class Resource : public Entity
{
public:
	Resource();
	Resource(int posX, int posY, Resource* resource);
	~Resource();
	
	bool Draw();
	void Destroy();

public:
	resourceItem res_type = BLACK_TREE;
	resourceType contains = WOOD;
	SDL_Rect blit_rect = { 0,0,0,0 };
	int selectionWidth = 0;

	//only for templates
	vector<SDL_Rect> blit_rects;
};

#endif
