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

void Minimap::InitMinimap()
{
	minimapPos.x = 1105;
	minimapPos.y = 549;
	minimapRatio = 0.035;
}

void Minimap::DrawTerrain(int x, int y, int r, int g, int b)
{
	SDL_Rect rect;
	rect.x = minimapPos.x - App->render->camera.x + (x * minimapRatio);
	rect.y = minimapPos.y - App->render->camera.y + (y * minimapRatio);
	rect.w = 2;
	rect.h = 2;
	App->render->DrawQuad(rect, r, g, b, true);
}

void Minimap::DrawUnits()
{
	for (list<Unit*>::iterator it = App->entityManager->player->units.begin(); it != App->entityManager->player->units.end(); it++) {

		SDL_Rect rect;
		rect.x = minimapPos.x - App->render->camera.x + ((*it)->entityPosition.x * minimapRatio);
		rect.y = minimapPos.y - App->render->camera.y + ((*it)->entityPosition.y * minimapRatio);
		rect.w = 4;
		rect.h = 4;
		App->render->DrawQuad(rect, 0, 0, 255, true);
	}

	for (list<Unit*>::iterator it = App->entityManager->AI_faction->units.begin(); it != App->entityManager->AI_faction->units.end(); it++) {

		if ((*it)->isActive) {
			SDL_Rect rect;
			rect.x = minimapPos.x - App->render->camera.x + ((*it)->entityPosition.x * minimapRatio);
			rect.y = minimapPos.y - App->render->camera.y + ((*it)->entityPosition.y * minimapRatio);
			rect.w = 4;
			rect.h = 4;
			App->render->DrawQuad(rect, 255, 0, 0, true);
		}
	}
}