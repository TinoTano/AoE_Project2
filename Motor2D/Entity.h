#ifndef _ENTITY_
#define _ENTITY_

#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "Timer.h"
#include "PugiXml\src\pugixml.hpp"

using namespace std;

struct Collider;

enum Faction {
	FREE_MEN, NATURE, SAURON_ARMY
};

enum EntityState {
	DESTROYED, CONSTRUCTING, CREATING, MOVING, ATTACKING, PATROLLING, GATHERING, IDLE, BEING_BUILT
};

struct Cost {

	int wood = 0;
	int stone = 0;
	int food = 0;
	int gold = 0;

	int Total() {
		return wood + stone + food + gold;
	}

	bool Spend(Cost cost) {

		if (wood - cost.wood < 0 || food - cost.food < 0 || gold - cost.gold < 0 || stone - cost.stone < 0)
			return false;

		wood -= cost.wood; food -= cost.food; gold -= cost.gold; stone -= cost.stone;

		return true;
	}

	void SetToZero() 
	{
		wood = stone = gold = food = 0;
	}

};

class Entity
{
public:
	Entity();
	~Entity();
	iPoint GetPosition() const;
	int GetEntityID() const;
	void SetActive(bool active);

	virtual bool Update(float dt);
	virtual bool Draw();
	virtual bool HandleInput();
	virtual void Destroy();
	 
	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;
	void drawLife(iPoint barPos);


public:
	int entityID = 0;
	bool isActive = false;
	SDL_Texture* entityTexture = nullptr;
	iPoint entityPosition = { 0,0 };
	Collider* collider = nullptr;
	EntityState state = IDLE;
	Faction faction = FREE_MEN;

	int Life = 0;
	int MaxLife = 0;
	int Attack = 0;
	int Defense = 0;

	int last_life = 0;
	Timer lifebar_timer;

	uint imageWidth = 0;
	uint imageHeight = 0;
	iPoint selectionAreaCenterPoint = { 0,0 };

};

typedef Cost StoredResources;

#endif // !__ENTITY_H__