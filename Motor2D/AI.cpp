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

	LoadAI_Data(gameData);

	return true;
}

bool AI::Start() {

	LoadExplorationMap();
	IncreaseExpansionLevel();

	return true;
}

void AI::LoadExplorationMap() {

	int width = App->map->data.mapWidth;
	int height = App->map->data.mapHeight;

	for (int i = 0; i < width; i += (width / 10)) {
		for (int j = 0; j < height; j += (height / 10)) {

			iPoint p{ i,j };
			iPoint Map_p = App->map->WorldToMap(p.x, p.y);

			if (!App->pathfinding->IsWalkable(Map_p) || !App->collision->IsOccupied(p)) {

				p = App->pathfinding->FindNearestAvailable(Map_p, 3);
				Map_p = App->map->WorldToMap(p.x, p.y);

				if (App->pathfinding->IsWalkable(Map_p) && App->collision->IsOccupied(p)) 
					exploration_points.push_back(p);
			}
			else
				exploration_points.push_back(p);
		}
	}

	//sort(exploration_points.begin(), exploration_points.end(), [](const iPoint& lhs, const iPoint& rhs, iPoint enemy_town_hall) { return lhs.DistanceTo(enemy_town_hall) < rhs.DistanceTo(enemy_town_hall); });

}



void AI::LoadAI_Data(pugi::xml_node& gameData) {


	pugi::xml_node ExpansionData;
	int expansion_count = 1;

	if (gameData.empty() == false)
	{

		for (ExpansionData = gameData.child("AI").child("Expansion"); ExpansionData; ExpansionData = ExpansionData.next_sibling("Expansion")) {

			pugi::xml_node techs;
			for (techs = ExpansionData.child("Tech"); techs; techs = techs.next_sibling("Tech"))
				expansion_tech_table.at(expansion_count).push_back((TechType)techs.attribute("value").as_int());

			pugi::xml_node buildings;
			for (buildings = ExpansionData.child("Building"); buildings; buildings = buildings.next_sibling("Building"))
				expansion_build_table.at(expansion_count).push_back((buildingType)techs.attribute("value").as_int());

			villager_expansion_table.at(expansion_level) = ExpansionData.child("Villager_num").attribute("value").as_int();

			expansion_count++;
		}
	}
}


bool AI::Update(float dt) {

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

		if (buildings_to_build.empty() && techs_to_research.empty() && unit_requests.empty() && villagers.size() >= villager_expansion_table.at(expansion_level))
			state = OFFENSIVE;

		break;

	case OFFENSIVE:

		ManageAttack();
		ManageUnitRequests();

		if (expansion_level < 10) {
			if (AI_timer.ReadSec() > (60 + (10 * expansion_level)))
				IncreaseExpansionLevel();
		}
		else
			StartAttack();

		break;
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

	if(!targets.empty())
		//sort(targets.begin(), targets.end(), [](const Entity& lhs, const Entity& rhs) { return lhs.MaxLife > rhs.MaxLife; });

	AI_timer.Start();
}

void AI::ManageAttack() {

	for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++) {

		if ((*it)->state == IDLE) {

			if (targets.empty()) {
				for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++)
					(*it)->Explore();
				break;
			}
			else {
				for (list<Squad*>::iterator it = offensive_squads.begin(); it != offensive_squads.end(); it++)
					(*it)->Attack();
				break;
			}
		}
	}
}


void AI::IncreaseExpansionLevel() {

	expansion_level++;

	list<buildingType> to_build = expansion_build_table.at(expansion_level);
	for (list<buildingType>::iterator it = to_build.begin(); it != to_build.end(); it++)
		buildings_to_build.push_front(*it);

	list<TechType> to_research = expansion_tech_table.at(expansion_level);
	for (list<TechType>::iterator it2 = to_research.begin(); it2 != to_research.end(); it2++)
		techs_to_research.push_front(*it2);

	if (expansion_level == 3)
		defensive_squads.push_back(new Squad());
	else if (expansion_level == 6)
		defensive_squads.push_back(new Squad());
	else if (expansion_level == 9)
		defensive_squads.push_back(new Squad());

	squad_size *= 1.2;

	for (list<Squad*>::iterator it3 = defensive_squads.begin(); it3 != defensive_squads.end(); it3++) 
		(*it3)->RestoreUnits();

	state = EXPANDING;
}

void AI::ManageBuildRequests() {

	for (list<buildingType>::iterator it = buildings_to_build.begin(); it != buildings_to_build.end(); it++) {
		if (Enemies->resources.Spend(App->entityManager->unitsDB[(*it)]->cost)) {
			iPoint placing_point = PlaceBuilding(buildings_to_build.front());
			Building* building = App->entityManager->CreateBuilding(placing_point.x, placing_point.y, buildings_to_build.front());

			for (int i = 0; i < building->MaxLife; i += 500)
				villager_requests.push_front(new BuildOrder(building));

			buildings_to_build.erase(it);
		}
	}
}

void AI::ManageVillagerRequests() {

	for (int i = villagers.size(); i < villager_expansion_table.at(expansion_level); i++) {
		if (Enemies->resources.Spend(App->entityManager->unitsDB[VILLAGER]->cost)) 
			Enemies->Town_center->order_list.push_front(new CreateUnitOrder(VILLAGER));
	}
}


void AI::ManageUnitRequests() {

	if (!unit_requests.empty()) {
		for (list<pair<unitType, Squad*>>::iterator requested_type = unit_requests.begin(); requested_type != unit_requests.end(); requested_type++) {

			for (list<pair<unitType, buildingType>>::iterator units = Enemies->tech_tree->available_units.begin(); units != Enemies->tech_tree->available_units.end(); requested_type++) {
				if ((*units).first == (*requested_type).first) {

					list<Building*>* enemy_buildings = &Enemies->buildings;
					for (list<Building*>::iterator building = enemy_buildings->begin(); building != enemy_buildings->end(); building++) {

						if ((*units).first == (*building)->type) {

							if (Enemies->resources.Spend(App->entityManager->unitsDB[(*requested_type).first]->cost)) {
								(*building)->order_list.push_front(new CreateUnitOrder((*requested_type).first, (*requested_type).second));
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
		for (list<pair<int, buildingType>>::iterator requested_type = tech_requests.begin(); requested_type != tech_requests.end(); requested_type++) {

			list<Building*>* enemy_buildings = &Enemies->buildings;
			for (list<Building*>::iterator building = enemy_buildings->begin(); building != enemy_buildings->end(); building++) {

				if ((*requested_type).second == (*building)->type) {

					if (Enemies->resources.Spend(Enemies->tech_tree->all_techs.at((*requested_type).first)->cost)) {
						Enemies->tech_tree->StartResearch((TechType)(*requested_type).first);
						tech_requests.erase(requested_type);
					}
					break;
				}
			}
		}
	}
}


void AI::Fetch_AICommand(Villager* villager) {

	if (villager_requests.empty())
		FillResourceRequests();

	villager->order_list.push_front(villager_requests.front());
	App->ai->villager_requests.pop_front();

}


void AI::FillResourceRequests() {

	for (int i = 0; i < MAX(1, (FOOD_PROPORTION * villagers.size())); i++)
		villager_requests.push_back(new GatherOrder(App->entityManager->FindNearestResource(FOOD, Enemies->Town_center->entityPosition)));
	for (int i = 0; i < MAX(1, (WOOD_PROPORTION * villagers.size())); i++)
		villager_requests.push_back(new GatherOrder(App->entityManager->FindNearestResource(WOOD, Enemies->Town_center->entityPosition)));
	for (int i = 0; i < STONE_PROPORTION * villagers.size(); i++)
		villager_requests.push_back(new GatherOrder(App->entityManager->FindNearestResource(STONE, Enemies->Town_center->entityPosition)));
	for (int i = 0; i < GOLD_PROPORTION * villagers.size(); i++)
		villager_requests.push_back(new GatherOrder(App->entityManager->FindNearestResource(GOLD, Enemies->Town_center->entityPosition)));

}


iPoint AI::PlaceBuilding(buildingType type) {

	int height = App->entityManager->buildingsDB[type]->imageHeight;
	int width = App->entityManager->buildingsDB[type]->imageWidth;
	iPoint start_position = Enemies->Town_center->entityPosition;

	int r = MAX(height, width);

	iPoint ret{ -1,-1 };
	for (int iterations = 1; iterations <= 5; iterations++) {

		for (int i = -iterations * r; i <= iterations * r; i += r) {
			for (int j = -iterations * r; j <= iterations * r; j += r) {

				ret.create(start_position.x + i, start_position.y + j);
				iPoint adjMap = App->map->WorldToMap(ret.x, ret.y);

				if (App->pathfinding->IsWalkable(adjMap) && !App->collision->IsOccupied(ret))
					return ret;
			}
		}
	}

	ret.create(-1, -1);
	return ret;
}