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
class Order;

enum unitType {
	ELVEN_LONGBLADE, DWARVEN_MAULER, GONDOR_SPEARMAN, ELVEN_ARCHER, DUNEDAIN_RANGE, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN, SIEGE_TOWER, LIGHT_CATAPULT,

	GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER, VENOMOUS_SPIDER, HARADRIM_OLIFANT, NAZGUL, TROLL_MAULER,

	VILLAGER, ELF_VILLAGER,

	GONDOR_HERO
};

enum unitDirection {
	DOWN, DOWN_LEFT, DOWN_RIGHT, LEFT, RIGHT, UP_LEFT, UP_RIGHT, UP
};

class Unit : public Entity
{
public:
	Unit();
	Unit(int posX, int posY, Unit* unit);
	~Unit();

	bool Update(float dt);
	bool Draw();
	void Destroy();

	unitType GetType()const;
	int GetLife() const;
	void SetPos(int posX, int posY);
	void SetSpeed(int amount);
	bool SetDestination(iPoint destination);
	void CalculateVelocity();
	void LookAt();
	void SetAnim(EntityState state);
	void SetTexture(EntityState state);

private:

public:
	// STATS
	unitType type = ELVEN_ARCHER;
	float unitAttackSpeed = 0;
	int unitPiercingDamage = 0;
	float unitMovementSpeed = 0;
	float attackSpeed = 0;
	bool IsVillager = false;
	bool IsHero = false;
	bool isGuard = false;
	Cost cost;
	
	// Utilities
	EntityState state = IDLE;
	SDL_Rect r = { 0,0,0,0 };
	iPoint next_step = { 0,0 };
	unitDirection currentDirection = RIGHT;
	list<iPoint>* path;
	fPoint velocity = { 0,0 };
	iPoint destinationTileWorld = { 0,0 };
	Collider* los = nullptr;
	Collider* range = nullptr;
	list<Order*> order_list;
	Building* buildingToCreate = nullptr;


	//Animations
	vector<Animation> idleAnimations;
	vector<Animation> movingAnimations;
	vector<Animation> attackingAnimations;
	vector<Animation> dyingAnimations;

	SDL_Texture* unitIdleTexture = nullptr;
	SDL_Texture* unitMoveTexture = nullptr;
	SDL_Texture* unitAttackTexture = nullptr;
	SDL_Texture* unitDieTexture = nullptr;

	Animation* currentAnim = nullptr;

	iPoint prev_pos = { 0,0 };
	iPoint next_pos = { 0,0 };

};

#endif // !__UNIT_H__


