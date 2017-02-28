#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include "Module.h"
#include "Entity.h"
#include "Unit.h"

class Entity;

class EntityManager : public Module {
public:
	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Unit* CreateUnit(int posX, int posY, bool isEnemy, unitType type, unitFaction race);
	/*Entity* CreateBuilding(buildingType type, int posX, int posY, bool isEnemy);
	Entity* CreateResource(resourceType type, int posX, int posY, bool isEnemy);
	Entity* CreateAnimal(animalType type, int posX, int posY, bool isEnemy);*/

	void DeleteUnit(Unit* unit, bool isEnemy);
	void OnCollisionEnter(Collider* c1, Collider* c2);
	//void OnCollisionExit(Collider* c1, Collider* c2);

private:
	void DestroyEntity(Entity* entity);

private:
	list<Unit*> friendlyUnitList;
	list<Unit*> enemyUnitList;
	//list<Unit*> buildingList;
	list<Unit*> removeUnitList;

public:
	//list<Entity*> selectedEntityList;
	int nextID;

};

#endif // !__ENTITY_MANAGER__


