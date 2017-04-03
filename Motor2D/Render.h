#ifndef __RENDER_H__
#define __RENDER_H__

#include "SDL/include/SDL.h"
#include "Module.h"
#include "p2Point.h"


#include <deque>

struct Sprite
{
	SDL_Rect rect = { 0, 0, 0, 0 };
	SDL_Texture* texture = nullptr;
	iPoint pos = { 0, 0 };
	int priority = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	int r = 0;
	int g = 0;
	int b = 0;
	int radius = 0;
};

class Render : public Module
{
public:

	Render();

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool use_camera = true, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	
	// Set background color
	void SetBackgroundColor(SDL_Color color);

	//Move camera with cursor
	pair<int,int> MoveCameraWithCursor(float dt);

public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
	SDL_Rect		culling_cam;
	std::deque<Sprite> sprites_toDraw;
	std::deque<Sprite> ui_toDraw;

private:

	bool			vsync = false;
};

#endif // __RENDER_H__