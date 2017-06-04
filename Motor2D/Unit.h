#ifndef _UNIT_H_
#define _UNIT_H_

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
class Squad;

enum unitType {
	ELVEN_LONGBLADE, DWARVEN_MAULER, GONDOR_SPEARMAN, ELVEN_ARCHER, DUNEDAIN_RANGE, ELVEN_CAVALRY, GONDOR_KNIGHT,
	ROHAN_KNIGHT, MOUNTED_DUNEDAIN,

	GOBLIN_SOLDIER, ORC_SOLDIER, URUK_HAI_SOLDIER, ORC_ARCHER, VENOMOUS_SPIDER, HARADRIM_OLIFANT, NAZGUL, TROLL_MAULER,

	SLAVE_VILLAGER, ELF_VILLAGER,

	LEGOLAS, GANDALF, BALROG
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

	void LookAt(fPoint dest);
	void SetTexture(EntityState state);
	void SubordinatedMovement(iPoint p);

public:
	// STATS
	unitType type = ELVEN_ARCHER;
	int unitPiercingDamage = 0;
	float unitMovementSpeed = 0;
	int cooldown_time = 0;
	int range_value = 0;
	int los_value = 0;
	bool IsVillager = false;
	bool IsHero = false;
	Cost cost;
	
	// Utilities
	SDL_Rect r = { 0,0,0,0 };
	unitDirection currentDirection = RIGHT;
	list<iPoint> path;
	iPoint destinationTileWorld = { 0,0 };
	Collider* los = nullptr;
	Collider* range = nullptr;
	list<Order*> order_list;
	Order* sub_movement = nullptr;
	int selectionRadius = 0;
	Squad* squad = nullptr;

	//Animations
	vector<Animation> idleAnimations;
	vector<Animation> movingAnimations;
	vector<Animation> attackingAnimations;
	vector<Animation> dyingAnimations;

	SDL_Texture* unitIdleTexture = nullptr;
	SDL_Texture* unitMoveTexture = nullptr;
	SDL_Texture* unitAttackTexture = nullptr;
	SDL_Texture* unitDieTexture = nullptr;

	vector<Animation>* currentAnim = nullptr;

	iPoint next_pos = { 0,0 };

};



#endif // !__UNIT_H__


