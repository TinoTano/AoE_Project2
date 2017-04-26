#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Animation.h"
#include "Timer.h"
#include <list>


enum buildingType {
	TOWN_CENTER, HOUSE, ORC_BARRACKS, ARCHERY_RANGE, STABLES, SIEGE_WORKSHOP, MARKET, BLACKSMITH, MILL, WALL, GATE, OUTPOST, MONASTERY, CASTLE, SAURON_TOWER
};

enum buildingState
{
	BUILDING_IDLE, BUILDING_FINISH_CONSTRUCTION, BUILDING_ATTACKING, BUILDING_DESTROYING
};

class Unit;

class Building : public Entity
{
public:
	Building();
	Building(int posX, int posY, Building* building);
	~Building();

	bool Update(float dt);
	bool Draw();
	bool IsEnemy() const;
	void AttackEnemy(float dt);
	void Dead();
	pugi::xml_node LoadBuildingInfo(buildingType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

public:
	buildingType type = ORC_BARRACKS;
	float buildingAttackSpeed = 0;
	int buildingPiercingDamage = 0;
	Timer attack_timer;
	list<Unit*> availableUnitsToCreateList;
	int hpBarWidth = 0;
	int buildingWoodCost = 0;
	int buildingStoneCost = 0;
	int buildingBuildTime = 0;
	SDL_Texture* buildingIdleTexture = nullptr;
	SDL_Texture* buildingDieTexture = nullptr;
	uint imageWidth = 0;
	uint imageHeight = 0;
	Entity* attackTarget = nullptr;
	bool canAttack = false;
	buildingState state = BUILDING_IDLE;
	Collider* LineOfSight = nullptr;
	Collider* range = nullptr;
	bool onConstruction = false;
	bool isSelected = false;
};

#endif // !__BUILDING_H__

