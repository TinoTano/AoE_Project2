#ifndef __Collision_H__
#define __Collision_H__

#include "Module.h"
#include "p2Point.h"

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
	iPoint pos;
	int r;
	bool to_delete = false;
	bool colliding = false;
	COLLIDER_TYPE type;
	Entity* entity;
	Module* callback;

	Collider(iPoint position, int radius, COLLIDER_TYPE type, Module* callback, Entity* entity) :
		pos(position),
		type(type),
		r(radius),
		callback(callback),
		entity(entity)
	{}

	void SetPos(int x, int y)
	{
		pos.x = x;
		pos.y = y;
	}

	bool CheckCollision(Collider* c2) const;
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

	Collider* AddCollider(iPoint position, int radius, COLLIDER_TYPE type, Module* callback = nullptr, Entity* entity = nullptr);
	void DeleteCollider(Collider* collider);
	void DebugDraw();

private:

	list<Collider*> colliders;
	bool debug = false;

public:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};

#endif // __ModuleCollision_H__

