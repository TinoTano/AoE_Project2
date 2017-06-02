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

bool AI::Update(float dt) {

	if (enabled) {

		CheckCollisions();
		bool completed = false;

		if (state == EXPANDING) {
			if (AI_timer.ReadSec() >= 2) {
				selected_building->Life += 100;
				AI_timer.Start();
			}

			if (selected_building->Life >= selected_building->MaxLife) {
				selected_building->Life = selected_building->MaxLife;
				selected_building->entityTexture = App->entityManager->buildingsDB[selected_building->type]->entityTexture;
				selected_building->GetBuildingBoundaries();
				selected_building->state = IDLE;
				completed = true;
			}
		}
		else                                                                           //state OFFENSIVE or DEFENSIVE
			completed = (selected_building->units_in_queue.empty());

		if (completed) {

			ChangeState();

			if (state == OFFENSIVE && !last_attack_squad.empty())
				LaunchAttack();
		}
	}
	return true;
}

void AI::QueueUnits() {

	//unitType type = rand() % selected_building->available_units.size();
	for (int i = 0; i < 1; i++)
		selected_building->units_in_queue.push_back(GOBLIN_SOLDIER);
	
}

void AI::SelectBuilding(AI_state ai_state) {

	vector<Building*> buildings;

	for (list<Building*>::iterator it = Enemies->buildings.begin(); it != Enemies->buildings.end(); it++) {
		if (((*it)->state == BEING_BUILT && ai_state == EXPANDING) || ((*it)->state != BEING_BUILT && ai_state != EXPANDING))
			buildings.push_back(*it);
	}

	if (!buildings.empty()) {
		int random = rand() % buildings.size();
		selected_building = buildings.at(random);
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
}

void AI::LaunchAttack() {

	for (list<Unit*>::iterator it = last_attack_squad.begin(); it != last_attack_squad.end(); it++) {
		(*it)->order_list.push_front(new MoveToOrder((*it), { TOWN_HALL_POS_X, TOWN_HALL_POS_Y }));
		(*it)->order_list.push_back(new UnitAttackOrder());
	}

	last_attack_squad.clear();
}


bool AI::Load(pugi::xml_node &)
{
	return true;
}

bool AI::Save(pugi::xml_node & data) const
{
	return true;
}

void AI::CheckCollisions() {

	for (list<Unit*>::iterator enemy = Enemies->units.begin(); enemy != Enemies->units.end(); enemy++) {

		potential_collisions.clear();
		App->collision->quadTree->Retrieve(potential_collisions, (*enemy)->los);

		for (list<Unit*>::iterator ally = App->entityManager->player->units.begin(); ally != App->entityManager->player->units.end(); ally++) {

			if ((*enemy)->los->CheckCollision((*ally)->collider)) {
				if ((*enemy)->order_list.empty() || (*enemy)->state != ATTACKING)
					(*enemy)->order_list.push_front(new UnitAttackOrder());
			}
		}

		for (list<Building*>::iterator ally_building = App->entityManager->player->buildings.begin(); ally_building != App->entityManager->player->buildings.end(); ally_building++) {

			if ((*enemy)->los->CheckCollision((*ally_building)->collider)) {
				if ((*enemy)->order_list.empty() || (*enemy)->state != ATTACKING)
					(*enemy)->order_list.push_front(new UnitAttackOrder());
			}
		}
	}
}