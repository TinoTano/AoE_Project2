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
#include "Gui.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node & config)
{
	LOG("Loading Scene");
	bool ret = true;
	active = false;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	active = true;
	if (App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	// My changes --------------------------------------------------------------

	App->entityManager->CreateUnit(400, 250, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(370, 300, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(350, 350, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(350, 400, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(370, 500, false, ELVEN_ARCHER);

	App->entityManager->CreateUnit(-350, 300, true, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-350, 500, true, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-50, 700, true, ELVEN_ARCHER);

	troll = App->entityManager->CreateUnit(50, 150, false, TROLL_MAULER);
	troll->isHero = true;

	my_townCenter = App->entityManager->CreateBuilding(580, 350, false, TOWN_CENTER);
	enemy_townCenter = App->entityManager->CreateBuilding(-580, 400, true, ORC_BARRACKS);

	// -------------------------------------------------------------------------

	App->fog->CreateFog(App->map->data.mapWidth, App->map->data.mapHeight);
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
	App->map->Draw();
	
	list<list<iPoint>*>* paths = nullptr;
	paths = App->pathfinding->GetPaths();

	if (debug && paths != nullptr) {

		for (list<list<iPoint>*>::const_iterator it = paths->begin(); it != paths->end(); it++) {
			list<iPoint>* path = (*it);

			for (list<iPoint>::iterator it2 = path->begin(); it2 != path->end(); it2++) {
				iPoint pos = App->map->MapToWorld((*it2).x, (*it2).y);
				App->render->Blit(debug_tex, pos.x, pos.y);
			}
		}
	}

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