
#ifndef _AI_
#define _AI_

#include "Module.h"
#include "Timer.h"
#include "Unit.h"
#include "Villager.h"
#include "TechTree.h"
#include "Building.h"
#include "Resource.h"

class AI : public Module {
public:

	AI();

	// Destructor
	virtual ~AI();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ManageExpenses();
	void ManageDefense();

	void ManageVillagers();
	void FillResourceRequests();
	iPoint PlaceBuilding(buildingType type);

	void ManageBuildings();

private:

public:

	Timer AI_timer;
	TechTree* enemy_techtree = nullptr;

	//unit management
	int army_size = 0;
	list<Unit*> units_defending;
	list<Unit*> units_exploring;
	list<Unit*> units_attacking;

	//Building management
	list<unitType> unit_requests;
	list<int> tech_requests;

	//villager management
	list<Unit*> idle_villagers;
	int gatherers = 0;
	list<resourceType> resource_requests;
	list<Building*> build_requests;

	int food_requests = 0;
	int wood_requests = 0;
	int stone_requests = 0;
	int gold_requests = 0;

};

#endif // !AI_
