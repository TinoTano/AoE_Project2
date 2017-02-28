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

	return true;
}

Unit* EntityManager::CreateUnit(int posX, int posY, bool isEnemy, unitType type, unitFaction race)
{
	Unit* unit = new Unit(posX, posY, isEnemy, type, race);
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

void EntityManager::DeleteUnit(Unit* unit, bool isEnemy)
{
	if (unit != nullptr){
		removeUnitList.push_back(unit);
		if (isEnemy) {
			enemyUnitList.remove(unit);
		}
		else {
			friendlyUnitList.remove(unit);
		}
	}
}

void EntityManager::OnCollisionEnter(Collider * c1, Collider * c2)
{
	//WIP
	if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
		for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
			if ((*it)->collider == c1) {
				(*it)->SetState(ATTACKING);
				if (c2->type == COLLIDER_ENEMY_UNIT) {
					for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
						if ((*it2)->collider == c2) {
							(*it)->attackUnitTarget = (*it2);
						}
					}
				}
				if (c2->type == COLLIDER_ENEMY_BUILDING) {
					for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
						if ((*it2)->collider == c2) {
							(*it)->attackUnitTarget = (*it2);
						}
					}
				}
			}
		}
	}
	if (c1->type == COLLIDER_FRIENDLY_UNIT || c1->type == COLLIDER_FRIENDLY_BUILDING) {
		for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
			if ((*it)->collider == c2) {
				(*it)->SetState(ATTACKING);
				if (c1->type == COLLIDER_FRIENDLY_UNIT) {
					for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
						if ((*it2)->collider == c1) {
							(*it)->attackUnitTarget = (*it2);
						}
					}
				}
				if (c1->type == COLLIDER_FRIENDLY_BUILDING) {
					for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
						if ((*it2)->collider == c1) {
							(*it)->attackUnitTarget = (*it2);
						}
					}
				}
			}
		}
	}
}

/*void EntityManager::OnCollisionExit(Collider * c1, Collider * c2)
{
	if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
		for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
			if ((*it)->collider == c1) {
				Unit* unit = (Unit*)(*it);
				unit->state = MOVING;
				unit->SetAnim(unit->currentDirection);
			}
		}
	}
}*/

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
