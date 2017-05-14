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
	unit->squad = this;

	if (commander == nullptr)
		commander = unit;

}

void Squad::RestoreUnits() {

	for (int i = units.size(); i <= App->ai->squad_size; i++) {
		pair<unitType, Squad*> unit_data{ type, this };
		App->ai->unit_requests.push_front(unit_data);
	}
}

void Squad::ClearOrders() {

	if (!units.empty()) {
		for (list<Unit*>::iterator it = units.begin(); it != units.end(); it++)
			(*it)->order_list.clear();

	}
}

bool Squad::IsRestored()
{
	return (units.size() >= App->ai->squad_size);
}


