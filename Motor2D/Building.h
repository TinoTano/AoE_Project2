#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Animation.h"
#include <list>

class Unit;

enum buildingType {
	TOWN_CENTER, HOUSE, ORC_BARRACKS, ARCHERY_RANGE, STABLES, SIEGE_WORKSHOP, MARKET, BLACKSMITH, MILL, WALL, GATE, OUTPOST, MONASTERY, CASTLE
};

enum buildingState
{
	BUILDING_IDLE, BUILDING_ATTACKING, BUILDING_DESTROYING
};

enum buildingFaction {
	FREE_MEN_BUILDING, SAURON_ARMY_BUILDING
};

class Building : public Entity
{
public:
	Building();
	Building(int posX, int posY, bool isEnemy, Building* building);
	~Building();

	bool Update(float dt);
	bool Draw();
	void Attack(float dt);
	void Dead();
	pugi::xml_node LoadBuildingInfo(buildingType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

public:
	buildingType type = ORC_BARRACKS;
	buildingFaction faction;
	float buildingAttackSpeed = 0;
	int buildingPiercingDamage = 0;
	bool isEnemy = false;
	float timer = 0;
	list<Unit*> availableUnitsToCreateList;
	bool isDamaged = false;
	int hpBarWidth = 0;
	int buildingWoodCost = 0;
	int buildingStoneCost = 0;
	int buildingBuildTime = 0;
	SDL_Texture* buildingIdleTexture = nullptr;
	SDL_Texture* buildingDieTexture = nullptr;
	uint imageWidth = 0;
	uint imageHeight = 0;
	Unit* attackUnitTarget = nullptr;
	int buildingLife = 0;
	int buildingMaxLife = 0;
	int buildingAttack = 0;
	int buildingDefense = 0;
	bool isVisible = true;
	bool isSelected = false;
	bool canAttack = false;
	buildingState state = BUILDING_IDLE;
};

#endif // !__BUILDING_H__







