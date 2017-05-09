#include "AI.h"
#include "EntityManager.h"
#include "Application.h"
#include "Orders.h"
#include "SceneManager.h"
#include "Pathfinding.h"

bool AI::Start() {

	enemy_techtree = new TechTree();
	//enemy_techtree->Start();
	return true;
}
bool AI::Update(float dt) {

	return true;

}

void AI::ManageBuildings() {

	list<Building*>* enemy_buildings = &App->entityManager->enemyBuildingList;
	if (!unit_requests.empty()) {
		for (list<unitType>::iterator requested_type = unit_requests.begin(); requested_type != unit_requests.end(); requested_type++) {

			for (list<Building*>::iterator it = enemy_buildings->begin(); it != enemy_buildings->end(); it++) {
				list<unitType>* avl_units = &enemy_techtree->available_units.at((*it)->type);

				for (list<unitType>::iterator avl_type = avl_units->begin(); avl_type != avl_units->end(); avl_type++) {

					if (avl_type == requested_type) {
						(*it)->order_list.push_front(new CreateUnitOrder((*requested_type)));
						unit_requests.erase(requested_type);
					}
				}
			}
		}
	}
	if (!tech_requests.empty()) {
		for (list<int>::iterator requested_type = tech_requests.begin(); requested_type != tech_requests.end(); requested_type++) {

			for (list<Building*>::iterator it = enemy_buildings->begin(); it != enemy_buildings->end(); it++) {
				list<int>* avl_techs = &enemy_techtree->available_techs.at((*it)->type);

				for (list<int>::iterator avl_type = avl_techs->begin(); avl_type != avl_techs->end(); avl_type++) {

					if (avl_type == requested_type) {
						enemy_techtree->StartResearch((*requested_type));
						tech_requests.erase(requested_type);
					}
				}
			}
		}
	}
}
void AI::ManageVillagers() {

	gatherers = idle_villagers.size();
	for (list<Unit*>::iterator it = idle_villagers.begin(); it != idle_villagers.end(); it++) {

		if (!build_requests.empty()) {
			//check resources
			(*it)->order_list.push_back(new BuildOrder(build_requests.front()));
			build_requests.pop_front();
			gatherers--;
			continue;
		}
		else if (resource_requests.empty())
			FillResourceRequests();

		(*it)->order_list.push_back(new GatherOrder(App->entityManager->FindNearestResource(resource_requests.front(), (*it)->entityPosition)));
		resource_requests.pop_front();
	}
}


void AI::FillResourceRequests() {

	// set resource needs proportion
    int total_requests = food_requests + wood_requests + stone_requests + gold_requests;

	int food_proportion = food_requests / (total_requests / 100);
	int wood_proportion = wood_requests / (total_requests / 100);
	int stone_proportion = stone_requests / (total_requests / 100);
	int gold_proportion = gold_requests / (total_requests / 100);

	for (int i = 0; i < MAX(1, (food_proportion * gatherers)); i++)
		resource_requests.push_back(FOOD);
	for (int i = 0; i < MAX(1, (wood_proportion * gatherers)); i++)
		resource_requests.push_back(WOOD);
	for (int i = 0; i < MAX(1, (stone_proportion * gatherers)); i++)
		resource_requests.push_back(STONE);
	for (int i = 0; i < MAX(1, (gold_proportion * gatherers)); i++)
		resource_requests.push_back(GOLD);

}


iPoint AI::PlaceBuilding(buildingType type) {

	int height = App->entityManager->buildingsDB[type]->imageHeight;
	int width = App->entityManager->buildingsDB[type]->imageWidth;
	iPoint start_position = App->sceneManager->level1_scene->enemy_townCenter->entityPosition;

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