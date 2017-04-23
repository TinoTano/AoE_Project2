#ifndef __Villager__
#define __Villager__

#include "Unit.h"
#include "Resource.h"

class Villager : public Unit
{
public:
	Villager();
	Villager(int posx, int posy, Villager* unit = nullptr);

	void GatherResource(float dt);

public:
	int gathering_speed = 0;
	int max_capacity = 0;
	int curr_capacity = 0;
	resourceType resource_carried = NONE;

	SDL_Texture* unitChoppingTexture = nullptr;

	vector<Animation> choppingAnimations;
	vector<Animation> minningAnimations;
	vector<Animation> collectingAnimalsAnimations;
	vector<Animation> collectingPlantsAnimations;

};



#endif