
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
#include "FogOfWar.h"
#include "Scene.h"
#include "Animation.h"
#include "SceneManager.h"
#include "Orders.h"
#include "AI.h"
#include "Squad.h"
#include "Hero.h"


//Move to Order:

void MoveToOrder::Start(Entity* entity) {

	unit = (Unit*)entity;

	state = EXECUTING;
	unit->SetTexture(MOVING);
	unit->destinationTileWorld = destinationMap;

	unit->next_step = unit->entityPosition;
	Execute();
}

void MoveToOrder::Execute() {

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

		// ===============================================================
		// Moving fog of war
		App->fog->Update(unit->prev_pos, unit->next_pos, unit->entityID);			        // This updates the FOW
		unit->prev_pos = unit->next_pos;
		unit->next_pos = App->map->WorldToMap(unit->collider->pos.x, unit->collider->pos.y);
		// ===============================================================

		App->collision->quadTree->UpdateCol(unit->collider);
		if (unit->IsHero) {
			Hero* hero = (Hero*)unit;
			if (hero->aoeTargets != nullptr) {
				App->collision->quadTree->UpdateCol(hero->aoeTargets);
				hero->aoeTargets->pos = hero->next_step;
			}
		}
	}
	else
		state = COMPLETED;

}

bool MoveToOrder::CheckCompletion()
{
	return (unit->entityPosition.DistanceTo(unit->destinationTileWorld) < 10);
}

//Follow path Order:

void FollowPathOrder::Start(Entity* entity)
{
	state = EXECUTING;
	unit = (Unit*)entity;
	unit->state = MOVING;
}


void FollowPathOrder::Execute() {

	if (!CheckCompletion()) {
		iPoint destinationWorld = App->map->MapToWorld(unit->path->front().x, unit->path->front().y);
		unit->order_list.push_front(new MoveToOrder(destinationWorld));
		unit->path->erase(unit->path->begin());

	}
	else {
		App->pathfinding->DeletePath(unit->path);
		state = COMPLETED;
	}

}

bool FollowPathOrder::CheckCompletion() {

	if (unit->path != nullptr) {
		if (unit->path->size() == 0) {
			App->pathfinding->DeletePath(unit->path);
			return true;
		}
	}
	return false;
}

//Reach Order:

void ReachOrder::Start(Entity* argunit) {

	unit = (Unit*)argunit;

	if (unit->SetDestination(entity->collider->pos)) {

		for (list<iPoint>::iterator it = unit->path->end(); it != unit->path->begin(); it--) {

			if (entity->collider->pos.DistanceTo(*it) < entity->collider->r)
				unit->path->pop_back();
		}
	}

	if (!unit->path->empty())
		unit->order_list.push_front(new FollowPathOrder());

	state = EXECUTING;
	unit->next_step = unit->entityPosition;
}

void ReachOrder::Execute() {

	if (!CheckCompletion()) {

		if (unit->destinationTileWorld != entity->entityPosition) {
			unit->destinationTileWorld = entity->collider->pos;
			unit->SetTexture(MOVING);
		}

		unit->entityPosition = unit->next_step;
		unit->collider->pos = unit->next_step;
		unit->range->pos = unit->entityPosition;
		unit->los->pos = unit->entityPosition;

		unit->CalculateVelocity();

		fPoint vel = unit->velocity * unit->unitMovementSpeed * App->entityManager->dt;
		roundf(vel.x);
		roundf(vel.y);

		unit->next_step.x = unit->entityPosition.x + int(vel.x);
		unit->next_step.y = unit->entityPosition.y + int(vel.y);

		// ===============================================================
		// Moving fog of war
		App->fog->Update(unit->prev_pos, unit->next_pos, unit->entityID);			        // This updates the FOW
		unit->prev_pos = unit->next_pos;
		unit->next_pos = App->map->WorldToMap(unit->collider->pos.x, unit->collider->pos.y);
		// ===============================================================

		App->collision->quadTree->UpdateCol(unit->collider);
		if (unit->IsHero) {
			Hero* hero = (Hero*)unit;
			if (hero->aoeTargets != nullptr) {
				App->collision->quadTree->UpdateCol(hero->aoeTargets);
				hero->aoeTargets->pos = hero->next_step;
			}
		}
	}
	else
		state = COMPLETED;

}


bool ReachOrder::CheckCompletion()
{
	if (entity->collider->GetUnit())
		return (entity->collider->pos.DistanceTo(unit->entityPosition) < unit->range->r);
	else
		return (entity->collider->pos.DistanceTo(unit->entityPosition) < (entity->collider->r + unit->collider->r));
}


void UnitAttackOrder::Start(Entity* entity)
{
	
	if (unit = entity->collider->GetUnit()) {
		unit->state = ATTACKING;
		App->entityManager->RallyCall(unit);

		if (unit->entityPosition.DistanceTo(target->entityPosition) > unit->range->r)
			unit->order_list.push_front(new ReachOrder(target));
		else {
			unit->SetTexture(ATTACKING);
			state = EXECUTING;
		}
	}

	Unit* enemy_unit = nullptr;
	if (enemy_unit = target->collider->GetUnit()) {
		if (enemy_unit->state != ATTACKING) {
			enemy_unit->order_list.push_front(new UnitAttackOrder(entity));
			enemy_unit->state = ATTACKING;
		}
	}
}

//Attack order:

void UnitAttackOrder::Execute() {

	if (!CheckCompletion()) {

		if (unit->entityPosition.DistanceTo(target->entityPosition) > unit->range->r) {
			unit->order_list.push_front(new ReachOrder(target));
			state = NEEDS_START;
		}
		else if (unit->currentAnim->Finished())
			target->Life -= unit->Attack - target->Defense;

		if (target->Life <= 0)
			target->Destroy();
	}
	else {
		state = COMPLETED;
		if (target = App->entityManager->FindTarget(unit))
			unit->order_list.push_back(new UnitAttackOrder(target));
	}

}

bool UnitAttackOrder::CheckCompletion() {

	if (target != nullptr) {
		if (target->Life > 0) {
			if (unit->entityPosition.DistanceTo(target->entityPosition) < unit->los->r)
				return false;
		}
	}
	return true;
}


void BuildingAttackOrder::Start(Entity* entity)
{

	if (building = entity->collider->GetBuilding()) {
		App->entityManager->RallyCall(building);
		building->attack_timer.Start();
		building->state = ATTACKING;
	}
	else
		state = COMPLETED;

}

//Attack order:

void BuildingAttackOrder::Execute() {

	if (!CheckCompletion()) {

		if (building->attack_timer.ReadSec() > 0.5) {
			target->Life -= building->Attack - target->Defense;
			building->attack_timer.Start();
		}
		

		if (target->Life <= 0)
			target->Destroy();
	}
	else
		state = COMPLETED;

}

bool BuildingAttackOrder::CheckCompletion() {

	if (target != nullptr) {
		if (target->Life > 0) {
			if (building->entityPosition.DistanceTo(target->entityPosition) < building->range->r)
				return false;
		}
	}
	return true;
}

//Gather order:

void GatherOrder::Start(Entity* entity) {
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

	if (resource != nullptr) {
		villager->resource_carried = resource->type;

		if (!villager->collider->CheckCollision(villager->resourcesWareHouse->collider) && villager->curr_capacity > 0) {

			switch (villager->resource_carried) {
			case WOOD:
				App->entityManager->player->resources.wood += villager->curr_capacity;
				break;
			case GOLD:
				App->entityManager->player->resources.gold += villager->curr_capacity;
				break;
			case FOOD:
				App->entityManager->player->resources.food += villager->curr_capacity;
				break;
			case STONE:
				App->entityManager->player->resources.stone += villager->curr_capacity;
				break;
			}

			App->sceneManager->level1_scene->UpdateResources();
			villager->curr_capacity = 0;
		}

		if (!villager->collider->CheckCollision(resource->collider)) {
			villager->order_list.push_front(new ReachOrder(resource));
			state = NEEDS_START;
		}
		else {
			villager->SetTexture(GATHERING);
			state = EXECUTING;
		}
	}
}

void GatherOrder::Execute() {
	if (resource != nullptr) {
		if (!CheckCompletion()) {
			if (villager->currentAnim->Finished()) {
				villager->curr_capacity += MIN(resource->Life, villager->gathering_speed);
				resource->Life -= MIN(resource->Life, villager->gathering_speed);
			}
		}
		else {
			state = NEEDS_START;
			villager->order_list.push_front(new ReachOrder(villager->resourcesWareHouse));
		}
	}
}

bool GatherOrder::CheckCompletion() {

	if (resource != nullptr) {
		if (villager->curr_capacity >= villager->max_capacity || resource->Life <= 0) {

			if (resource->Life <= 0)
				resource->Destroy();

			return true;
		}
	}
	return false;
}

//Build order:

void BuildOrder::Start(Entity* entity) {
	state = EXECUTING;
	villager = (Villager*)entity;
	villager->state = CONSTRUCTING;

	if (!villager->collider->CheckCollision(building->collider)) {
		villager->order_list.push_front(new ReachOrder(building));
		state = NEEDS_START;
	}
	else
		villager->SetTexture(CONSTRUCTING);
}

void BuildOrder::Execute()
{
	if (!CheckCompletion()) {
		if (villager->currentAnim->Finished())
			//building->Life += MIN(building->MaxLife - building->Life, villager->buildingSpeed);
			building->Life += 200;
	}
	else
		state = COMPLETED;
}

bool BuildOrder::CheckCompletion() {

	if (building != nullptr) {
		if (building->Life >= building->MaxLife) {
			building->Life = building->MaxLife;
			building->entityTexture = building->buildingIdleTexture;
			building->GetBuildingBoundaries();
			building->collider->type = COLLIDER_BUILDING;
			building->state = IDLE;
			if (building->type == FARM) {
				villager->resourcesWareHouse = building;
			}
			return true;
		}
	}
	return false;
}

//Create unit:

void CreateUnitOrder::Start(Entity* entity) {
	state = EXECUTING;
	building = (Building*)entity;
	building->state = CREATING;
	timer.Start();
}

void CreateUnitOrder::Execute()
{
	if (CheckCompletion()) {
		iPoint creation_place = App->map->WorldToMap(building->entityPosition.x, building->entityPosition.y + 150);
		creation_place = App->pathfinding->FindNearestAvailable(creation_place, 10);
		creation_place = App->map->MapToWorld(creation_place.x, creation_place.y);
		Unit* unit = App->entityManager->CreateUnit(creation_place.x, creation_place.y, type);

		if (belongs_to) 
			belongs_to->units.push_back(unit);

		state = COMPLETED;
	}
}

bool CreateUnitOrder::CheckCompletion()
{
	return (timer.ReadSec() > 3); //  3: unit creation time (temporal)
}