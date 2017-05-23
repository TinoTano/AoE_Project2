#include "AI.h"
#include "EntityManager.h"
#include "Application.h"
#include "Orders.h"
#include "SceneManager.h"
#include "Squad.h"
#include "Pathfinding.h"
#include <time.h>
#include <algorithm>

bool AI::Awake(pugi::xml_node& gameData) {

	Enemies = App->entityManager->AI_faction;
	srand(time(NULL));
	return true;
}

bool AI::Start() {

	pugi::xml_document gameDataFile;
	pugi::xml_node gameData;
	gameData = App->LoadGameDataFile(gameDataFile);

	LoadAI_Data(gameData);

	IncreaseExpansionLevel();

	return true;
}

bool TargetPriority(const pair<Entity*, iPoint> lhs, const  pair<Entity*, iPoint> rhs)
{
	return (lhs.first->MaxLife > rhs.first->MaxLife);
}

bool ThreatPriority(const pair<Entity*, iPoint>* lhs, const  pair<Entity*, iPoint>* rhs)
{
	return (lhs->second.DistanceTo(App->ai->Enemies->Town_center->entityPosition) > rhs->second.DistanceTo(App->ai->Enemies->Town_center->entityPosition));
}

bool ExplorationPriority(const iPoint& lhs, const iPoint& rhs)
{
	return (lhs.DistanceTo(App->ai->Enemies->Town_center->entityPosition) < rhs.DistanceTo(App->ai->Enemies->Town_center->entityPosition));
}


void AI::LoadExplorationMap() {

	int width = App->map->data.height;
	int height = App->map->data.width;

	for (int i = width / 40; i < width - (width / 40); i += (width / 40)) {
		for (int j = height / 40;  j < height - (height / 40); j += (height / 40)) {

			iPoint p{ i,j };
			if (App->pathfinding->IsWalkable(p) && ( i + j > 21 && i + j < 39))   //second condition to fix map shape
				exploration_points.push_back(p);
		}
	}

	exploration_points.sort(ExplorationPriority);

}



void AI::LoadAI_Data(pugi::xml_node& gameData) {


	pugi::xml_node ExpansionData;

	if (gameData.empty() == false)
	{
		for (ExpansionData = gameData.child("AI").child("Expansion"); ExpansionData; ExpansionData = ExpansionData.next_sibling("Expansion")) {

			pugi::xml_node techs;
			list<TechType> expansion_techs;
			for (techs = ExpansionData.child("Tech"); techs; techs = techs.next_sibling("Tech"))
				expansion_techs.push_back((TechType)techs.attribute("value").as_int());

			expansion_tech_table.push_back(expansion_techs);

			pugi::xml_node buildings;
			list<buildingType> expansion_buildings;
			for (buildings = ExpansionData.child("Building"); buildings; buildings = buildings.next_sibling("Building"))
				expansion_buildings.push_back((buildingType)buildings.attribute("value").as_int());

			expansion_build_table.push_back(expansion_buildings);

			villager_expansion_table.push_back(ExpansionData.child("Villager_num").attribute("value").as_int());
		}
	}
}


bool AI::Update(float dt) {

	if (enabled) {
		switch (state) {

		case DEFENSIVE:

			ManageUnitRequests();
			ManageVillagerRequests();
			break;

			if (threats.empty())
				state = EXPANDING;

			break;

		case EXPANDING:

			ManageVillagerRequests();
			ManageBuildRequests();
			ManageTechRequests();
			ManageUnitRequests();

			if (build_requests.empty() && tech_requests.empty() && unit_requests.empty() && Enemies->villagers.size() >= villager_expansion_table.at(expansion_level)) {
				state = OFFENSIVE;
				AI_timer.Start();
				StartAttack();
			}
			break;

		case OFFENSIVE:

			ManageAttack();
			ManageUnitRequests();

			if (AI_timer.ReadSec() > (20 + (20 * expansion_level))) {
				if (expansion_level < 10)
					IncreaseExpansionLevel();
				else
					state = EXPANDING;
			}

			break;
		}

		for (list<Squad*>::iterator it = defensive_squads.begin(); it != defensive_squads.end(); it++)
			(*it)->Update();

		for (list<Squad*>::iterator it2 = offensive_squads.begin(); it2 != offensive_squads.end(); it2++) 
			(*it2)->Update();
	}

	return true;
}

void AI::StartAttack() {

	if(offensive_squads.size() < defensive_squads.size())
		offensive_squads.push_back(new Squad());

	for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++) {
		if ((*it)->units.empty())
			(*it)->Start();
		if ((*it)->units.size() < squad_size)
			(*it)->RestoreUnits();
	}

	if (!targets.empty())
		targets.sort(TargetPriority);

	AI_timer.Start();
}

void AI::ManageAttack() {

	for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++) {

		if ((*it)->squad_orderlist.empty() && (*it)->IsRestored()) {
			if (!targets.empty()) {
				(*it)->squad_orderlist.push_front(new SquadMoveToOrder((*it)->commander, targets.front().second));

				for (list<Unit*>::iterator it2 = (*it)->units.begin(); it2 != (*it)->units.end(); it2++) {
					(*it2)->state = ATTACKING;
					(*it2)->order_list.push_back(new UnitAttackOrder());
				}

				targets.pop_front();
				continue;
			}

			else if (!exploration_points.empty()) {
				(*it)->squad_orderlist.push_front(new SquadMoveToOrder((*it)->commander, exploration_points.front()));
				exploration_points.pop_front();
			}
		}
	}
}


void AI::IncreaseExpansionLevel() {


	list<buildingType> to_build = expansion_build_table.at(expansion_level);
	for (list<buildingType>::iterator it = to_build.begin(); it != to_build.end(); it++)
		build_requests.push_front(*it);

	list<TechType> to_research = expansion_tech_table.at(expansion_level);
	for (list<TechType>::iterator it2 = to_research.begin(); it2 != to_research.end(); it2++)
		tech_requests.push_front(*it2);

	if (expansion_level == 1)
		defensive_squads.push_back(new Squad());
	else if (expansion_level == 4)
		defensive_squads.push_back(new Squad());
	else if (expansion_level == 7)
		defensive_squads.push_back(new Squad());

	squad_size *= 1.2;

	for (list<Squad*>::iterator it3 = defensive_squads.begin(); it3 != defensive_squads.end(); it3++) 
		(*it3)->RestoreUnits();

	expansion_level++;

	state = EXPANDING;
}

void AI::ManageBuildRequests() {

	for (list<buildingType>::iterator it = build_requests.begin(); it != build_requests.end(); it++) {

		if (Enemies->resources.Spend(App->entityManager->buildingsDB[(*it)]->cost)) {

			iPoint placing_point = PlaceBuilding(build_requests.front());
			Building* building = App->entityManager->CreateBuilding(placing_point.x, placing_point.y, (*it));

			int i = 0;
			for (list<Villager*>::iterator it2 = Enemies->villagers.begin(); it2 != Enemies->villagers.end() && i < building->MaxLife; it2++) {
				(*it2)->order_list.push_back(new BuildOrder());
				i += 500;
			}
			build_requests.erase(it);
		}
	}
}

void AI::ManageVillagerRequests() {

	for (int i = Enemies->villagers.size() + requested_villagers; i < villager_expansion_table.at(expansion_level); i++) {
		if (Enemies->resources.Spend(App->entityManager->unitsDB[SLAVE_VILLAGER]->cost)) {
			Enemies->Town_center->units_in_queue.push_back(SLAVE_VILLAGER);
			requested_villagers++;
		}
	}
}


void AI::ManageUnitRequests() {

	if (!unit_requests.empty()) {
		for (list<unitType>::iterator requested_type = unit_requests.begin(); requested_type != unit_requests.end(); requested_type++) {

			for (list<pair<unitType, buildingType>>::iterator units = Enemies->tech_tree->available_units.begin(); units != Enemies->tech_tree->available_units.end(); requested_type++) {
				if ((*units).first == (*requested_type)) {

					list<Building*>* enemy_buildings = &Enemies->buildings;
					for (list<Building*>::iterator building = enemy_buildings->begin(); building != enemy_buildings->end(); building++) {

						if ((*units).second == (*building)->type) {

							if (Enemies->resources.Spend(App->entityManager->unitsDB[(*requested_type)]->cost)) {
								Enemies->Town_center->units_in_queue.push_back((*requested_type));
								unit_requests.erase(requested_type);
							}
							break;
						}
					}
				}
				break;
			}
		}
	}
}

void AI::ManageTechRequests() {

	if (!tech_requests.empty()) {
		for (list<TechType>::iterator requested_type = tech_requests.begin(); requested_type != tech_requests.end(); requested_type++) {

			list<Building*>* enemy_buildings = &Enemies->buildings;
			for (list<Building*>::iterator building = enemy_buildings->begin(); building != enemy_buildings->end(); building++) {

				if (Enemies->tech_tree->all_techs.at(*requested_type)->researched_in == (*building)->type) {

					if (Enemies->resources.Spend(Enemies->tech_tree->all_techs.at(*requested_type)->cost)) {
						Enemies->tech_tree->StartResearch(*requested_type);
						tech_requests.erase(requested_type);
					}
					break;
				}
			}
		}
	}
}


void AI::Fetch_AICommand(Villager* villager) {

	if (resource_requests.empty())
		FillResourceRequests();

	villager->resource_carried = resource_requests.front();
	villager->order_list.push_front(new GatherOrder());
	resource_requests.pop_front();

}


void AI::FillResourceRequests() {

	for (int i = 0; i < MAX(1, (FOOD_PROPORTION * Enemies->villagers.size())); i++)
		resource_requests.push_back(FOOD);
	for (int i = 0; i < MAX(1, (WOOD_PROPORTION * Enemies->villagers.size())); i++)
		resource_requests.push_back(WOOD);
	for (int i = 0; i < STONE_PROPORTION * Enemies->villagers.size(); i++)
		resource_requests.push_back(STONE);
	for (int i = 0; i < GOLD_PROPORTION * Enemies->villagers.size(); i++)
		resource_requests.push_back(GOLD);

}


iPoint AI::PlaceBuilding(buildingType type) {

	int height = App->entityManager->buildingsDB[type]->imageHeight;
	int width = App->entityManager->buildingsDB[type]->imageWidth;
	iPoint start_position = Enemies->Town_center->entityPosition;

	int r = MAX(height, width);

	iPoint ret{ -1,-1 };
	for (int iterations = 1; iterations <= 3; iterations++) {

		for (int i = -iterations * r; i <= iterations * r; i += r) {
			for (int j = -iterations * r; j <= iterations * r; j += r) {

				ret.create(start_position.x + i, start_position.y + j);
				iPoint adjMap = App->map->WorldToMap(ret.x, ret.y);

				if (App->pathfinding->IsWalkable(adjMap))
					return ret;
			}
		}
	}

	ret.create(-1, -1);
	return ret;
}


void AI::UpdateThreats(Entity* entity) {

	for (list<pair<Entity*, iPoint>>::iterator threat = threats.begin(); threat != threats.end(); threat++) {
		if ((*threat).first == entity) {
			(*threat).second = entity->entityPosition;
			return;
		}
	}

	pair<Entity*, iPoint> new_threat = { entity, entity->entityPosition };
	threats.push_back(new_threat);

}

void AI::UpdateTargets(Entity* entity) {

	for (list<pair<Entity*, iPoint>>::iterator target = targets.begin(); target != targets.end(); target++) {
		if ((*target).first == entity) {
			(*target).second = entity->entityPosition;
			return;
		}
	}

	pair<Entity*, iPoint> new_target = { entity, entity->entityPosition };
	targets.push_back(new_target);

}

void AI::RemoveThreats(Entity* entity) {

	for (list<pair<Entity*, iPoint>>::iterator target = targets.begin(); target != targets.end(); target++) {
		if ((*target).first == entity) {
			targets.erase(target);
			break;
		}
	}

	for (list<pair<Entity*, iPoint>>::iterator threat = threats.begin(); threat != threats.end(); threat++) {
		if ((*threat).first == entity) {
			threats.erase(threat);
			break;
		}
	}

}


Squad* AI::AssignUnit(Unit* unit) {

	for (list<Squad*>::iterator it = defensive_squads.begin(); it != defensive_squads.end(); it++) {
		if (unit->type == (*it)->type && (*it)->units.size() < squad_size) {
			(*it)->Assign(unit);
			return (*it);
		}

	}

	for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++) {
		if (unit->type == (*it)->type && (*it)->units.size() < squad_size) {
			(*it)->Assign(unit);
			return (*it);
		}
	}
}