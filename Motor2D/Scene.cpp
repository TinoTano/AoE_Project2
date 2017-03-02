#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "Scene.h"
#include "p2Log.h"
#include "EntityManager.h"
#include "Unit.h"
#include "FogOfWar.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	if (App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	//Test
	villager = App->entityManager->CreateUnit(350, 350, false, ELVEN_ARCHER, FREE_MEN_UNIT);
	villager2 = App->entityManager->CreateUnit(600, 400, true, ELVEN_ARCHER, FREE_MEN_UNIT);
	TestBuilding = App->entityManager->CreateBuilding(150, 100, true, BARRACKS, FREE_MEN_BUILDING);

	App->fog->CreateFog(App->map->data.width*App->map->data.tile_width, App->map->data.height*App->map->data.tile_height);
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
	}

	App->render->MoveCameraWithCursor(dt);
	App->map->Draw();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		villager->SetDestination();
	}

	if (debug) {
		const list<iPoint>* path = App->pathfinding->GetLastPath();

		for (list<iPoint>::const_iterator it = path->begin(); it != path->end(); it++) {
			iPoint pos = App->map->MapToWorld((*it).x, (*it).y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	iPoint villagerToWorld = App->map->WorldToMap(villager->entityPosition.x, villager->entityPosition.y);
	string str = to_string(map_coordinates.x) + "," + to_string(map_coordinates.y) + "  " + to_string(villagerToWorld.x) + "," + to_string(villagerToWorld.y);
	App->win->SetTitle(str.c_str());

	//App->fog->removeFog(villager->entityPosition.x, villager->entityPosition.y);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}