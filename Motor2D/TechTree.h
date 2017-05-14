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

enum TechType {

	NO_TECH,

	RANGED_WEAPONS = 1,					ORC_MINES = 1,
	HORSE_TRAINING = 2,					GUTS_COMMERCE = 2,
	TOWN_MILITIA = 3,					STEEL_MINES = 3,

	ENHANCED_BLACKSMITHING = 4,			SKIRMISH_TACTICS = 4,
	MOUNTAIN_FORGE = 5,					BEASTS_TAMING = 5,
	DWARVEN_STEEL = 6,					SARUMAN_SUPPORT = 6,
	DWARVEN_TACTICS = 7,				HARADRIM_REINFORCEMENTS = 7,

	ENHANCED_TRAINING = 8,				GIANT_BREEDING = 8,
	ELVEN_TACTICS = 9,					CALL_TO_THE_NINE = 9,
	ELVEN_STEEL = 10,              
	LIBRARY = 11,                  

	ENHANCED_VILLAGERS = 12,       
	HUMAN_WARFARE = 13,            
	ROHAN_HORSES = 14,             
	DUNEDAIN_OUTPOST = 15,

};

struct Tech {
	
	Tech() {
		unlocks_building = (buildingType)-1;
		unlocks_unit.first = (unitType)-1;
		multipliers.first = -1;

	}

	string name;
	string desc;
	int id;

	list<TechType> unlocks_tech;
	pair<unitType, buildingType> unlocks_unit;
	pair<float, TechMultiplier> multipliers;
	buildingType unlocks_building;

	buildingType researched_in;
	bool researching = false;
	Cost cost;
	int research_time;
	int aux_timer;
	Timer research_timer;

};


class TechTree {
public:
	list<pair<unitType, buildingType>> available_units;
	list<TechType> available_techs;
	list<buildingType> available_buildings;
	vector<float> multiplier_list;

	vector<Tech*> all_techs;

public:

	TechTree();

	void Reset(Faction faction);
	void Update();

	void LoadTechTree(pugi::xml_node gameData);

	void StartResearch(TechType tech_id);
	void Researched(TechType tech_id);

};


#endif
