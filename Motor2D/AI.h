#ifndef _AI_H_
#define _AI_H_

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

	Squad* AssignUnit(Unit* unit);
	void ManageAttack();
	void StartAttack();

	void UpdateThreats(Entity* entity);
	void UpdateTargets(Entity* entity);
	void RemoveThreats(Entity* entity);

	void ManageUnitRequests();
	void ManageBuildRequests();
	void ManageVillagerRequests();
	void ManageTechRequests();

	void FillResourceRequests();
	iPoint PlaceBuilding(buildingType type);

	void IncreaseExpansionLevel();

	void LoadExplorationMap();
	void LoadAI_Data(pugi::xml_node& gameData);

public:

	// general utilities
	Timer AI_timer;
	GameFaction* Enemies = nullptr;
	int expansion_level = 0;
	AI_state state = EXPANDING;
	bool enabled = false;

	//expansion management
	vector<list<buildingType>> expansion_build_table;
	vector<list<TechType>> expansion_tech_table;
	vector<int> villager_expansion_table;

	//army management
	int squad_size = 3;
	list<Squad*> defensive_squads;
	list<Squad*> offensive_squads;
	list<pair<Entity*, iPoint>> targets;
	list<pair<Entity*, iPoint>> threats;
	list<iPoint> exploration_points;

	//requests management
	void Fetch_AICommand(Villager* villager);
	list<resourceType> resource_requests; 
	list<unitType> unit_requests;
	list<buildingType> build_requests;
	list<TechType> tech_requests;

	//villager management
	int requested_villagers = 0;

};

#endif
