#ifndef __Collision_H__
#define __Collision_H__

#include "Module.h"

class Unit;
class Entity;
class Building;

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_FRIENDLY_UNIT,
	COLLIDER_ENEMY_UNIT,
	COLLIDER_FRIENDLY_BUILDING,
	COLLIDER_ENEMY_BUILDING,
	COLLIDER_RESOURCE,


	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	Entity* entity;
	Module* callback;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, Module* callback = nullptr, Entity* entity = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback),
		entity(entity)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
	Unit* GetUnit();
	Building* GetBuilding();
};

class Collision : public Module
{
public:

	Collision();
	virtual ~Collision();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, Module* callback = nullptr, Entity* entity = nullptr);
	void DeleteCollider(Collider* collider);
	void DebugDraw();

private:

	list<Collider*> colliders;
	bool debug = false;

public:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};

#endif // __ModuleCollision_H__

