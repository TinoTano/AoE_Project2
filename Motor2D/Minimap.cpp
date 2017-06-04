#include "Application.h"
#include "Minimap.h"
#include "Render.h"
#include "FileSystem.h"
#include "Textures.h"
#include "Map.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "Window.h"
#include "Gui.h"
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


bool Minimap::CleanUp()
{
	return true;
}

bool Minimap::Start()
{

	minimapClickable.w = 247;
	minimapClickable.h = 120;

	minimapPos.x = 1105;
	minimapPos.y = 549;

	minimapRatio = 0.035;

	return true;
}

bool Minimap::Update(float dt)
{
	SDL_Point mousePos;

	iPoint cameraOldPos = { App->render->camera.x, App->render->camera.y };

	App->input->GetMousePosition(mousePos.x, mousePos.y);

	minimapNewPos = mousePos;

	mousePos.x -= App->render->camera.x;
	mousePos.y -= App->render->camera.y;


	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
		if (SDL_PointInRect(&mousePos, &minimapClickable))
		{
			App->render->camera.x = (-minimapNewPos.x + minimapPos.x + offsetX) / minimapRatio;
			App->render->camera.y = (-minimapNewPos.y + minimapPos.y + offsetY) / minimapRatio;
			App->render->culling_cam.x = -App->render->camera.x - 300;
			App->render->culling_cam.y = -App->render->camera.y - 300;

			std::pair<int, int> movement;
			movement.first = App->render->camera.x - cameraOldPos.x;
			movement.second = App->render->camera.y - cameraOldPos.y;

			App->gui->ScreenMoves(movement);

			uint x, y;

			App->win->GetWindowSize(x, y);

			offsetX = ((x * minimapRatio) / 2);
			offsetY = ((y * minimapRatio) / 2);
		}
	}

	return true;
}

void Minimap::GetClickableArea(std::pair<int, int> position)
{
	minimapClickable.x = position.first + 72;
	minimapClickable.y = position.second + 25;

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

	for (list<Building*>::iterator it = App->entityManager->player->buildings.begin(); it != App->entityManager->player->buildings.end(); it++) {

		SDL_Rect rect;
		rect.x = minimapPos.x - App->render->camera.x + ((*it)->entityPosition.x * minimapRatio);
		rect.y = minimapPos.y - App->render->camera.y + ((*it)->entityPosition.y * minimapRatio);
		rect.w = 4;
		rect.h = 4;
		App->render->DrawQuad(rect, 0, 255, 255, true);
	}

	for (list<Building*>::iterator it = App->entityManager->AI_faction->buildings.begin(); it != App->entityManager->AI_faction->buildings.end(); it++) {

		if ((*it)->isActive) {
			SDL_Rect rect;
			rect.x = minimapPos.x - App->render->camera.x + ((*it)->entityPosition.x * minimapRatio);
			rect.y = minimapPos.y - App->render->camera.y + ((*it)->entityPosition.y * minimapRatio);
			rect.w = 4;
			rect.h = 4;
			App->render->DrawQuad(rect, 255, 0, 255, true);
		}
	}
}


void Minimap::DrawCamera()
{
	uint x, y;
	SDL_Rect rect;

	App->win->GetWindowSize(x, y);
	rect.x = minimapPos.x - App->render->camera.x - (App->render->camera.x * minimapRatio);
	rect.y = minimapPos.y - App->render->camera.y - (App->render->camera.y * minimapRatio);
	rect.w = x * minimapRatio;
	rect.h = y * minimapRatio;

	App->render->DrawQuad(rect, 255, 255, 255, false);
}
