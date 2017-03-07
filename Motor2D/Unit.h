#ifndef __UNIT_H__
#define __UNIT_H__

#include "p2Point.h"
#include "Entity.h"
#include "Animation.h"
#include <list>
#include <vector>
#include "Input.h"
#include "Map.h"

class Building;

enum unitType {
	ELVEN_LONGBLADE, DWARVEN_MAULER, GONDOR_SPEARMAN, ELVEN_ARCHER, DUNEDAIN_RANGER, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN, SIEGE_TOWER, LIGHT_CATAPULT,

	GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER, VENOMOUS_SPIDER, HARADRIM_OLIFANT, NAZGUL, TROLL_MAULER
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
	Unit(int posX, int posY, bool isEnemy, unitType type);
	~Unit();

	bool Update(float dt);
	bool Draw();

	unitType GetType()const;
	int GetLife() const;
	void SetPos(int posX, int posY);
	void SetSpeed(int amount);
	void SetDestination();
	void Move(float dt);
	void CalculateVelocity();
	void LookAt();
	void SetAnim(unitDirection currentDirection);
	void AttackEnemyUnit(float dt);
	void AttackEnemyBuilding(float dt);
	void Dead();
	void SetState(unitState state);
	pugi::xml_node LoadUnitInfo(unitType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:
	unitType type;
	unitFaction faction;
	unitDirection direction;
	float unitAttackSpeed;
	int unitPiercingDamage;
	float unitMovementSpeed;
	bool isEnemy;
	list<iPoint> path;
	bool destinationReached = true;
	fPoint velocity;
	iPoint destinationTile;
	iPoint destinationTileWorld;
	float attackSpeed;
	float timer = 0;
	int hpBarWidth;
	unitDirection currentDirection;
	SDL_Texture* unitIdleTexture;
	SDL_Texture* unitMoveTexture;
	SDL_Texture* unitAttackTexture;
	SDL_Texture* unitDieTexture;
	unitState state;

public:
	Unit* attackUnitTarget;
	Building* attackBuildingTarget;
	int unitLife;
	int unitMaxLife;
	int unitAttack;
	int unitDefense;
	bool isVisible;
	bool isSelected;

	//Animations
	vector<Animation> idleAnimations;
	vector<Animation> movingAnimations;
	vector<Animation> attackingAnimations;
	vector<Animation> dyingAnimations;

	Animation* currentAnim = nullptr;

};

#endif // !__UNIT_H__


