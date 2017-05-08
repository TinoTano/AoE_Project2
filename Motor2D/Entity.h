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

	int woodCost = 0;
	int stoneCost = 0;
	int foodCost = 0;
	int goldCost = 0;

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
	int entityID;
	bool isActive = false;
	SDL_Texture* entityTexture;
	iPoint entityPosition;
	Collider* collider;
	EntityState state = IDLE;
	Faction faction = FREE_MEN;

	int Life = 0;
	int MaxLife = 0;
	int Attack = 0;
	int Defense = 0;

	int last_life = 0;
	Timer lifebar_timer;

};

#endif // !__ENTITY_H__