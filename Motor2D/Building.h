#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <list>


enum buildingType {
	TOWN_CENTER, HOUSE, ORC_BARRACKS, ARCHERY_RANGE, STABLES, SIEGE_WORKSHOP, MARKET, BLACKSMITH, MILL, WALL, GATE, OUTPOST, MONASTERY, CASTLE, SAURON_TOWER
};

class Unit;
class Order;

class Building : public Entity
{
public:
	Building();
	Building(int posX, int posY, Building* building);
	~Building();

	bool Update(float dt);
	bool Draw();
	bool IsEnemy() const;
	pugi::xml_node LoadBuildingInfo(buildingType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

public:
	//STATS:
	buildingType type = ORC_BARRACKS;
	float buildingAttackSpeed = 0;
	int buildingPiercingDamage = 0;
	int buildingWoodCost = 0;
	int buildingStoneCost = 0;
	int buildingBuildTime = 0;
	bool canAttack = false;

	//Utilities
	Timer attack_timer;
	list<Unit*> availableUnitsToCreateList;
	SDL_Texture* buildingIdleTexture = nullptr;
	SDL_Texture* buildingDieTexture = nullptr;
	uint imageWidth = 0;
	uint imageHeight = 0;
	Collider* LineOfSight = nullptr;
	Collider* range = nullptr;
	list<Order*> order_list;

};

#endif // !__BUILDING_H__

