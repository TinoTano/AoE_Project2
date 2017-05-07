#ifndef _Order_
#define _Order_

#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "p2Point.h"
#include "Collision.h"
#include "Timer.h"

enum Order_state {
	NEEDS_START, EXECUTING, COMPLETED
};

enum OrderType {
	MOVETO, ATTACK, FOLLOWPATH, GATHER, BUILD, CREATE, REACH
};

class Order {
public:

	Order_state state = NEEDS_START;
	OrderType order_type;

public:

	virtual void Start(Entity* entity) {};
	virtual void Execute() {};
	virtual bool CheckCompletion() { return true; };

};


class MoveToOrder : public Order {

public:

	iPoint destinationMap = { -1, -1 };
	Unit* unit = nullptr;

public:

	MoveToOrder(iPoint destination) : destinationMap(destination) { order_type = MOVETO; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();

};


class FollowPathOrder : public Order {

public:

	Unit* unit = nullptr;

public:

	FollowPathOrder() { order_type = FOLLOWPATH; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();

};


class ReachOrder : public Order {

public:

	Entity* entity = nullptr;
	Unit* unit = nullptr;

public:

	ReachOrder(Entity* argentity) : entity(argentity) { order_type = REACH; }

	void Start(Entity* argunit);
	void Execute();
	bool CheckCompletion();

};

class AttackOrder : public Order {

public:

	Entity* target = nullptr;
	Unit* unit = nullptr;
	Building* building = nullptr;

public:

	AttackOrder(Entity* argtarget) : target(argtarget) { order_type = ATTACK; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();
};


class GatherOrder : public Order {

public:

	Villager* villager = nullptr;
	Resource* resource = nullptr;

public:

	GatherOrder(Resource* argresource = nullptr) : resource(argresource) { order_type = GATHER; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();
};


class BuildOrder : public Order {

public:

	Villager* villager = nullptr;
	Building* building = nullptr;

public:

	BuildOrder(Building* argbuilding) : building(argbuilding) { order_type = BUILD; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();

};



class CreateUnitOrder : public Order {

public:

	Building* building = nullptr;
	unitType type = VILLAGER;
	Timer timer;

public:

	CreateUnitOrder(unitType argtype) : type(argtype) { order_type = CREATE; }

	void Start(Entity* entity);
	void Execute();
	bool CheckCompletion();

};

#endif