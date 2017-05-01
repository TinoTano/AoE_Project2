#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
#include "Render.h"
#include "Villager.h"
#include "Collision.h"

#define NOTHUD SDL_Rect{0, 30, 1920 , 622}
#define CAMERA_OFFSET_X App->render->camera.x
#define CAMERA_OFFSET_Y App->render->camera.y

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

	bool IsOccupied(iPoint tile, iPoint ignore_tile = { -1,-1 });

	bool LoadGameData();

	Unit* CreateUnit(int posX, int posY, unitType type);
	Building* CreateBuilding(int posX, int posY, buildingType type);
	Resource* CreateResource(int posX, int posY, resourceItem type);

	void DeleteUnit(Unit* unit);
	void DeleteBuilding(Building* building);
	void DeleteResource(Resource* resource);
	void OnCollision(Collision_data& col_data);
	void FillSelectedList();

	Resource* FindNearestResource(resourceType type, iPoint pos);

private:
	void DestroyEntity(Entity* entity);

private:
	list<Unit*> removeUnitList;
	list<Building*> removeBuildingList;
	list<Resource*> removeResourceList;

	SDL_Rect multiSelectionRect = { 0,0,0,0 };
	Timer click_timer;

	map<int, Unit*> unitsDB;
	map<int, Building*> buildingsDB;
	map<int, Resource*> resourcesDB;

	Entity* clicked_entity = nullptr;

	int mouseX;
	int mouseY;

public:
	int nextID;
	list<Entity*> selectedEntityList;
	COLLIDER_TYPE selectedListType = COLLIDER_NONE;
	list<Unit*> friendlyUnitList;
	list<Unit*> enemyUnitList;
	list<Building*> friendlyBuildingList;
	list<Building*> enemyBuildingList;
	list<Resource*> resourceList;

	bool placingBuilding = false;
	buildingType creatingBuildingType = ORC_BARRACKS;
};

#endif // !__ENTITY_MANAGER__





