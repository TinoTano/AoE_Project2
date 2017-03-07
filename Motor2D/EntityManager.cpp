#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"
#include "Application.h"
#include "Scene.h"
#include "Unit.h"
#include "FogOfWar.h"
#include "Render.h"

EntityManager::EntityManager() : Module()
{
	name = "entityManager";
	nextID = 1;
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node & config)
{
	

	return true;
}

bool EntityManager::Start()
{
	LOG("Starting EntityManager");
	bool ret = true;
	drawMultiSelectionRect = false;
	return ret;
}

bool EntityManager::PreUpdate()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	int mouseX;
	int mouseY;
	App->input->GetMousePosition(mouseX, mouseY);
	mouseX -= App->render->camera.x;
	mouseY -= App->render->camera.y;

	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
		App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
		App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		Unit* clickedUnit = nullptr;
		for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
			if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
				mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
				//if ((*it)->isVisible) {
					clickedUnit = (*it);
				//}
			}
		}

		for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
			if ((*it)->isSelected) {
				(*it)->SetDestination();
				if (clickedUnit != nullptr) {
					(*it)->attackUnitTarget = clickedUnit;
				}
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		multiSelectionRect.x = mouseX;
		multiSelectionRect.y = mouseY;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		multiSelectionRect.w = mouseX - multiSelectionRect.x;
		multiSelectionRect.h = mouseY - multiSelectionRect.y;
		if (drawMultiSelectionRect == false) {
			drawMultiSelectionRect = true;
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		if (drawMultiSelectionRect == true) {
			drawMultiSelectionRect = false;

			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if ((*it)->entityPosition.x < multiSelectionRect.w && (*it)->entityPosition.x > multiSelectionRect.x &&
					(*it)->entityPosition.y < multiSelectionRect.h && (*it)->entityPosition.y > multiSelectionRect.y) {
					(*it)->isSelected = true;
				}
			}
			multiSelectionRect = { 0,0,0,0 };
		}
	}

	if (drawMultiSelectionRect) {
		App->render->DrawQuad(multiSelectionRect, 255, 255, 255, 255, false);
	}
	
	return true;
}

bool EntityManager::PostUpdate()
{
	if (removeUnitList.size() > 0) {
		list<Unit*>::iterator i = removeUnitList.begin();

		while (i != removeUnitList.end())
		{
			list<Unit*>::iterator unitToDestroy = i;
			++i;
			DestroyEntity((*unitToDestroy));
		}

		removeUnitList.clear();
	}

	if (removeBuildingList.size() > 0) {
		list<Building*>::iterator i = removeBuildingList.begin();

		while (i != removeBuildingList.end())
		{
			list<Building*>::iterator buildingToDestroy = i;
			++i;
			DestroyEntity((*buildingToDestroy));
		}

		removeBuildingList.clear();
	}
	return true;
}

bool EntityManager::CleanUp()
{
	LOG("Freeing EntityManager");

	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		RELEASE((*it));
	}
	friendlyUnitList.clear();

	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		RELEASE((*it));
	}
	enemyUnitList.clear();

	for (list<Unit*>::iterator it = removeUnitList.begin(); it != removeUnitList.end(); it++) {
		RELEASE((*it));
	}
	removeUnitList.clear();

	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		RELEASE((*it));
	}
	friendlyBuildingList.clear();

	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		RELEASE((*it));
	}
	enemyBuildingList.clear();

	for (list<Building*>::iterator it = removeBuildingList.begin(); it != removeBuildingList.end(); it++) {
		RELEASE((*it));
	}
	removeBuildingList.clear();

	return true;
}

Unit* EntityManager::CreateUnit(int posX, int posY, bool isEnemy, unitType type)
{
	Unit* unit = new Unit(posX, posY, isEnemy, type);
	unit->entityID = nextID;
	nextID++;
	if (!isEnemy) {
		friendlyUnitList.push_back(unit);
	}
	else {
		enemyUnitList.push_back(unit);
	}


	return unit;
}

Building* EntityManager::CreateBuilding(int posX, int posY, bool isEnemy, buildingType type)
{
	Building* building = new Building(posX, posY, isEnemy, type);
	building->entityID = nextID;
	nextID++;
	if (!isEnemy) {
		friendlyBuildingList.push_back(building);
	}
	else {
		enemyBuildingList.push_back(building);
	}


	return building;
}

void EntityManager::DeleteUnit(Unit* unit, bool isEnemy)
{
	if (unit != nullptr) {
		removeUnitList.push_back(unit);
		if (isEnemy) {
			enemyUnitList.remove(unit);
		}
		else {
			friendlyUnitList.remove(unit);
		}
	}
}

void EntityManager::DeleteBuilding(Building* unit, bool isEnemy)
{
	if (unit != nullptr) {
		removeBuildingList.push_back(unit);
		if (isEnemy) {
			enemyBuildingList.remove(unit);
		}
		else {
			friendlyBuildingList.remove(unit);
		}
	}
}

void EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_FRIENDLY_UNIT) {
		if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if ((*it)->collider == c1 && (*it)->attackUnitTarget != nullptr) {
					(*it)->SetState(UNIT_ATTACKING);
					/*if (c2->type == COLLIDER_ENEMY_UNIT) {
						for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackUnitTarget = (*it2);
								break;
							}
						}
					}
					if (c2->type == COLLIDER_ENEMY_BUILDING) {
						for (list<Building*>::iterator it2 = enemyBuildingList.begin(); it2 != enemyBuildingList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackBuildingTarget = (*it2);
								break;
							}
						}
					}*/
					break;
				}
			}
		}
	}
	if (c1->type == COLLIDER_FRIENDLY_BUILDING && c2->type == COLLIDER_ENEMY_UNIT) {
		for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
			if ((*it)->collider == c1) {
				if ((*it)->canAttack) {
					(*it)->state = BUILDING_ATTACKING;
					/*for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
						if ((*it2)->collider == c2) {
							(*it)->attackUnitTarget = (*it2);
							break;
						}
					}*/
				}
				break;
			}
		}
	}

	//Enemy
	if (c2->type == COLLIDER_ENEMY_UNIT) {
		if (c1->type == COLLIDER_FRIENDLY_UNIT || c1->type == COLLIDER_FRIENDLY_BUILDING) {
			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				if ((*it)->collider == c2 && (*it)->attackUnitTarget != nullptr) {
					(*it)->SetState(UNIT_ATTACKING);
					/*if (c1->type == COLLIDER_FRIENDLY_UNIT) {
						for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
							if ((*it2)->collider == c1) {
								(*it)->attackUnitTarget = (*it2);
								break;
							}
						}
					}
					if (c1->type == COLLIDER_FRIENDLY_BUILDING) {
						for (list<Building*>::iterator it2 = friendlyBuildingList.begin(); it2 != friendlyBuildingList.end(); it2++) {
							if ((*it2)->collider == c1) {
								(*it)->attackBuildingTarget = (*it2); 
								break;
							}
						}
					}*/
					break;
				}
			}
		}
	}
	if (c2->type == COLLIDER_ENEMY_BUILDING && c1->type == COLLIDER_FRIENDLY_UNIT) {
		for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
			if ((*it)->collider == c2 && (*it)->attackUnitTarget != nullptr) {
				if ((*it)->canAttack) {
					(*it)->state = BUILDING_ATTACKING;
					/*for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
						if ((*it2)->collider == c1) {
							(*it)->attackUnitTarget = (*it2);
							break;
						}
					}*/
				}
				break;
			}
		}
	}
}


void EntityManager::DestroyEntity(Entity * entity)
{
	if (entity != nullptr) {
		list<Unit*>::iterator it = removeUnitList.begin();

		while (it != removeUnitList.end())
		{
			if (*it == entity)
			{
				removeUnitList.remove(*it);
				delete entity;
				return;
			}
			++it;
		}
	}
}