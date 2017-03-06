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
	Building(int posX, int posY, bool isEnemy, buildingType type);
	~Building();

	bool Update(float dt);
	bool Draw();
	void Attack(float dt);
	void Dead();
	pugi::xml_node LoadBuildingInfo(buildingType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:
	buildingType type;
	buildingFaction faction;
	float buildingAttackSpeed;
	int buildingPiercingDamage;
	bool isEnemy;
	float timer = 0;
	list<Unit*> availableUnitsToCreateList;
	bool isDamaged;
	int hpBarWidth;
	pugi::xml_node buildingNodeInfo;
	int buildingWoodCost;
	int buildingStoneCost;
	int buildingBuildTime;
	SDL_Texture* buildingIdleTexture;
	SDL_Texture* buildingDieTexture;
	uint imageWidth;
	uint imageHeight;

public:
	Unit* attackUnitTarget;
	int buildingLife;
	int buildingMaxLife;
	int buildingAttack;
	int buildingDefense;
	buildingState state;
	bool isSelected;
	bool canAttack;
};

#endif // !__BUILDING_H__







