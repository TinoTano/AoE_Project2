#ifndef _TechTree_
#define _TechTree_

#include "Module.h"
#include "Building.h"
#include "Entity.h"
#include "Unit.h"

enum TechMultiplier {

	//units 
	MELEE_UNIT_ATTACK, RANGED_UNIT_ATTACK, CAVALRY_ATTACK,    MELEE_UNIT_DEFENSE, RANGED_UNIT_DEFENSE, CAVALRY_DEFENSE,
	MELEE_UNIT_SPEED, RANGED_UNIT_SPEED, CAVALRY_SPEED,       MELEE_UNIT_COST, RANGED_UNIT_COST, CAVALRY_COST,

	//
	MAX_MULTIPLIERS
};

enum TechList {

	BASIC_TECH,

	ENHANCED_BLACKSMITHING,
	MOUNTAIN_FORGE,
	DWARVEN_STEEL,
	DWARVEN_TACTICS

};

struct Tech {
	
	string name;
	string desc;
	int id;

	list<pair<int, buildingType>> unlocks_techs;
	list<pair<unitType, buildingType>> unlocks_units;
	list<pair<float, TechMultiplier>> multipliers;
	list<buildingType> unlocks_buildings;

	buildingType researched_in;
	Cost cost;
	int research_time;
	Timer research_timer;

};


class TechTree {
public:
	vector<list<unitType>> available_units;
	vector<list<int>> available_techs;
	list<buildingType> available_buildings;
	vector<float> multiplier_list;

	vector<Tech*> all_techs;

public:

	void Start(pugi::xml_node gameData);
	void Update();

	void LoadTechTree(pugi::xml_node gameData);

	void StartResearch(int tech_id);
	void Researched(int tech_id);

};


#endif
