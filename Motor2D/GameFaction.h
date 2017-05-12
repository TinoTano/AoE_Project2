#ifndef _GAMEFACTION_
#define _GAMEFACTION_

#include "Entity.h"
#include "TechTree.h"
#include <list>

class Unit;
class Villager;
class Building;

class GameFaction{
public:

	GameFaction(Faction argfaction) : faction(argfaction) { tech_tree = new TechTree(); Reset(); }

	~GameFaction() 
	{
		RELEASE(tech_tree);
	}

	void Reset() {
		resources.SetToZero();
		Town_center = nullptr;

		units.clear();
		villagers.clear();
		buildings.clear();

		tech_tree->Reset(faction);
	}

	StoredResources resources;
	Faction faction;
	TechTree* tech_tree;

	Building* Town_center;

	list<Unit*> units;
	list<Villager*> villagers;
	list<Building*> buildings;

};

#endif // !_GAMEFACTION_

