#ifndef __UNIT_H__
#define __UNIT_H__

#include "p2Point.h"
#include "Entity.h"
#include "Animation.h"
#include <list>
#include <vector>
#include "Input.h"
#include "Map.h"
#include "Timer.h"

class Building;

enum unitType {
	ELVEN_LONGBLADE, DWARVEN_MAULER, GONDOR_SPEARMAN, ELVEN_ARCHER, DUNEDAIN_RANGE, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN, SIEGE_TOWER, LIGHT_CATAPULT,

	GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER, VENOMOUS_SPIDER, HARADRIM_OLIFANT, NAZGUL, TROLL_MAULER,

	VILLAGER
};

enum unitState
{
	UNIT_IDLE, UNIT_MOVING, UNIT_ATTACKING, UNIT_DEAD
};

enum unitFaction {
	FREE_MEN_UNIT, SAURON_ARMY_UNIT
};

enum unitDirection {
	DOWN, DOWN_LEFT, DOWN_RIGHT, LEFT, RIGHT, UP_LEFT, UP_RIGHT, UP
};

class Unit : public Entity
{
public:
	Unit();
	Unit(int posX, int posY, bool isEnemy, Unit* unit);
	~Unit();

	bool Update(float dt);
	bool Draw();

	unitType GetType()const;
	int GetLife() const;
	void SetPos(int posX, int posY);
	void SetSpeed(int amount);
	void SetDestination(iPoint destination);
	void Move(float dt);
	void CalculateVelocity();
	void LookAt();
	void SetAnim(unitDirection currentDirection);
	void AttackEnemyUnit(float dt);
	void AttackEnemyBuilding(float dt);
	void GatherResource(float dt);
	void Dead();
	void SetState(unitState state);

private:

public:
	unitType type = ELVEN_ARCHER;
	unitFaction faction;
	unitDirection direction = RIGHT;
	float unitAttackSpeed = 0;
	int unitPiercingDamage = 0;
	float unitMovementSpeed = 0;
	bool isEnemy = false;
	list<iPoint> path;
	bool destinationReached = true;
	fPoint velocity = { 0,0 };
	iPoint destinationTile = { 0,0 };
	iPoint destinationTileWorld = { 0,0 };
	float attackSpeed = 0;
	float timer = 0;
	int hpBarWidth = 40;
	unitDirection currentDirection = RIGHT;
	SDL_Texture* unitIdleTexture = nullptr;
	SDL_Texture* unitMoveTexture = nullptr;
	SDL_Texture* unitAttackTexture = nullptr;
	SDL_Texture* unitDieTexture = nullptr;
	unitState state = UNIT_IDLE;
	Unit* attackUnitTarget = nullptr;
	Building* attackBuildingTarget = nullptr;
	int unitLife = 0;
	int unitMaxLife= 0;
	int unitAttack = 0;
	int unitDefense = 0;
	bool isVisible = true;
	bool isSelected = false;
	Resource* resourceTarget = nullptr;
	int unitRange = 0;
	SDL_Rect r = { 0,0,0,0 };
	int unitRangeOffset = 0;
	bool checkTile = true;

	//Animations
	vector<Animation> idleAnimations;
	vector<Animation> movingAnimations;
	vector<Animation> attackingAnimations;
	vector<Animation> dyingAnimations;

	//villager
	vector<Animation> cuttingAnimations;
	vector<Animation> minningAnimations;
	vector<Animation> collectingAnimalsAnimations;
	vector<Animation> collectingPlantsAnimations;

	Animation* currentAnim = nullptr;

	//////
	bool isHero = false;
	bool Hero_Special_Attack();
	Timer time_inactive;
	Timer time_active;
	bool clicked = false;
};

#endif // !__UNIT_H__


