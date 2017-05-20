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

	RANGED_WEAPONS = 0,					ORC_MINES = 0,
	HORSE_TRAINING = 1,					GUTS_COMMERCE = 1,
	TOWN_MILITIA = 2,					STEEL_MINES = 2,

	ENHANCED_BLACKSMITHING = 3,			SKIRMISH_TACTICS = 3,
	MOUNTAIN_FORGE = 4,					BEASTS_TAMING = 4,
	DWARVEN_STEEL = 5,					SARUMAN_SUPPORT = 5,
	DWARVEN_TACTICS = 6,				HARADRIM_REINFORCEMENTS = 6,

	ENHANCED_TRAINING = 7,				GIANT_BREEDING = 7,
	ELVEN_TACTICS = 8,					CALL_TO_THE_NINE = 8,
	ELVEN_STEEL = 9,              
	LIBRARY = 10,                  

	ENHANCED_VILLAGERS = 11,       
	HUMAN_WARFARE = 12,            
	ROHAN_HORSES = 13,             
	DUNEDAIN_OUTPOST = 14,

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
