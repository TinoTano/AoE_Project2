#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"
#include "Application.h"
#include "Scene.h"

EntityManager::EntityManager() : Module()
{
	name = "entityManager";
	nextID = 1;
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &)
{
	bool ret = true;

	return ret;
}

bool EntityManager::Start()
{
	LOG("Starting EntityManager");
	bool ret = true;

	return ret;
}

bool EntityManager::PreUpdate()
{
	return true;
}

bool EntityManager::Update(float dt)
{
	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
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

Unit* EntityManager::CreateUnit(int posX, int posY, bool isEnemy, unitType type, unitFaction faction)
{
	Unit* unit = new Unit(posX, posY, isEnemy, type, faction);
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

Building* EntityManager::CreateBuilding(int posX, int posY, bool isEnemy, buildingType type, buildingFaction faction)
{
	Building* building = new Building(posX, posY, isEnemy, type, faction);
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
	//WIP
	if (c1->type == COLLIDER_FRIENDLY_UNIT) {
		if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if ((*it)->collider == c1) {
					(*it)->SetState(UNIT_ATTACKING);
					if (c2->type == COLLIDER_ENEMY_UNIT) {
						for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackUnitTarget = (*it2);
							}
						}
					}
					if (c2->type == COLLIDER_ENEMY_BUILDING) {
						for (list<Building*>::iterator it2 = enemyBuildingList.begin(); it2 != enemyBuildingList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackBuildingTarget = (*it2);
							}
						}
					}
				}
			}
		}
	}
	if (c1->type == COLLIDER_FRIENDLY_BUILDING && c2->type == COLLIDER_ENEMY_UNIT) {
		for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
			if ((*it)->collider == c1) {
				(*it)->state = BUILDING_ATTACKING;
				for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
					if ((*it2)->collider == c2) {
						(*it)->attackUnitTarget = (*it2);
					}
				}
			}
		}
	}

	//Enemy
	if (c2->type == COLLIDER_ENEMY_UNIT) {
		if (c1->type == COLLIDER_FRIENDLY_UNIT || c1->type == COLLIDER_FRIENDLY_BUILDING) {
			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				if ((*it)->collider == c2) {
					(*it)->SetState(UNIT_ATTACKING);
					if (c1->type == COLLIDER_FRIENDLY_UNIT) {
						for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
							if ((*it2)->collider == c1) {
								(*it)->attackUnitTarget = (*it2);
							}
						}
					}
					if (c1->type == COLLIDER_FRIENDLY_BUILDING) {
						for (list<Building*>::iterator it2 = friendlyBuildingList.begin(); it2 != friendlyBuildingList.end(); it2++) {
							if ((*it2)->collider == c1) {
								(*it)->attackBuildingTarget = (*it2);
							}
						}
					}
				}
			}
		}
	}
	if (c2->type == COLLIDER_ENEMY_BUILDING && c1->type == COLLIDER_FRIENDLY_UNIT) {
		for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
			if ((*it)->collider == c2) {
				(*it)->state = BUILDING_ATTACKING;
				for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
					if ((*it2)->collider == c1) {
						(*it)->attackUnitTarget = (*it2);
					}
				}
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