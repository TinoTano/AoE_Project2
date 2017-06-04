#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "TechTree.h"
#include "Building.h"
#include "Resource.h"
#include "Render.h"
#include "GameFaction.h"
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

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	bool LoadGameData();

	Unit* CreateUnit(int posX, int posY, unitType type);
	Building* CreateBuilding(int posX, int posY, buildingType type);
	Resource* CreateResource(int posX, int posY, resourceItem type);
	Resource* ReLoadResource(int posX, int posY, resourceItem type, SDL_Rect rect); // This is used instead of CreateResource when loading the game again.

	void DeleteEntity(Entity* entity);

	void FillSelectedList();
	void DrawSelectedList();
	Collider* CheckCursorHover(iPoint cursor_pos);

	Resource* FindNearestResource(resourceType type, iPoint pos);
	Building* FindNearestBuilding(Unit* unit);
	Entity* FindTarget(Entity* entity);

	void AddResources(Villager* villager);

	void RallyCall(Entity* entity);

private:
	void DestroyEntity(Entity* entity);

private:
	list<Entity*> removeEntityList;

	SDL_Rect multiSelectionRect = { 0,0,0,0 };
	Timer click_timer;

	Entity* clicked_entity = nullptr;

	int mouseX;
	int mouseY;

public:
	int nextID = 0;
	bool game_stops = false;

	list<Entity*> selectedEntityList;
	COLLIDER_TYPE selectedListType = COLLIDER_NONE;

	list<Entity*> WorldEntityList;
	list<Resource*> resource_list;

	GameFaction* player = nullptr;
	GameFaction* AI_faction = nullptr;

	CursorHovering cursor_hover = HOVERING_TERRAIN;
	bool placingBuilding = false;
	buildingType placing_type = ORC_BARRACKS;
	float dt = 0;
	SDL_Rect NotHUD = { 0,0,0,0 };

	SDL_Texture* constructingPhase1;
	SDL_Texture* constructingPhase2;
	SDL_Texture* constructingPhase3;

	map<int, Unit*> unitsDB;
	map<int, Building*> buildingsDB;
	map<int, Resource*> resourcesDB;
};

#endif // !_ENTITY_MANAGER_





