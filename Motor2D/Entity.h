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
	DESTROYED, CONSTRUCTING, ATTACKING, GATHERING, MOVING, IDLE, BEING_BUILT
};

struct Cost {

	int wood = 0;
	int stone = 0;
	int food = 0;
	int gold = 0;

	bool Spend(Cost cost);
	void SetToZero();

};

class Entity
{
public:
	Entity();
	~Entity();
	iPoint GetPosition() const;
	int GetEntityID() const;

	virtual bool Update(float dt);
	virtual bool Draw();
	virtual void Destroy();
	 
	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;
	void drawLife(iPoint barPos);


public:
	int entityID = 0;
	bool isActive = false;
	string name;
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

	iPoint selectionAreaCenterPoint = { 0,0 };

};

typedef Cost StoredResources;

#endif // !__ENTITY_H__