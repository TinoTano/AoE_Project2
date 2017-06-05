
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "p2Point.h"
#include "Collision.h"
#include "Application.h"
#include "StaticQuadtree.h"
#include "Pathfinding.h"
#include "Villager.h"
#include "EntityManager.h"
#include "FogOfWar.h"
#include "Scene.h"
#include "Animation.h"
#include "SceneManager.h"
#include "Orders.h"
#include "AI.h"
#include "Hero.h"
#include "Audio.h"
#include "ParticleManager.h"

//Move to Order:

//MoveToOrder::MoveToOrder(Unit* unit, iPoint destWorld) {
//
//	order_type = MOVETO; 
//	unit->path.clear();
//	iPoint origin = App->map->WorldToMap(unit->collider->pos.x, unit->collider->pos.y);
//	iPoint destMap = App->map->WorldToMap(destWorld.x, destWorld.y);
//	App->collision->relevant_unit = unit;
//
//	if (!App->pathfinding->IsWalkable(destMap))
//		destMap = App->pathfinding->FindNearestAvailable(destMap, 5);
//
//	if (!App->pathfinding->IsWalkable(origin))
//		origin = App->pathfinding->FindNearestAvailable(origin, 5);
//
//	if (origin.x == -1 || destMap.x == -1) {
//		state == COMPLETED;
//		return;
//	}
//
//	Path path;
//	path.open.pathNodeList.push_back(PathNode(0, 0, origin, NULL));
//	path.origin = origin;
//	path.destination = destMap;
//
//	App->pathfinding->CalculatePath(&path);
//
//	for (list<iPoint>::iterator it = path.finished_path.begin(); it != path.finished_path.end(); it++)
//		unit->path.push_back((*it));
//
//	destMap = App->map->WorldToMap(destWorld.x, destWorld.y);
//	unit->path.push_back(destMap);
//
//	App->collision->relevant_unit = nullptr;
//}
//
//void MoveToOrder::Start(Unit* unit) {
//
//	state = COMPLETED;
//
//	if (!unit->path.empty()) {
//
//		unit->destinationTileWorld = App->map->MapToWorld(unit->path.front().x, unit->path.front().y);
//		unit->path.pop_front();
//
//		unit->next_pos = unit->entityPosition;
//		unit->state = MOVING;
//
//		unit->SetTexture(MOVING);
//		state = EXECUTING;
//	}
//}
//
//void MoveToOrder::Execute(Unit* unit) {
//
//	if (!CheckCompletion(unit)) {
//
//		iPoint prev_posMap = App->map->WorldToMap(unit->entityPosition.x, unit->entityPosition.y);
//		unit->entityPosition = unit->next_pos;
//		iPoint posMap = App->map->WorldToMap(unit->entityPosition.x, unit->entityPosition.y);
//
//		fPoint velocity;
//		velocity.x = unit->destinationTileWorld.x - unit->collider->pos.x;
//		velocity.y = unit->destinationTileWorld.y - unit->collider->pos.y;
//
//		if (velocity.x != 0 || velocity.y != 0)
//			velocity.Normalize();
//
//		velocity = velocity * unit->unitMovementSpeed * App->entityManager->dt;
//		roundf(velocity.x);
//		roundf(velocity.y);
//
//		unit->next_pos.x = unit->entityPosition.x + int(velocity.x);
//		unit->next_pos.y = unit->entityPosition.y + int(velocity.y);
//
//		unit->LookAt(velocity);
//
//		unit->collider->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };
//		unit->range->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };
//		unit->los->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };
//
//		App->fog->Update(prev_posMap, posMap, unit->entityID);
//		App->collision->quadTree->UpdateCol(unit->collider);
//
//	}
//	else
//		state = COMPLETED;
//
//}
//
//bool MoveToOrder::CheckCompletion(Unit* unit)
//{
//	if (unit->collider->pos.DistanceTo(unit->destinationTileWorld) < 10) {
//		if (unit->path.empty())
//			return true;
//		else {
//			unit->destinationTileWorld = App->map->MapToWorld(unit->path.front().x, unit->path.front().y);
//			unit->path.pop_front();
//		}
//	}
//	return false;
//}

MoveToOrder::MoveToOrder(Unit* unit, iPoint destWorld) {

	order_type = MOVETO;
	//App->pathfinding->DeletePath(unit->path_id);
	unit->path.clear();

	if (unit->collider != nullptr) {
		iPoint origin = App->map->WorldToMap(unit->collider->pos.x, unit->collider->pos.y);
		iPoint destMap = App->map->WorldToMap(destWorld.x, destWorld.y);

		if (!App->pathfinding->IsWalkable(destMap, unit->collider) /*&& !App->collision->FindCollider(destMap, unit->collider->r, unit->collider)*/)
			destMap = App->pathfinding->FindNearestAvailable(destMap, 5);

		if (!App->pathfinding->IsWalkable(origin, unit->collider))
			origin = App->pathfinding->FindNearestAvailable(origin, 5);

		if (origin.x == -1 || destMap.x == -1) {
			state == COMPLETED;
			return;
		}

		iPoint worldOrigin = App->map->MapToWorld(origin.x, origin.y);
		App->pathfinding->current_unit = unit;
		unit->path = App->pathfinding->CreatePath(worldOrigin, destMap);
		App->pathfinding->current_unit = nullptr;
		if (unit->path.empty()) {
			state = COMPLETED;
			return;
		}
		/*else {
		unit->pathReady = false;
		unit->pathIsCalculating = true;
		}*/
	}

	lastPathPos = destWorld;

}

void MoveToOrder::Start(Unit* unit) {

	//state = COMPLETED;

	if (!unit->path.empty()) {
		if (unit->path.size() == 1) {
			unit->destinationTileWorld = lastPathPos;
		}
		else {
			if (unit->path.front().DistanceTo(unit->entityPosition) < 30) {
				unit->path.erase(unit->path.begin());
			}
			unit->destinationTileWorld = unit->path.front();
		}
		unit->state = MOVING;
		unit->SetTexture(MOVING);
		state = EXECUTING;
	}
}

void MoveToOrder::Execute(Unit* unit) {

	if (!CheckCompletion(unit)) {

		iPoint prev_posMap = App->map->WorldToMap(unit->entityPosition.x, unit->entityPosition.y);

		if (!unit->path.empty()) {
			if (prev_posMap == App->map->WorldToMap(unit->path.front().x, unit->path.front().y)) {
				unit->path.erase(unit->path.begin());
			}
		}
		else {
			unit->destinationTileWorld = lastPathPos;
		}

		fPoint velocity;
		velocity.x = unit->destinationTileWorld.x - unit->collider->pos.x;
		velocity.y = unit->destinationTileWorld.y - unit->collider->pos.y;

		if (velocity.x != 0 || velocity.y != 0)
			velocity.Normalize();

		velocity = velocity * unit->unitMovementSpeed * App->entityManager->dt;
		roundf(velocity.x);
		roundf(velocity.y);

		if (unit->unitMovementSpeed >= 130) {
			if (int(velocity.x) == 1) {
				velocity.x = 2;
			}
			else if (int(velocity.x) == -1) {
				velocity.x = -2;
			}
			if (int(velocity.y) == 1) {
				velocity.y = 2;
			}
			else if (int(velocity.y) == -1) {
				velocity.y = -2;
			}
		}

		unit->entityPosition.x += int(velocity.x);
		unit->entityPosition.y += int(velocity.y);

		unit->LookAt(velocity);

		unit->collider->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };
		unit->range->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };
		unit->los->pos = { unit->entityPosition.x, unit->entityPosition.y + unit->selectionAreaCenterPoint.y };

		iPoint posMap = App->map->WorldToMap(unit->entityPosition.x, unit->entityPosition.y);

		App->fog->Update(prev_posMap, posMap, unit->entityID);
		App->collision->quadTree->UpdateCol(unit->collider);

	}
	else {
		state = COMPLETED;
		//unit->pathReady = false;
		//App->pathfinding->DeletePath(unit->path_id);
		unit->path.clear();
	}
}

bool MoveToOrder::CheckCompletion(Unit* unit)
{
	if (unit->collider->pos.DistanceTo(unit->destinationTileWorld) < 10) {
		if (unit->path.empty()) {
			//unit->pathIsCalculating = false;
			return true;
		}
		else {
			unit->destinationTileWorld = unit->path.front();
			unit->path.erase(unit->path.begin());
		}
	}

	return false;
}

void UnitAttackOrder::Start(Unit* unit)
{
	if (Entity* nearest_enemy = App->entityManager->FindTarget(unit)) {   

		if (unit->range->CheckCollision(nearest_enemy->collider)) {
			state = EXECUTING;
			unit->SetTexture(ATTACKING);
			if (unit->sub_movement) {
				RELEASE(unit->sub_movement);
				unit->sub_movement = nullptr;
			}
		}
		else if ((unit->los->CheckCollision(nearest_enemy->collider) || (nearest_enemy->isActive && unit->faction == FREE_MEN))) 
			unit->SubordinatedMovement(nearest_enemy->collider->pos);
		else
			state = COMPLETED;

		unit->state = ATTACKING;
	}
	else
		state = COMPLETED;
}

//Attack order:

void UnitAttackOrder::Execute(Unit* unit) {

	if (CheckCompletion(unit)) {
		if (Entity* nearest_enemy = App->entityManager->FindTarget(unit)) {  // not DESTROYED

			if (unit->range->CheckCollision(nearest_enemy->collider)) {

				App->audio->PlayFightSound(unit);
				//JOSEP HERE
				if (unit->range_value > 0 ) 
				{
					pair<int, int> unitpos = {unit->GetPosition().x, unit->GetPosition().y };
					pair<int, int> enemypos = { nearest_enemy->GetPosition().x,nearest_enemy->GetPosition().y };
					App->particlemanager->CreateArrow(400, unitpos, enemypos);
				}
				nearest_enemy->Life -= MAX(unit->Attack - nearest_enemy->Defense, unit->unitPiercingDamage);
				if (nearest_enemy->state != ATTACKING)
					App->entityManager->RallyCall(nearest_enemy);

				if (nearest_enemy->Life <= 0) {
					nearest_enemy->state = DESTROYED;
					state = NEEDS_START;
				}
				else if (nearest_enemy->collider->type == COLLIDER_UNIT && nearest_enemy->state != ATTACKING) {
					Unit* enemy = (Unit*)nearest_enemy;
					enemy->order_list.push_front(new UnitAttackOrder());
					enemy->state = ATTACKING;
				}
			}
			else
				state = NEEDS_START;
		}
		else
			state = COMPLETED;
	}
}

bool UnitAttackOrder::CheckCompletion(Unit* unit) 
{
	return (unit->currentAnim->at(unit->currentDirection).Finished());
}

void GatherOrder::Start(Unit* unit) {

	Villager* villager = (Villager*)unit;
	villager->state = GATHERING;

	if (villager->curr_capacity > 0) {

		if (unit->faction == App->entityManager->player->faction) {
			if (villager->collider->CheckCollision(App->entityManager->player->Town_center->collider)) {
				App->entityManager->AddResources(villager);
				if (unit->sub_movement) {
					RELEASE(unit->sub_movement);
					unit->sub_movement = nullptr;
				}
			}
			else
				unit->SubordinatedMovement(App->entityManager->player->Town_center->collider->pos);
		}
		else{
			if (villager->collider->CheckCollision(App->entityManager->AI_faction->Town_center->collider)) {

				if (unit->sub_movement) {
					RELEASE(unit->sub_movement);
					unit->sub_movement = nullptr;
				}

				App->entityManager->AddResources(villager);
				state = COMPLETED;
				return;
			}
			else
				unit->SubordinatedMovement(App->entityManager->AI_faction->Town_center->collider->pos);
		}
	}


	if (villager->resource_carried != NONE) {
		if (Resource* resource = App->entityManager->FindNearestResource(villager->resource_carried, villager->entityPosition)) {

			if (resource->collider !=nullptr && !villager->collider->CheckCollision(resource->collider))
				unit->SubordinatedMovement(resource->collider->pos);
			else {

				if (unit->sub_movement) {
					RELEASE(unit->sub_movement);
					unit->sub_movement = nullptr;
				}
				state = EXECUTING;
				unit->SetTexture(GATHERING);
				return;
			}
		}
		else
			state = COMPLETED;
	}

}

void GatherOrder::Execute(Unit* unit) {

	if (CheckCompletion(unit)) {

		Villager* villager = (Villager*)unit;

		if (Resource* resource = App->entityManager->FindNearestResource(villager->resource_carried, villager->entityPosition)) {   // not DESTROYED

			if (villager->collider->CheckCollision(resource->collider)) {
				App->audio->PlayGatherSound(resource);
				villager->curr_capacity += MIN(resource->Life, villager->gathering_speed);
				resource->Life -= MIN(resource->Life, villager->gathering_speed);

				if (resource->Life <= 0 || villager->curr_capacity >= villager->max_capacity) {
					if (resource->Life <= 0)
						resource->Destroy();

					state = NEEDS_START;
				}
			}
			else
				state = NEEDS_START;
		}
		else
			state = COMPLETED;
	}

}

bool GatherOrder::CheckCompletion(Unit* unit) 
{
	return (unit->currentAnim->at(unit->currentDirection).Finished());
}

//Build order:

void BuildOrder::Start(Unit* unit) {
	

	if (Building* to_build = App->entityManager->FindNearestBuilding(unit)) {
		unit->state = CONSTRUCTING;

		if (!unit->collider->CheckCollision(to_build->collider)) {
			unit->SubordinatedMovement(to_build->collider->pos);
		}
		else {
			if (unit->sub_movement) {
				RELEASE(unit->sub_movement);
				unit->sub_movement = nullptr;
			}
			unit->SetTexture(CONSTRUCTING);
			state = EXECUTING;
		}
	}
	else
		state = COMPLETED;
}

void BuildOrder::Execute(Unit* unit)
{
	if (CheckCompletion(unit)) {

		if (Building* to_build = App->entityManager->FindNearestBuilding(unit)) {

			if (to_build->collider->CheckCollision(unit->collider)) {

				Villager* villager = (Villager*)unit;
				to_build->Life += MIN(to_build->MaxLife - to_build->Life, villager->buildingSpeed);

				if (App->render->CullingCam(unit->entityPosition))
				{
					App->audio->PlayFx(rand() % ((BUILDING_2 - BUILDING_1) + 1) + BUILDING_1);
				}

				if (to_build->Life >= to_build->MaxLife) {
					to_build->Life = to_build->MaxLife;
					to_build->entityTexture = App->entityManager->buildingsDB[to_build->type]->entityTexture;
					to_build->GetBuildingBoundaries();
					to_build->state = IDLE;
					state = COMPLETED;
				}
				else if (to_build->Life > to_build->MaxLife / 1.5f) {
					to_build->entityTexture = App->entityManager->constructingPhase3;
					to_build->GetBuildingBoundaries();
				}
				else if (to_build->Life > to_build->MaxLife / 3) {
					to_build->entityTexture = App->entityManager->constructingPhase2;
					to_build->GetBuildingBoundaries();
				}
			}
			else
				unit->order_list.push_front(new MoveToOrder(unit, to_build->collider->pos));
		}
		else
			state = COMPLETED;
	}
}

bool BuildOrder::CheckCompletion(Unit* unit) 
{
	return (unit->currentAnim->at(unit->currentDirection).Finished());
}
