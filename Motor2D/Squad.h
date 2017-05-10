#ifndef _SQUAD_
#define _SQUAD_

#include "Unit.h"

class Squad {

public:
	list<Entity*> units;
	Unit* commander = nullptr;
	unitType type = ORC_SOLDIER;
	EntityState state = IDLE;

public:

	Squad() { Start(); }

	void Start();
	void Assign(Unit* unit);
	void RestoreUnits();
	void Attack();
	void Explore();

};

#endif
