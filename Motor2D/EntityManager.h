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

enum CursorHovering {
	HOVERING_TERRAIN, HOVERING_ALLY_UNIT, HOVERING_ENEMY, HOVERING_RESOURCE, HOVERING_ALLY_BUILDING 
};

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

	bool LoadGameData();

	Unit* CreateUnit(int posX, int posY, unitType type);
	Building* CreateBuilding(int posX, int posY, buildingType type);
	Resource* CreateResource(int posX, int posY, resourceItem type);

	void DeleteUnit(Unit* unit);
	void DeleteBuilding(Building* building);
	void DeleteResource(Resource* resource);
	void OnCollision(Collision_data& col_data);

	void FillSelectedList();
	void DrawSelectedList();
	Collider* CheckCursorHover(iPoint cursor_pos);

	Resource* FindNearestResource(resourceType type, iPoint pos);
	Entity* FindTarget(Unit* unit);
	void Untarget(Entity* destroyed_entity);

	void RallyCall(Entity* entity);

private:
	void DestroyEntity(Entity* entity);

private:
	list<Unit*> removeUnitList;
	list<Building*> removeBuildingList;
	list<Resource*> removeResourceList;

	SDL_Rect multiSelectionRect = { 0,0,0,0 };
	Timer click_timer;


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

	CursorHovering cursor_hover = HOVERING_TERRAIN;
	bool placingBuilding = false;
	float dt = 0;
	buildingType creatingBuildingType = ORC_BARRACKS;
	SDL_Rect NotHUD;
	Building* buildingToCreate = nullptr;

	map<int, Unit*> unitsDB;
	map<int, Building*> buildingsDB;
	map<int, Resource*> resourcesDB;
};

#endif // !_ENTITY_MANAGER_





