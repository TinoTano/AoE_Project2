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

	App->entityManager->CreateUnit(350, 300, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-350, 300, true, ELVEN_ARCHER);
	App->entityManager->CreateUnit(350, 350, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-100, 550, true, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-150, 350, false, ELVEN_ARCHER);
	App->entityManager->CreateUnit(-400, 500, true, ELVEN_ARCHER);
	troll = App->entityManager->CreateUnit(50, 150, false, TROLL_MAULER);
	
	my_townCenter = App->entityManager->CreateBuilding(580, 350, false, TOWN_CENTER);

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
	App->gui->ScreenMoves(App->render->MoveCameraWithCursor(dt));

	if (debug) {
		const list<iPoint>* path = App->pathfinding->GetLastPath();

		for (list<iPoint>::const_iterator it = path->begin(); it != path->end(); it++) {
			iPoint pos = App->map->MapToWorld((*it).x, (*it).y);
			App->render->Blit(debug_tex, pos.x, pos.y);
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