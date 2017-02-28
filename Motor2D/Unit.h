#ifndef __UNIT_H__
#define __UNIT_H__

#include "p2Point.h"
#include "Entity.h"
#include "Animation.h"
#include <list>
#include "Input.h"
#include "Map.h"

using namespace std;

enum unitType {
	ELVEN_LONGBLADE, DWARVEN_MULER, GONDOR_SPEARMAN, SOLDIER, ELVEN_ARCHER, DUNEDAIN_RANGER, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN, SIEGE_TOWER, LIGHT_CATAPULT, GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER,
	VENOMOUS_SPIDER, HARADRIM_OLIFANT, MOUNTED_NAZGUL, TROLL_MAULER
};

enum unitState
{
	IDLE, MOVING, ATTACKING, DEAD
};

enum unitFaction {
	FREE_MEN, SAURON_ARMY
};

enum unitDirection {
	NONE, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT
};

class Unit : public Entity
{
public:
	Unit(int posX, int posY, bool isEnemy, unitType type, unitFaction race);
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
	void Dead();
	void SetState(unitState state);

private:
	unitType type;
	unitFaction race;
	unitDirection direction;
	uint unitLife;
	uint unitAttack;
	uint unitDefense;
	float unitAttackSpeed;
	int unitPiercingDamage;
	float unitMovementSpeed;
	bool isEnemy;
	list<iPoint> path;
	bool destinationReached = true;
	fPoint velocity;
	iPoint destinationTile;
	iPoint destinationTileWorld;
	iPoint ret;
	float attackDelay;
	float timer = 0;

public:
	unitState state;
	unitDirection currentDirection;
	SDL_Texture* unitIdleTexture;
	SDL_Texture* unitMoveTexture;
	SDL_Texture* unitAttackTexture;
	SDL_Texture* unitDieTexture;
	Unit* attackUnitTarget;

	//Animations

	//Idle directions
	Animation idleUp;
	Animation idleDown;
	Animation idleRight;
	Animation idleLeft;
	Animation idleUpRight;
	Animation idleDownRight;
	Animation idleUpLeft;
	Animation idleDownLeft;

	//Move directions
	Animation movingUp;
	Animation movingDown;
	Animation movingRight;
	Animation movingLeft;
	Animation movingUpRight;
	Animation movingDownRight;
	Animation movingUpLeft;
	Animation movingDownLeft;

	//Attack directions
	Animation attackingUp;
	Animation attackingDown;
	Animation attackingRight;
	Animation attackingLeft;
	Animation attackingUpRight;
	Animation attackingDownRight;
	Animation attackingUpLeft;
	Animation attackingDownLeft;

	//Death directions
	Animation dyingUp;
	Animation dyingDown;
	Animation dyingRight;
	Animation dyingLeft;
	Animation dyingUpRight;
	Animation dyingDownRight;
	Animation dyingUpLeft;
	Animation dyingDownLeft;

	Animation* currentAnim = nullptr;

};

#endif // !__UNIT_H__

