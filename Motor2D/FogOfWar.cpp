#include "FogOfWar.h"
#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "p2Log.h"
#include "Textures.h"
#include "p2Defs.h"

FogOfWar::FogOfWar()
{
	
}


FogOfWar::~FogOfWar()
{
}

bool FogOfWar::Start()
{
	fogRemover = App->tex->LoadImageAsSurface("textures/fogCercle.png");
	return true;
}

bool FogOfWar::Update(float dt)
{
	DrawFog(fogSurface, 0, 0);
	return true;
}

void FogOfWar::DrawFog(SDL_Surface* in_Surface, int in_X, int in_Y)
{
	///if (SDL_UpdateTexture(fogTexture, NULL, fogSurface->pixels, fogSurface->pitch) != -1) {
		//App->render->Blit(fogTexture, App->render->camera.x, App->render->camera.y);
	//}
}

void FogOfWar::CreateFog(int width, int height)
{
	fogSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_Rect screenRect = { 0, 0, width, height };
	SDL_FillRect(fogSurface, &screenRect, 0xFF202020);
	fogTexture = App->tex->LoadStreamingSurface(fogSurface);
}

void FogOfWar::removeFog(int posX, int posY)
{
	const int halfWidth = fogRemover->w / 2;
	const int halfHeight = fogRemover->h / 2;

	SDL_Rect sourceRect = { 0, 0, fogRemover->w, fogRemover->h };
	SDL_Rect destRect = { posX - halfWidth, posY - halfHeight, fogRemover->w, fogRemover->h };

	// Make sure our rects stays within bounds
	if (destRect.x < 0)
	{
		sourceRect.x -= destRect.x; // remove the pixels outside of the surface
		sourceRect.w -= sourceRect.x; // shrink to the surface, not to offset fog
		destRect.x = 0;
		destRect.w -= sourceRect.x; // shrink the width to stay within bounds
	}
	if (destRect.y < 0)
	{
		sourceRect.y -= destRect.y; // remove the pixels outside
		sourceRect.h -= sourceRect.y; // shrink to the surface, not to offset fog
		destRect.y = 0;
		destRect.h -= sourceRect.y; // shrink the height to stay within bounds
	}

	int xDistanceFromEdge = (destRect.x + destRect.w) - fogSurface->w;
	if (xDistanceFromEdge > 0) // we're busting
	{
		sourceRect.w -= xDistanceFromEdge;
		destRect.w -= xDistanceFromEdge;
	}
	int yDistanceFromEdge = (destRect.y + destRect.h) - fogSurface->h;
	if (yDistanceFromEdge > 0) // we're busting
	{
		sourceRect.h -= yDistanceFromEdge;
		destRect.h -= yDistanceFromEdge;
	}

	SDL_LockSurface(fogSurface);

	Uint32* destPixels = (Uint32*)fogSurface->pixels;
	Uint32* srcPixels = (Uint32*)fogRemover->pixels;

	static bool keepFogRemoved = true;

	for (int x = 0; x < destRect.w; ++x)
	{
		for (int y = 0; y < destRect.h; ++y)
		{
			Uint32* destPixel = destPixels + (y + destRect.y) * fogSurface->w + destRect.x + x;
			Uint32* srcPixel = srcPixels + (y + sourceRect.y) * fogRemover->w + sourceRect.x + x;

			unsigned char* destAlpha = (unsigned char*)destPixel + 3; // fetch alpha channel
			unsigned char* srcAlpha = (unsigned char*)srcPixel + 3; // fetch alpha channel
			if (keepFogRemoved == true && *srcAlpha > 0)
			{
				continue; // skip this pixel
			}

			*destAlpha = *srcAlpha;
		}
	}

	SDL_UnlockSurface(fogSurface);
}

