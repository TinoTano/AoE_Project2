#ifndef _GAMEFACTION_
#define _GAMEFACTION_

#include "Entity.h"
#include <list>

class TechTree;
class Unit;
class Villager;
class Building;

class GameFaction{
public:

	GameFaction(Faction argfaction) : faction(argfaction) {};

	StoredResources resources;
	Faction faction;
	TechTree* tech_tree;

	Building* Town_center;

	list<Unit*> units;
	list<Villager*> villagers;
	list<Building*> buildings;

};

#endif // !_GAMEFACTION_

