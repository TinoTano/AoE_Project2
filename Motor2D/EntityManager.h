#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"
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

	iPoint FindNearestResource(resourceType type, iPoint pos);

private:
	void DestroyEntity(Entity* entity);

private:
	list<Unit*> removeUnitList;
	list<Building*> removeBuildingList;
	list<Resource*> removeResourceList;

	SDL_Rect multiSelectionRect = { 0,0,0,0 };
	int timesClicked = 0;
	float doubleClickTimer = 0;

	map<int, Unit*> unitsDB;
	map<int, Building*> buildingsDB;
	map<int, Resource*> resourcesDB;

	int mouseX;
	int mouseY;

	Unit* clickedUnit = nullptr;
	Building* clickedBuilding = nullptr;
	Resource* clickedResource = nullptr;
	
public:
	int nextID;
	list<Unit*> selectedUnitList;
	list<Building*> selectedBuildingList;
	Resource* selectedResource = nullptr;
	list<Unit*> friendlyUnitList;
	list<Unit*> enemyUnitList;
	list<Building*> friendlyBuildingList;
	list<Building*> enemyBuildingList;
	list<Resource*> resourceList;
};

#endif // !__ENTITY_MANAGER__





