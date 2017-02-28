#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"

enum buildingType {
	
};

enum buildingState
{
	IDLE, ATTACKING, DAMAGED, DESTROYING
};

enum buildingFaction {
	FREE_MEN, SAURON_ARMY
};

class Building : public Entity
{
public:
	Building(int posX, int posY, bool isEnemy, buildingType type, buildingFaction race);
	~Building();

	bool Update(float dt);
	bool Draw();
};

#endif // !__BUILDING_H__



