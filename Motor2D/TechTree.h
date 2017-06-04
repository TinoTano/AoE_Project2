#ifndef _TechTree_
#define _TechTree_

#include "Module.h"
#include "Building.h"
#include "Entity.h"
#include "Unit.h"

enum TechMultiplier {

	//units 
	MELEE_UNIT_ATTACK, RANGED_UNIT_ATTACK, CAVALRY_ATTACK, MELEE_UNIT_DEFENSE, RANGED_UNIT_DEFENSE, CAVALRY_DEFENSE,
	MELEE_UNIT_SPEED, RANGED_UNIT_SPEED, CAVALRY_SPEED, MELEE_UNIT_COST, RANGED_UNIT_COST, CAVALRY_COST,

	//
	MAX_MULTIPLIERS
};

enum TechType {

	ENHANCED_VILLAGERS = 0, ORC_MINES = 0,
	RANGED_WEAPONS = 1, GUTS_COMMERCE = 1,
	ELVEN_STEEL = 2, STEEL_MINES = 2,
	HORSE_TRAINING = 3, SKIRMISH_TACTICS = 3,
	MOUNTAIN_FORGE = 4, BEASTS_TAMING = 4,
	ELVEN_TACTICS = 5, SARUMAN_SUPPORT = 5,
	ENHANCED_BLACKSMITHING = 6, HARADRIM_REINFORCEMENTS = 6,

	TOWN_MILITIA = 7, GIANT_BREEDING = 7,
	LIBRARY = 8, CALL_TO_THE_NINE = 8,
	ROHAN_HORSES = 9,
};

struct Tech {

	Tech() {
		unlocks_building = (buildingType)-1;
		unlocks_unit.front().first = (unitType)-1;
		multipliers.first = -1;
	}

	string name;
	string desc;
	int id;

	list<TechType> unlocks_tech;
	list<pair<unitType, buildingType>> unlocks_unit;
	pair<float, TechMultiplier> multipliers;
	buildingType unlocks_building;
	buildingType researched_in;
	bool researching = false;
	Cost cost;
	int research_time;
	int aux_timer;
	Timer research_timer;
	SDL_Rect minature;

};


class TechTree {
public:
	list<pair<unitType, buildingType>> available_units;
	list<TechType> available_techs;
	list<buildingType> available_buildings;
	vector<float> multiplier_list;

	vector<Tech*> all_techs;

public:

	bool researching_tech = false;
	TechTree();

	void Reset(Faction faction);
	void Update();

	void LoadTechTree(pugi::xml_node gameData);

	void StartResearch(TechType tech_id);
	void Researched(TechType tech_id);


};


#endif
