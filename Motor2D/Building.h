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
	void Destroy();
	void GetBuildingBoundaries();
	pugi::xml_node LoadBuildingInfo(buildingType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

public:
	//STATS:
	buildingType type = ORC_BARRACKS;
	float buildingAttackSpeed = 0;
	int buildingPiercingDamage = 0;
	Cost cost;
	int buildingBuildTime = 0;
	bool canAttack = false;
	bool waitingToPlace = false;
	bool canBePlaced = false;

	//Utilities
	Timer attack_timer;
	SDL_Texture* buildingIdleTexture = nullptr;
	SDL_Texture* buildingDieTexture = nullptr;
	SDL_Texture* constructingPhase1 = nullptr;
	SDL_Texture* constructingPhase2 = nullptr;
	SDL_Texture* constructingPhase3 = nullptr;
	uint imageWidth = 0;
	uint imageHeight = 0;
	Collider* los = nullptr;
	Collider* range = nullptr;
	list<Order*> order_list;
	list<Order*> target_of;

};

#endif // !__BUILDING_H__

