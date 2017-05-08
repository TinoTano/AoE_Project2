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

void Minimap::DrawTerrain(int x, int y)
{
	Sprite minimapTile;

	minimapTile.rect.x = 1105 - App->render->camera.x + (x * 0.035);
	minimapTile.rect.y = 549 - App->render->camera.y + (y * 0.035);
	minimapTile.rect.w = 2;
	minimapTile.rect.h = 2;

	minimapTile.priority = 20;
	minimapTile.g = 153;
	minimapTile.b = 51;

	App->render->ui_toDraw.push_back(minimapTile);
}

void Minimap::DrawUnits()
{

	Sprite minimapTile;

	minimapTile.rect.w = 4;
	minimapTile.rect.h = 4;
	minimapTile.priority = 21;

	for (list<Unit*>::iterator it = App->entityManager->friendlyUnitList.begin(); it != App->entityManager->friendlyUnitList.end(); it++) {

		minimapTile.rect.x = 1105 - App->render->camera.x + ((*it)->entityPosition.x * 0.035);
		minimapTile.rect.y = 549 - App->render->camera.y + ((*it)->entityPosition.y * 0.035);

		minimapTile.b = 255;

		App->render->ui_toDraw.push_back(minimapTile);
	}

	for (list<Unit*>::iterator it = App->entityManager->enemyUnitList.begin(); it != App->entityManager->enemyUnitList.end(); it++) {

		if ((*it)->isActive) {
			minimapTile.rect.x = 1105 - App->render->camera.x + ((*it)->entityPosition.x * 0.035);
			minimapTile.rect.y = 549 - App->render->camera.y + ((*it)->entityPosition.y * 0.035);

			minimapTile.r = 255;

			App->render->ui_toDraw.push_back(minimapTile);
		}
	}
}