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
	ELVEN_LONGBLADE, DWARVEN_MAULER, GONDOR_SPEARMAN, ELVEN_ARCHER, DUNEDAIN_RANGER, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN, SIEGE_TOWER, LIGHT_CATAPULT,

	GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER, VENOMOUS_SPIDER, HARADRIM_OLIFANT, NAZGUL, TROLL_MAULER
};

enum unitState
{
	UNIT_IDLE, UNIT_MOVING, UNIT_ATTACKING, UNIT_DEAD
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
  bool IsEnemy() const;
	void SetPos(int posX, int posY);
	void SetSpeed(int amount);
	void SetDestination(iPoint destination);
	void Move(float dt);
	void CalculateVelocity();
	void LookAt();
	void SetAnim(unitDirection currentDirection);
	void AttackEnemy(float dt);
	void Dead();
	void SetState(unitState state);
	pugi::xml_node LoadUnitInfo(unitType type);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:

public:
	unitType type = ELVEN_ARCHER;
	Faction faction;
	unitDirection direction = RIGHT;
	float unitAttackSpeed = 0;
	int unitPiercingDamage = 0;
	float unitMovementSpeed = 0;
	list<iPoint>* path;

	fPoint velocity = { 0,0 };
	iPoint destinationTileWorld = { 0,0 };
	float attackSpeed = 0;
	int hpBarWidth = 40;
	unitDirection currentDirection = RIGHT;
	SDL_Texture* unitIdleTexture = nullptr;
	SDL_Texture* unitMoveTexture = nullptr;
	SDL_Texture* unitAttackTexture = nullptr;
	SDL_Texture* unitDieTexture = nullptr;
	unitState state = UNIT_IDLE;
	Entity* attackTarget = nullptr;
	bool isVisible = true;
	iPoint next_step = { 0,0 };


	//Animations
	vector<Animation> idleAnimations;
	vector<Animation> movingAnimations;
	vector<Animation> attackingAnimations;
	vector<Animation> dyingAnimations;
	Animation* currentAnim = nullptr;

	// My changes --------------------------------------------

	bool isHero = false;
	bool Hero_Special_Attack();
	Timer time_inactive;
	Timer time_active;
	bool clicked = false;
	
	// -------------------------------------------------------
};

#endif // !__UNIT_H__


