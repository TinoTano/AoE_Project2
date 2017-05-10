#include "Squad.h"
#include "Building.h"
#include "Unit.h"
#include "AI.h"


void Squad::Start() {

	list<pair< buildingType, unitType>>* available_types = &App->ai->enemy_techtree->all_available_units;
	int random = rand() / available_types->size();

	list<pair< buildingType, unitType>>::iterator it = available_types->begin();

	for (int i = 0; i < random; i++)
		it++;

	type = (*it).second;
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

