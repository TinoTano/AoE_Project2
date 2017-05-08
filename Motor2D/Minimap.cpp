#include "Application.h"
#include "Minimap.h"
#include "Render.h"
#include "FileSystem.h"
#include "Textures.h"
#include "Map.h"
#include "EntityManager.h"
#include "p2Log.h"
#include <math.h>

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

Minimap::Minimap() : Module()
{
	name = "minimap";
}

//Destructor
Minimap::~Minimap()
{}

bool Minimap::Awake(pugi::xml_node& config)
{
	return true;
}

bool Minimap::CleanUp()
{
	return true;
}

void Minimap::DrawTerrain(int x, int y, int r, int g, int b)
{
	SDL_Rect rect;
	rect.x = 1105 - App->render->camera.x + (x * 0.035);
	rect.y = 549 - App->render->camera.y + (y * 0.035);
	rect.w = 2;
	rect.h = 2;
	App->render->DrawQuad(rect, r, g, b, true);
}

void Minimap::DrawUnits()
{
	for (list<Unit*>::iterator it = App->entityManager->friendlyUnitList.begin(); it != App->entityManager->friendlyUnitList.end(); it++) {

		SDL_Rect rect;
		rect.x = 1105 - App->render->camera.x + ((*it)->entityPosition.x * 0.035);
		rect.y = 549 - App->render->camera.y + ((*it)->entityPosition.y * 0.035);
		rect.w = 4;
		rect.h = 4;
		App->render->DrawQuad(rect, 0, 0, 255, true);
	}

	for (list<Unit*>::iterator it = App->entityManager->enemyUnitList.begin(); it != App->entityManager->enemyUnitList.end(); it++) {

		if ((*it)->isActive) {
			SDL_Rect rect;
			rect.x = 1105 - App->render->camera.x + ((*it)->entityPosition.x * 0.035);
			rect.y = 549 - App->render->camera.y + ((*it)->entityPosition.y * 0.035);
			rect.w = 4;
			rect.h = 4;
			App->render->DrawQuad(rect, 255, 0, 0, true);
		}
	}
}