#ifndef _Order_
#define _Order_

#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "p2Point.h"
#include "Collision.h"
#include "Application.h"
#include "QuadTree.h"
#include "Pathfinding.h"
#include "Villager.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Animation.h"
#include "SceneManager.h"
#include "Timer.h"

enum Order_state {
	NEEDS_START, EXECUTING, COMPLETED
};

enum OrderType {
	MOVETO, ATTACK, FOLLOWPATH, GATHER, BUILD, CREATE, REACH

};

class Order {
public:

	Order_state state = NEEDS_START;
	OrderType order_type;

public:

	virtual void Start(Entity* entity) {};
	virtual void Execute() {};
	virtual bool CheckCompletion() { return true; };

};


class MoveToOrder : public Order {

public:

	iPoint destinationMap = { -1, -1 };
	Unit* unit = nullptr;

public:

	MoveToOrder(iPoint destination) : destinationMap(destination) { order_type = MOVETO; }

	void Start(Entity* entity) {

		unit = (Unit*)entity;

		state = EXECUTING;
		unit->SetTexture(MOVING);
		unit->destinationTileWorld = destinationMap;

		unit->next_step = unit->entityPosition;
		Execute();
	}

	void Execute() {

		if (!CheckCompletion()) {
			unit->entityPosition = unit->next_step;
			unit->collider->pos = unit->next_step;
			unit->range->pos = unit->entityPosition;

			unit->CalculateVelocity();

			fPoint vel = unit->velocity * unit->unitMovementSpeed * App->entityManager->dt;
			roundf(vel.x);
			roundf(vel.y);

			unit->next_step.x = unit->entityPosition.x + int(vel.x);
			unit->next_step.y = unit->entityPosition.y + int(vel.y);

			App->collision->quadTree->UpdateCol(unit->collider);
		}
		else
			state = COMPLETED;

	}

	bool CheckCompletion() 
	{
		return (unit->entityPosition.DistanceTo(unit->destinationTileWorld) < 10);
	}

};



class FollowPathOrder : public Order {

public:

	Unit* unit = nullptr;

public:

	FollowPathOrder() { order_type = FOLLOWPATH; }

	void Start(Entity* entity)
	{
		state = EXECUTING;
		unit = (Unit*)entity;
		unit->state = MOVING;
	}


	void Execute() {

		if (!CheckCompletion()) {
			iPoint destinationWorld = App->map->MapToWorld(unit->path->front().x, unit->path->front().y);
			Order* new_order = new MoveToOrder(destinationWorld);
			unit->order_list.push_front(new_order);
			unit->path->erase(unit->path->begin());

		}
		else {
			App->pathfinding->DeletePath(unit->path);
			state = COMPLETED;
		}

	}

	bool CheckCompletion() {

		if (unit->path != nullptr) {
			if (unit->path->size() == 0) {
				App->pathfinding->DeletePath(unit->path);
				return true;
			}
		}
		return false;
	}

};


class ReachOrder : public Order {

public:

	Entity* entity = nullptr;
	Unit* unit = nullptr;

public:

	ReachOrder(Entity* argentity) : entity(argentity) { order_type = REACH; }

	void Start(Entity* argunit) {

		unit = (Unit*)argunit;

		if(unit->SetDestination(entity->collider->pos)){

			for (list<iPoint>::iterator it = unit->path->end(); it != unit->path->begin(); it--) {

				if (entity->collider->pos.DistanceTo(*it) < entity->collider->r)
					unit->path->pop_back();
			}
		}

		if (!unit->path->empty()) {
			Order* new_order = new FollowPathOrder();
			unit->order_list.push_front(new_order);
		}
		
		state = EXECUTING;
		unit->next_step = unit->entityPosition;
	}

	void Execute() {

		if (!CheckCompletion()) {

			if (unit->destinationTileWorld != entity->entityPosition) {
				unit->destinationTileWorld = entity->collider->pos;
				unit->SetTexture(MOVING);
			}

			unit->entityPosition = unit->next_step;
			unit->collider->pos = unit->next_step;
			unit->range->pos = unit->entityPosition;

			unit->CalculateVelocity();

			fPoint vel = unit->velocity * unit->unitMovementSpeed * App->entityManager->dt;
			roundf(vel.x);
			roundf(vel.y);

			unit->next_step.x = unit->entityPosition.x + int(vel.x);
			unit->next_step.y = unit->entityPosition.y + int(vel.y);

			App->collision->quadTree->UpdateCol(unit->collider);
		}
		else
			state = COMPLETED;

	}

	bool CheckCompletion() 
	{
		return (entity->collider->pos.DistanceTo(unit->entityPosition) < entity->collider->r);
	}

};

class AttackOrder : public Order {

public:

	Entity* target = nullptr;
	Unit* unit = nullptr;
	Building* building = nullptr;

public:

	AttackOrder(Entity* argtarget) : target(argtarget) { order_type = ATTACK; }

	void Start(Entity* entity)
	{
		state = EXECUTING;

		if (unit = entity->collider->GetUnit()) {
			unit->SetTexture(ATTACKING);
			unit->state = ATTACKING;
		}
		else if (building = entity->collider->GetBuilding()) {
			building->attack_timer.Start();
			building->state = ATTACKING;
		}

		Unit* enemy_unit = nullptr;
		if (enemy_unit = target->collider->GetUnit()) {

			if (enemy_unit->state == ATTACKING) {

				for (list<Order*>::iterator it = enemy_unit->order_list.begin(); it != enemy_unit->order_list.end(); it++) {
					if ((*it)->order_type == ATTACK) {
						AttackOrder* atk_order = (AttackOrder*)(*it);
						if (atk_order->target == entity)
							enemy_unit->order_list.erase(it);
					}
				}
			}

			if (enemy_unit->order_list.front()->order_type == MOVETO)
				enemy_unit->order_list.pop_front();
		}

		Order* new_order = (Order*)new AttackOrder(entity);
		enemy_unit->order_list.push_front(new_order);
	}

	void Execute() {

		if (!CheckCompletion()) {

			if (unit) {
				if (unit->entityPosition.DistanceTo(target->entityPosition) > unit->range->r) {

					Order* new_order = new MoveToOrder(target->entityPosition);  // should pathfind
					unit->order_list.push_front(new_order);
				}
				else if (unit->currentAnim->Finished())
					target->Life -= unit->Attack - target->Defense;
			}
			else if (building) {

				if (building->entityPosition.DistanceTo(target->entityPosition) < building->range->r &&
					building->attack_timer.ReadSec() > 0.5) {

					target->Life -= building->Attack - target->Defense;
					building->attack_timer.Start();
				}
			}

			if (target->Life <= 0) 
				target->Life = -1;
		}
		else
			state = COMPLETED;

	}

	bool CheckCompletion() {

		if (target != nullptr) {
			if (target->Life > 0) {

				if (unit) {
					if (unit->entityPosition.DistanceTo(target->entityPosition) < unit->los->r)
						return false;
				}
				else if (building) {
					if (building->entityPosition.DistanceTo(target->entityPosition) < building->range->r)
						return false;
				}
			}
		}
		return true;
	}
};

class GatherOrder : public Order {

public:

	Villager* villager = nullptr;
	Resource* resource = nullptr;

public:

	GatherOrder(Resource* argresource = nullptr) : resource(argresource) { order_type = GATHER; }

	void Start(Entity* entity) {
		state = EXECUTING;
		villager = (Villager*)entity;
		villager->state = GATHERING;

		if (resource == nullptr) {
			if (villager->resource_carried != NONE)
				resource = App->entityManager->FindNearestResource(villager->resource_carried, villager->entityPosition);
			else {
				state = COMPLETED;
				return;
			}
		}

		villager->resource_carried = resource->type;

		if (!villager->collider->CheckCollision(App->sceneManager->level1_scene->my_townCenter->collider) && villager->curr_capacity > 0) {

			switch (villager->resource_carried) {
			case WOOD:
				App->sceneManager->level1_scene->woodCount += villager->curr_capacity;
				App->sceneManager->level1_scene->wood->SetString(to_string(App->sceneManager->level1_scene->woodCount));
				break;
			case GOLD:
				App->sceneManager->level1_scene->goldCount += villager->curr_capacity;
				App->sceneManager->level1_scene->gold->SetString(to_string(App->sceneManager->level1_scene->woodCount));
				break;
			case FOOD:
				App->sceneManager->level1_scene->foodCount += villager->curr_capacity;
				App->sceneManager->level1_scene->food->SetString(to_string(App->sceneManager->level1_scene->woodCount));
				break;
			case STONE:
				App->sceneManager->level1_scene->woodCount += villager->curr_capacity;
				App->sceneManager->level1_scene->wood->SetString(to_string(App->sceneManager->level1_scene->woodCount));
				break;
			}

			villager->curr_capacity = 0;
		}

		if (!villager->collider->CheckCollision(resource->collider)) {
			Order* new_order = new ReachOrder(resource);  
			villager->order_list.push_front(new_order);
			state = NEEDS_START;
		}
		else {
			villager->SetTexture(GATHERING);
			state = EXECUTING;
		}
	}

	void Execute() {

		if (!CheckCompletion()) {
			if (villager->currentAnim->Finished()) {
				villager->curr_capacity += MIN(resource->Life, villager->gathering_speed);
				resource->Life -= MIN(resource->Life, villager->gathering_speed);
			}
		}
		else {
			state = NEEDS_START;
			iPoint town_hall_pos(TOWN_HALL_POS_X, TOWN_HALL_POS_Y);
			Order* new_order = new ReachOrder(App->sceneManager->level1_scene->my_townCenter); 
			villager->order_list.push_front(new_order);
		}
	}

	bool CheckCompletion() {

		if (resource != nullptr) {
			if (villager->curr_capacity >= villager->max_capacity || resource->Life <= 0) {

				if (resource->Life <= 0) {
					App->collision->DeleteCollider(resource->collider);
					App->entityManager->DeleteResource(resource);
				}

				return true;
			}
		}
		return false;
	}
};


class BuildOrder : public Order {

public:

	Villager* villager = nullptr;
	Building* building = nullptr;

public:

	BuildOrder(Building* argbuilding) : building(argbuilding) { order_type = BUILD; }

	void Start(Entity* entity) {
		state = EXECUTING;
		villager = (Villager*)entity;
		villager->state = CONSTRUCTING;

		if (!villager->collider->CheckCollision(building->collider)) {
			Order* new_order = new ReachOrder(building);  // should pathfind
			villager->order_list.push_front(new_order);
			state = NEEDS_START;
		}
		else
			villager->SetTexture(CONSTRUCTING);
	}

	void Execute()
	{
		if (!CheckCompletion()) {
			if (villager->currentAnim->Finished()) 
				building->Life += MIN(building->MaxLife - building->Life, villager->buildingSpeed);
		}
	}

	bool CheckCompletion() {

		if (building != nullptr) {
			if (building->Life == building->MaxLife)
				return true;
		}
		return false;
	}

};



class CreateUnitOrder : public Order {

public:

	Building* building = nullptr;
	unitType type = VILLAGER;
	Timer timer;

public:

	CreateUnitOrder(unitType argtype) : type(argtype) { order_type = CREATE; }

	void Start(Entity* entity) {
		state = EXECUTING;
		building = (Building*)entity;
		building->state = CREATING;
		timer.Start();
	}

	void Execute()
	{
		if (CheckCompletion()) {
			iPoint creation_place = App->map->WorldToMap(building->entityPosition.x , building->entityPosition.y + 150 );
			creation_place = App->pathfinding->FindNearestAvailable(creation_place, 10);
			creation_place = App->map->MapToWorld(creation_place.x, creation_place.y);
			App->entityManager->CreateUnit(creation_place.x, creation_place.y, type);
			state = COMPLETED;
		}
	}

	bool CheckCompletion() 
	{
		return (timer.ReadSec() > 3); //  3: unit creation time (temporal)
	}

};

#endif