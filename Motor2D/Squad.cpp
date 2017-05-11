#include "Squad.h"
#include "Building.h"
#include "GameFaction.h"
#include "Unit.h"
#include "AI.h"


void Squad::Start() {

	vector<unitType> available_types;
	list<pair<unitType, buildingType>>* researched_units = &App->ai->Enemies->tech_tree->available_units;

	for (list<pair<unitType, buildingType>>::iterator it2 = researched_units->begin(); it2 != researched_units->end(); it2++) {
		for (list<Building*>::iterator it = App->ai->Enemies->buildings.begin(); it != App->ai->Enemies->buildings.end(); it++) {
			if ((*it2).second == (*it)->type) {
				available_types.push_back((*it2).first);
				break;
			}
		}
	}

	int random = rand() % available_types.size();

	type = available_types.at(random);
	state = IDLE;
	commander = nullptr;
}

void Squad::Assign(Unit* unit) {

	units.push_back(unit);

	if (commander == nullptr)
		commander = unit;
}

void Squad::RestoreUnits() {

	for (int i = units.size(); i <= App->ai->squad_size; i++) {
		pair<unitType, Squad*> unit_data{ type, this };
		App->ai->unit_requests.push_front(unit_data);
	}
}

void Squad::Attack() {

	if (commander->SetDestination(App->ai->targets.front()->entityPosition)) {

		if (units.size() > 1) {

			units.remove(commander);
			App->pathfinding->SharePath(commander, units);
			units.push_front(commander);

		}

		for (list<Entity*>::iterator it = units.begin(); it != units.end(); it++) {
			Unit* unit = (Unit*)(*it);
			unit->order_list.push_back(new FollowPathOrder());
			unit->order_list.push_back(new UnitAttackOrder(App->ai->targets.front()));
		}

		App->ai->targets.pop_front();
	}

}

void Squad::Explore() {

	if (commander->SetDestination(App->ai->exploration_points.front())) {

		if (units.size() > 1) {

			units.remove(commander);
			App->pathfinding->SharePath(commander, units);
			units.push_front(commander);

		}

		for (list<Entity*>::iterator it = units.begin(); it != units.end(); it++) {
			Unit* unit = (Unit*)(*it);
			unit->order_list.push_back(new FollowPathOrder());
		}

		App->ai->exploration_points.pop_front();
	}
}

