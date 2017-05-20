#ifndef __Villager__
#define __Villager__

#include "Unit.h"
#include "Resource.h"
#include "Building.h"

class Villager : public Unit
{
public:
	Villager();
	Villager(int posx, int posy, Villager* unit = nullptr);


public:
	//STATS
	int gathering_speed = 0;
	int max_capacity = 0;
	int curr_capacity = 0;
	resourceType resource_carried = NONE;
	int buildingSpeed = 200;

	//ANIMATIONS
	SDL_Texture* unitChoppingTexture = nullptr;

	vector<Animation> choppingAnimations;
	vector<Animation> minningAnimations;
	vector<Animation> collectingAnimalsAnimations;
	vector<Animation> collectingPlantsAnimations;

};



#endif