#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "Animation.h"
#include <list>

class Unit;

enum buildingType {
	TOWN_CENTER, BARRACKS, FARM, ARCHER_TOWER
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
	Building(int posX, int posY, bool isEnemy, buildingType type, buildingFaction faction);
	~Building();

	bool Update(float dt);
	bool Draw();
	void Attack(float dt);
	void Dead();
	void SetAnim();

private:
	buildingType type;
	buildingFaction faction;
	float buildingAttackSpeed;
	int buildingPiercingDamage;
	bool isEnemy;
	float attackDelay;
	float timer = 0;
	list<Unit*> availableUnitsToCreateList;
	bool isDamaged;
	Animation* currentAnim = nullptr;

public:
	Unit* attackUnitTarget;
	int buildingLife;
	int buildingAttack;
	int buildingDefense;
	buildingState state;
};

#endif // !__BUILDING_H__







