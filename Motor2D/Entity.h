#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "PugiXml\src\pugixml.hpp"

using namespace std;

struct Collider;

enum Faction {
	FREE_MEN, NATURE, SAURON_ARMY
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
	virtual void Dead();
	 
	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;


public:
	int entityID;
	bool isActive = false;
	SDL_Texture* entityTexture;
	iPoint entityPosition;
	Collider* collider;
	Faction faction = NATURE;

	int Life = 0;
	int MaxLife = 0;
	int Attack = 0;
	int Defense = 0;
};

#endif // !__ENTITY_H__


