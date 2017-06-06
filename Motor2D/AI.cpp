#include "AI.h"
#include "EntityManager.h"
#include "Application.h"
#include "Orders.h"
#include "SceneManager.h"
#include "Collision.h"
#include "StaticQuadtree.h"
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
	AI_timer.Start();

	return true;
}

void AI::LoadAI_Data(pugi::xml_node& gameData) {

	pugi::xml_node AvailableUnit;

	if (gameData.empty() == false)
	{
		for (AvailableUnit = gameData.child("AI").child("AvailableUnit"); AvailableUnit; AvailableUnit = AvailableUnit.next_sibling("AvailableUnit")) {

			buildingType building = (buildingType)AvailableUnit.attribute("building").as_int();
			unitType unit = (unitType)AvailableUnit.attribute("unit").as_int();
			available_unit.insert(pair<buildingType, unitType>(building, unit));
		}
	}
}



bool AI::Update(float dt) {

	if (enabled) {

		CheckCollisions();

		if (!completed) {

			if (state == EXPANDING) {

				if (selected_building->type == SAURON_TOWER)
					return (completed = true);

				if ((selected_building->Life >= selected_building->MaxLife)) {
					selected_building->Life = selected_building->MaxLife;
					selected_building->entityTexture = App->entityManager->buildingsDB[selected_building->type]->entityTexture;
					selected_building->GetBuildingBoundaries();
					selected_building->state = IDLE;
					completed = true;
				}
				else if (build_timer.ReadSec() >= 3) {
					selected_building->Life += 100;
					build_timer.Start();
				}
			}
			else                                                                           //state OFFENSIVE or DEFENSIVE
				completed = (selected_building->units_in_queue.empty());
		}

		if (completed && AI_timer.ReadSec() > 60) {

			ChangeState();

			if (state == OFFENSIVE && !last_attack_squad.empty())
				LaunchAttack();

			completed = false;
		}
	}
	return true;
}

void AI::QueueUnits() {

	int num = 0, life = App->entityManager->unitsDB[available_unit[selected_building->type]]->MaxLife;

	if (life < 50)
		num = 5;
	else if (life < 200)
		num = 3;
	else
		num = 1;

	for (int i = 0; i < num; i++)
		selected_building->units_in_queue.push_back(available_unit[selected_building->type]);

}

void AI::SelectBuilding(AI_state ai_state) {

	vector<Building*> buildings;

	for (list<Building*>::iterator it = Enemies->buildings.begin(); it != Enemies->buildings.end(); it++) {
		if (((*it)->state == DESTROYED && ai_state == EXPANDING && (*it)->creation_timer.ReadSec() > 60) || ((*it)->state != DESTROYED && ai_state != EXPANDING))
			buildings.push_back(*it);
	}

	if (!buildings.empty()) {

		selected_building = buildings.front();

		if (ai_state == EXPANDING) {
			for (vector<Building*>::iterator it2 = buildings.begin(); it2 != buildings.end(); it2++) {
				if ((*it2)->MaxLife < selected_building->MaxLife)
					selected_building = (*it2);
			}
		}
		else {
			int random = rand() % buildings.size();
			selected_building = buildings.at(random);
		}
	}
	else
		selected_building = App->entityManager->AI_faction->Town_center;
}

void AI::ChangeState() {

	if (forced_state) {
		state = forced_state;
		selected_building = forced_building;
		forced_state = NULL_STATE;
		forced_building = nullptr;
	}
	else {
		state = (AI_state)((rand() % (MAX_STATES - 1)) + 1);
		SelectBuilding(state);
	}

	if (state == OFFENSIVE || state == DEFENSIVE)
		QueueUnits();	

	AI_timer.Start();
}

void AI::LaunchAttack() {

	
	for (list<Unit*>::iterator it = last_attack_squad.begin(); it != last_attack_squad.end(); it++) 
		(*it)->order_list.push_front(new MoveToOrder((*it), { TOWN_HALL_POS_X, TOWN_HALL_POS_Y + 200}));

	last_attack_squad.clear();
}


bool AI::Load(pugi::xml_node & data)
{

	Enemies = App->entityManager->AI_faction;
	state = (AI_state)data.child("State").attribute("value").as_int();
	forced_state = (AI_state)data.child("ForcedState").attribute("value").as_int();
	enabled = data.child("Enabled").attribute("value").as_bool();

	for (list<Entity*>::iterator it = App->entityManager->WorldEntityList.begin(); it != App->entityManager->WorldEntityList.end(); ++it)
	{
		if (data.child("SelectedBuilding").attribute("value").as_int() == (*it)->entityID)
		{
			selected_building = (Building*)(*it);
		}
		if (data.child("ForcedBuilding").attribute("value").as_int() == (*it)->entityID)
		{
			forced_building = (Building*)(*it);
		}
	}

	last_attack_squad.clear();
	for (pugi::xml_attribute lastsquad = data.child("LastAttacks").attribute("Squad"); lastsquad; lastsquad = lastsquad.next_attribute())
	 {
		 for (list<Entity*>::iterator it = App->entityManager->WorldEntityList.begin(); it != App->entityManager->WorldEntityList.end(); ++it)
		 {
			 if (lastsquad.as_int() == (*it)->entityID)
			 {
				 Unit* unit = (Unit*)(*it);
				 last_attack_squad.push_back(unit);
			 }
		 }
	}

	potential_collisions.clear();

	available_unit.clear();

	for (pugi::xml_node it = data.child("AvailableUnits").child("AvailableUnit"); it; it = it.next_sibling("AvailableUnit"))
	{
		map<int, unitType> load;
		int number = it.attribute("Num").as_int();
		unitType type = (unitType)it.attribute("Unit").as_int();
		load.emplace(number, type); 
	}

	return true;
}

bool AI::Save(pugi::xml_node & data) const
{
	data.append_child("State").append_attribute("value") = (int)state;
	data.append_child("ForcedState").append_attribute("value") = (int)forced_state;
	data.append_child("Enabled").append_attribute("value") = enabled;
	if (selected_building != nullptr)
	data.append_child("SelectedBuilding").append_attribute("value") = selected_building->entityID;
	if (forced_building != nullptr)
	data.append_child("ForcedBuilding").append_attribute("value") = forced_building->entityID;

	pugi::xml_node lastsquad = data.append_child("LastAttacks");

		for (list<Unit*>::const_iterator it = last_attack_squad.begin(); it != last_attack_squad.end(); ++it )
	{
		lastsquad.append_attribute("Squad") = (*it)->entityID;
	}

		pugi::xml_node availableunits = data.append_child("AvailableUnits");

		for (map<int, unitType>::const_iterator it = available_unit.begin(); it != available_unit.end(); ++it)
		{
			pugi::xml_node unit = availableunits.append_child("AvailableUnit");
			unit.append_attribute("Num") = (*it).first;
			unit.append_attribute("Unit") = (*it).second;
		}

	return true;
}

void AI::CheckCollisions() {

	for (list<Unit*>::iterator enemy = Enemies->units.begin(); enemy != Enemies->units.end(); enemy++) {

		potential_collisions.clear();
		App->collision->quadTree->Retrieve(potential_collisions, (*enemy)->los);


		for (list<Unit*>::iterator ally = App->entityManager->player->units.begin(); ally != App->entityManager->player->units.end(); ally++) {

			if ((*enemy)->los->CheckCollision((*ally)->collider)) {
				if ((*enemy)->order_list.empty() || (*enemy)->state != ATTACKING) {
					(*enemy)->order_list.clear();
					(*enemy)->order_list.push_front(new UnitAttackOrder());
					(*enemy)->order_list.push_back(new MoveToOrder((*enemy), { TOWN_HALL_POS_X, TOWN_HALL_POS_Y + 200 }));
				}
			}
		}

		for (list<Building*>::iterator ally_building = App->entityManager->player->buildings.begin(); ally_building != App->entityManager->player->buildings.end(); ally_building++) {

			if ((*enemy)->los->CheckCollision((*ally_building)->collider)) {
				if ((*enemy)->order_list.empty() || (*enemy)->state != ATTACKING) {
					(*enemy)->order_list.clear();
					(*enemy)->order_list.push_front(new UnitAttackOrder());
					(*enemy)->order_list.push_back(new MoveToOrder((*enemy), { TOWN_HALL_POS_X, TOWN_HALL_POS_Y + 200 }));
				}
			}
		}

		for (list<Unit*>::iterator enemy2 = App->entityManager->AI_faction->units.begin(); enemy2 != App->entityManager->AI_faction->units.end(); enemy2++) {

			if ((*enemy)->collider->CheckCollision((*enemy2)->collider) && (*enemy) != (*enemy2)) {
				bool needs_to_move = false;

				if ((*enemy)->order_list.empty() && (*enemy2)->order_list.empty())
					needs_to_move = true;
				else if (!(*enemy)->order_list.empty() && !(*enemy2)->order_list.empty()) {
					if ((*enemy)->order_list.front()->order_type != MOVETO && ((*enemy2)->order_list.front()->order_type != MOVETO))
						needs_to_move = true;
				}

				if(needs_to_move){
					iPoint destMap = App->map->WorldToMap((*enemy)->entityPosition.x, (*enemy)->entityPosition.y);
					destMap = App->pathfinding->FindNearestWalkable(destMap);
					iPoint destWorld = App->map->MapToWorld(destMap.x, destMap.y);
					(*enemy)->order_list.push_front(new MoveToOrder((*enemy), destWorld));
				}
			}
		}
	}
}