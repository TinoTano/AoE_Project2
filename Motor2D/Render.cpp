#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "p2Log.h"
#include "Input.h"
#include "Textures.h"
#include "Map.h"
#include "FogOfWar.h"
#include "Minimap.h"
#include "SceneManager.h"
#include <algorithm>

#define VSYNC true

Render::Render() : Module()
{
	name = "renderer";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		vsync = true;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}
	culling_cam = camera;

	culling_cam.w += (300 * 2);
	culling_cam.h += (300 * 2);
	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::PostUpdate()
{

	for (std::deque<Sprite>::iterator it = sprites_toDraw.begin(); it != sprites_toDraw.end(); it++) {
		if ((*it).priority < 0) {
			sprites_toDraw.erase(it);
		}
	}
	std::sort(sprites_toDraw.begin(), sprites_toDraw.end(), [](const Sprite& lhs, const Sprite& rhs) { return lhs.priority < rhs.priority; });

	for (int it = 0; it < sprites_toDraw.size(); it++)
	{
		if (sprites_toDraw[it].change_color) {
			SDL_SetTextureColorMod(sprites_toDraw[it].texture, sprites_toDraw[it].r, sprites_toDraw[it].g, sprites_toDraw[it].b);
		}
		else {
			SDL_SetTextureColorMod(sprites_toDraw[it].texture, 255, 255, 255);
		}
		if (sprites_toDraw[it].texture != nullptr && sprites_toDraw[it].flip >= 0 && sprites_toDraw[it].flip < 3)
			Blit(sprites_toDraw[it].texture, sprites_toDraw[it].pos.x, sprites_toDraw[it].pos.y, &sprites_toDraw[it].rect, sprites_toDraw[it].flip);
		else
		{
			if (sprites_toDraw[it].radius == 0) DrawQuad(sprites_toDraw[it].rect, sprites_toDraw[it].r, sprites_toDraw[it].g, sprites_toDraw[it].b, sprites_toDraw[it].filled);
			else DrawCircle(sprites_toDraw[it].pos.x, sprites_toDraw[it].pos.y, sprites_toDraw[it].radius, sprites_toDraw[it].r, sprites_toDraw[it].g, sprites_toDraw[it].b);
		}
	}

	sprites_toDraw.clear();

	for (std::deque<Sprite>::iterator it = ui_toDraw.begin(); it != ui_toDraw.end(); it++) {
		if ((*it).priority < 0) {
			ui_toDraw.erase(it);
		}
	}
	std::sort(ui_toDraw.begin(), ui_toDraw.end(), [](const Sprite& lhs, const Sprite& rhs) { return lhs.priority < rhs.priority; });

	for (int it = 0; it < ui_toDraw.size(); it++)
	{
		if (ui_toDraw[it].texture != nullptr && ui_toDraw[it].flip >= 0 && ui_toDraw[it].flip < 3)
			Blit(ui_toDraw[it].texture, ui_toDraw[it].pos.x, ui_toDraw[it].pos.y, &ui_toDraw[it].rect, ui_toDraw[it].flip);
		else
		{
			if (ui_toDraw[it].radius == 0)
				DrawQuad(ui_toDraw[it].rect, ui_toDraw[it].r, ui_toDraw[it].g, ui_toDraw[it].b, ui_toDraw[it].filled, ui_toDraw[it].a);
			else DrawCircle(ui_toDraw[it].pos.x, ui_toDraw[it].pos.y, ui_toDraw[it].radius, ui_toDraw[it].r,ui_toDraw[it].g, ui_toDraw[it].b);
		}
	}

	ui_toDraw.clear();

	// Minimap ===================================================================

	if (App->sceneManager->current_scene == App->sceneManager->level1_scene || App->sceneManager->current_scene == App->sceneManager->play_scene)
	{
		for (list<MapLayer*>::iterator it = App->map->data.layers.begin(); it != App->map->data.layers.end(); it++)
		{
			MapLayer* layer = *it;

			if (layer->properties.Get("Nodraw") != 0)
				continue;

			for (int y = 0; y < App->map->data.height; ++y)
			{
				for (int x = 0; x < App->map->data.width; ++x)
				{
					int tile_id = layer->Get(x, y);
					int visibility = App->fog->Get(x, y);

					iPoint tileWorld = App->map->MapToWorld(x, y);

					if ((0 < tile_id && tile_id < 6 || 12 > tile_id && tile_id < 14) && visibility != 0)
						App->minimap->DrawTerrain(tileWorld.x, tileWorld.y, 0, 153, 51);

					else if (visibility != 0)
						App->minimap->DrawTerrain(tileWorld.x, tileWorld.y, 105, 105, 105);
				}
			}
		}

		App->minimap->DrawUnits();
		App->minimap->DrawCamera();
	}

	// ==============================================================================


	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();


	culling_cam.x = data.child("cull").attribute("x").as_int();
	culling_cam.y = data.child("cull").attribute("y").as_int();

	App->gui->LoadToScreen(data);
	return true;
}

// Save Game State
bool Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	pugi::xml_node cull = data.append_child("cull");
	cull.append_attribute("x") = culling_cam.x;
	cull.append_attribute("y") = culling_cam.y;

	App->gui->SaveToScreen(data);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

pair <int,int> Render::MoveCameraWithCursor(float dt)
{
	int mousePosX;
	int mousePosY;
	pair<int, int> movement;
	App->input->GetMousePosition(mousePosX, mousePosY);


	if (mousePosX < 10 && camera.x < cameraScene.left)	//Move left
		movement.first = 10;

	else if (mousePosX > camera.w - 10 && camera.x > cameraScene.right)  //Move right
		movement.first = -10;

	if (mousePosY < 10 && camera.y < cameraScene.up)	//Move up
		movement.second = 10;

	else if (mousePosY > camera.h - 10 && camera.y > cameraScene.down)  //Move down
		movement.second = -10;

	//LOG("cam: %d, %d", camera.x, camera.y);
	//LOG("iso_cam: %d, %d", cam_pos_iso.x, cam_pos_iso.y);
	camera.x += movement.first;
	camera.y += movement.second;

	culling_cam.x = -camera.x - 300;
	culling_cam.y = -camera.y - 300;

	return movement;
}

bool Render::CullingCam(iPoint point) {

	SDL_Point p = { point.x, point.y };
	return SDL_PointInRect(&p, &culling_cam);

}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, bool use_camera, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x =  x * scale;
	rect.y =  y * scale;
	if (use_camera) {
		rect.x += (int)(camera.x * speed);
		rect.y += (int)(camera.y * speed);
	}

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, bool filled, Uint8 a , bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool isIsometric, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	if (use_camera)
	{
		x *= scale;
		y *= scale;
		x += camera.x;
		y += camera.y;
	}

	float angleMultiplier = 1;
	if (isIsometric) {
		angleMultiplier = 0.5f;
	}
	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)((y + radius * sin(i * factor) * angleMultiplier));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawIsometricRect(iPoint center, uint width) const
{
	App->render->DrawLine(center.x, center.y - (width * 0.25f), center.x + (width * 0.5f), center.y, 255, 255, 255, 255);
	App->render->DrawLine(center.x, center.y - (width * 0.25f), center.x - (width * 0.5f), center.y, 255, 255, 255, 255);
	App->render->DrawLine(center.x, center.y + (width * 0.25f), center.x + (width * 0.5f), center.y, 255, 255, 255, 255);
	App->render->DrawLine(center.x, center.y + (width * 0.25f), center.x - (width * 0.5f), center.y, 255, 255, 255, 255);
	return true;
}

bool Render::DrawIsometricCircle(int x1, int y1, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	return DrawCircle(x1, y1, radius, r, g, b, a, true, use_camera);
}
