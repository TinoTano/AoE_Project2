#ifndef _SQUAD_
#define _SQUAD_

#include "Unit.h"

class Order;

class Squad {

public:
	list<Unit*> units;
	Unit* commander = nullptr;
	unitType type = ORC_SOLDIER;
	EntityState state = IDLE;
	list<Order*> squad_orderlist;

public:

	Squad() { Start(); }

	void Start();
	void Assign(Unit* unit);
	void RestoreUnits();
	void ClearOrders();
	bool IsRestored();

};

#endif
