#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"

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
	for (list<Unit*>::iterator it = unitList.begin(); it != unitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	return true;
}

bool EntityManager::PostUpdate()
{
	if (removeEntityList.size() > 0) {
		list<Entity*>::iterator i = removeEntityList.begin();

		while (i != removeEntityList.end())
		{
			list<Entity*>::iterator entityToDestroy = i;
			++i;
			DestroyEntity((*entityToDestroy));
		}

		removeEntityList.clear();
	}
	return true;
}

bool EntityManager::CleanUp()
{
	LOG("Freeing EntityManager");

	for (list<Unit*>::iterator it = unitList.begin(); it != unitList.end(); it++) {
		RELEASE((*it));
	}
	unitList.clear();

	for (list<Entity*>::iterator it = removeEntityList.begin(); it != removeEntityList.end(); it++) {
		RELEASE((*it));
	}
	removeEntityList.clear();

	return true;
}

Unit* EntityManager::CreateUnit(int posX, int posY, bool isEnemy, unitType type, unitRace race)
{
	Unit* unit = new Unit(posX, posY, isEnemy, type, race);
	unit->entityID = nextID;
	nextID++;
	unitList.push_back(unit);

	return unit;
}

void EntityManager::DeleteEntity(Entity * entity)
{
	if (entity != nullptr){
		removeEntityList.push_back(entity);
	}
}

void EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	//WIP
	if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
		for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
			if ((*it)->collider == c1) {
				Unit* unit = (Unit*)(*it);
				unit->entityTexture = unit->unitAttack;
				unit->state = ATTACKING;
				unit->SetAnim(unit->currentDirection);
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
		list<Entity*>::iterator it = removeEntityList.begin();

		while (it != removeEntityList.end())
		{
			if (*it == entity)
			{
				removeEntityList.remove(*it);
				delete entity;
				return;
			}
			++it;
		}
	}
}
