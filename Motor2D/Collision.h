#ifndef __Collision_H__
#define __Collision_H__

#include "Module.h"
#include "p2Point.h"

class Unit;
class Entity;
class Building;
class Resource;
class QuadTree;

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_UNIT,
	COLLIDER_BUILDING,
	COLLIDER_RESOURCE,
	COLLIDER_LOS,
	COLLIDER_RANGE,


	COLLIDER_MAX
};

enum Collision_state {

	UNSOLVED,
	SOLVING,
	SOLVED

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
	Resource* GetResource();
};

struct Collision_data {

	Collider* c1;
	Collider* c2;
	Collision_state state = UNSOLVED;

	Collision_data(Collider* c1, Collider* c2) : c1(c1), c2(c2)
	{}

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
	bool FindCollision(Collider* col1, Collider* col2);

	Collider* FindNearestCollider(iPoint point);
	bool IsOccupied(iPoint worldPos);
	void DebugDraw();

private:

	list<Collider*> colliders;
	bool debug = false;
	
	list<Collider*> potential_collisions;

public:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	list<Collision_data*> collision_list;
	QuadTree* quadTree;
};

#endif // __ModuleCollision_H__

