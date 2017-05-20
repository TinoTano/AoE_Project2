#ifndef _ORDERS_
#define _ORDERS_

#include "p2Point.h"

class Unit;
class Squad;

enum Order_state {
	NEEDS_START, EXECUTING, COMPLETED
};

enum OrderType {
	MOVETO, ATTACK, FOLLOW, GATHER, BUILD, CREATE, REACH, SQUADMOVETO, SQUADATTACK
};

class Order {
public:

	Order_state state = NEEDS_START;
	OrderType order_type;

public:

	virtual void Start(Unit* unit) {};   
	virtual void Execute(Unit* unit) {};
	virtual bool CheckCompletion(Unit* unit) { return true; };

};


class MoveToOrder : public Order {

public:

	MoveToOrder(Unit* unit, iPoint destWorld);

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);

};

class UnitAttackOrder : public Order {
public:

	UnitAttackOrder() { order_type = ATTACK; }

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);
};



class GatherOrder : public Order {
public:

	GatherOrder() { order_type = GATHER; }

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);
};


class BuildOrder : public Order {
public:

	BuildOrder() { order_type = BUILD; }

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);

};


class FollowOrder : public Order {

public:

	FollowOrder() { order_type = FOLLOW; }

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);

};

class SquadMoveToOrder : public Order {

public:

	SquadMoveToOrder(Unit* unit, iPoint dest);

	void Start(Unit* unit);
	void Execute(Unit* unit);
	bool CheckCompletion(Unit* unit);

};




#endif