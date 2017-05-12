#pragma once


#include "Module.h"
#include "Timer.h" 
#include "Unit.h"
#include "Villager.h"
#include "TechTree.h"
#include "Building.h"
#include "Pathfinding.h"
#include "Squad.h"
#include "Resource.h"
#include "Application.h"
#include "GameFaction.h"
#include "Orders.h"

#include <cstdlib>
#include <deque>

#define EXPANSION_VAR_VAL (250 * expansion_level)
#define EXPANSION_FIX_VAL 1000

#define FOOD_PROPORTION 0.35
#define WOOD_PROPORTION 0.35
#define STONE_PROPORTION 0.15
#define GOLD_PROPORTION 0.15

enum AI_state {
	EXPANDING, DEFENSIVE, OFFENSIVE
};


class AI : public Module {
public:

	AI(){ name = "AI"; };

	// Destructor
	virtual ~AI() {};

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate() { return true; };

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate() { return true; };

	// Called before quitting
	bool CleanUp() { return true; };

	void ManageAttack();
	void StartAttack();

	void ManageUnitRequests();
	void ManageBuildRequests();
	void ManageVillagerRequests();
	void ManageTechRequests();

	void FillResourceRequests();

	void IncreaseExpansionLevel();

	iPoint PlaceBuilding(buildingType type);
	void LoadExplorationMap();
	void LoadAI_Data(pugi::xml_node& gameData);

public:

	// general utilities
	Timer AI_timer;
	GameFaction* Enemies;
	int expansion_level = 0;
	AI_state state = EXPANDING;
	bool enabled = false;

	//expansion management
	list<buildingType> buildings_to_build;
	vector<list<buildingType>> expansion_build_table;
	list<TechType> techs_to_research;
	vector<list<TechType>> expansion_tech_table;
	vector<int> villager_expansion_table;

	//army management
	int squad_size = 7;
	list<Squad*> defensive_squads;
	list<Squad*> offensive_squads;
	deque<Entity*> targets;
	deque<Entity*> threats;
	list<iPoint> exploration_points;

	//requests management
	void Fetch_AICommand(Villager* villager);
	list<Order*> villager_requests;  // resource + build requests
	list<pair<unitType, Squad*>> unit_requests;

	//villager management
	int requested_villagers = 0;

};
