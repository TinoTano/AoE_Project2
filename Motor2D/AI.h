#ifndef _AI_H_
#define _AI_H_

#include "Module.h"
#include "Timer.h" 
#include "Unit.h"
#include "Villager.h"
#include "TechTree.h"
#include "Building.h"
#include "Pathfinding.h"
#include "Resource.h"
#include "Application.h"
#include "GameFaction.h"
#include "Orders.h"

#include <cstdlib>
#include <deque>

enum AI_state {
	NULL_STATE, EXPANDING, DEFENSIVE, OFFENSIVE, MAX_STATES
};


class AI : public Module {
public:

	AI(){ name = "AI"; };

	// Destructor
	virtual ~AI() {};

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start() { return true; };

	// Called before all Updates
	bool PreUpdate() { return true; };

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate() { return true; };

	// Called before quitting
	bool CleanUp() { return true; };

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void QueueUnits();
	void SelectBuilding(AI_state ai_state);
	void ChangeState();
	void LaunchAttack();


public:

	// general utilities
	Timer AI_timer;
	GameFaction* Enemies = nullptr;
	AI_state state = EXPANDING;
	AI_state forced_state = NULL_STATE;
	bool enabled = false;

	Building* selected_building = nullptr;
	Building* forced_building = nullptr;
	list<Unit*> last_attack_squad;
};

#endif
