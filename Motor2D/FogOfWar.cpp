#include "FogOfWar.h"
#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "p2Log.h"
#include "Textures.h"
#include "p2Defs.h"
#include "Map.h"
#include "Input.h"

FogOfWar::FogOfWar()
{
	
}


FogOfWar::~FogOfWar()
{
}

bool FogOfWar::Start()
{
	showHighFog = true;
	showLowFog = true;
	return true;
}

bool FogOfWar::Update(float dt)
{
	DrawFog();
	return true;
}

void FogOfWar::DrawFog()
{
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		showLowFog = !showLowFog;
	}
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		showHighFog = !showHighFog;
	}

	if (showLowFog) {
		App->render->Blit(lowFogTexture, -App->map->data.mapWidth / 2, 0);
	}
	
	if (showHighFog) {
		App->render->Blit(highFogTexture, -App->map->data.mapWidth / 2, 0);
	}
	
}

void FogOfWar::CreateFog(int width, int height)
{
	//High fog creation
	highFogSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_FillRect(highFogSurface, NULL, 0xFF000000);
	highFogTexture = App->tex->LoadStreamingTextureFromSurface(highFogSurface);
	SDL_SetTextureBlendMode(highFogTexture, SDL_BLENDMODE_BLEND);

	//Remove fog zone creation
	removerSurface = App->tex->LoadImageAsSurface("textures/fogCercle.png");
	fogRemoverTexture = App->tex->LoadStreamingTextureFromSurface(removerSurface);

	//Low fog creation
	lowFogSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_FillRect(lowFogSurface, NULL, 0xFF000000);
	lowFogTexture = App->tex->LoadStreamingTextureFromSurface(lowFogSurface);
	SDL_SetTextureBlendMode(lowFogTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(lowFogTexture, 125);

	//------- Get 
	SDL_LockTexture(highFogTexture, NULL, &highFogPixels, &highFogPitch);

	//Copy loaded/formatted surface pixels
	memcpy(highFogPixels, highFogSurface->pixels, highFogSurface->pitch * highFogSurface->h);

	//Unlock texture to update
	SDL_UnlockTexture(highFogTexture);
	highFogPixels = NULL;

	//Get image dimensions
	App->tex->GetSize(highFogTexture, fogTextWidth, fogTextHeight);


	//////////////////////////////
	SDL_LockTexture(fogRemoverTexture, NULL, &removerPixels, &removerPitch);

	//Copy loaded/formatted surface pixels
	memcpy(removerPixels, removerSurface->pixels, removerSurface->pitch * removerSurface->h);

	//Unlock texture to update
	SDL_UnlockTexture(fogRemoverTexture);
	removerPixels = NULL;

	//Get image dimensions
	App->tex->GetSize(fogRemoverTexture, removerTextWidth, removerTextHeight);


	//////////////////////////////
	SDL_LockTexture(lowFogTexture, NULL, &lowFogPixels, &lowFogPitch);

	//Copy loaded/formatted surface pixels
	memcpy(lowFogPixels, lowFogSurface->pixels, lowFogSurface->pitch * lowFogSurface->h);

	//Unlock texture to update
	SDL_UnlockTexture(lowFogTexture);
	lowFogPixels = NULL;

}

void FogOfWar::removeFog(int posX, int posY)
{
	const int halfWidth = removerTextWidth / 2;
	const int halfHeight = removerTextHeight / 2;

	posX += App->map->data.mapWidth / 2;

	SDL_Rect sourceRect = { 0, 0,removerTextWidth, removerTextHeight };
	SDL_Rect destRect = { posX - halfWidth, posY - halfHeight, removerTextWidth, removerTextHeight };

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

	int xDistanceFromEdge = (destRect.x + destRect.w) - highFogSurface->w;
	if (xDistanceFromEdge > 0) // we're busting
	{
		sourceRect.w -= xDistanceFromEdge;
		destRect.w -= xDistanceFromEdge;
	}
	int yDistanceFromEdge = (destRect.y + destRect.h) - highFogSurface->h;
	if (yDistanceFromEdge > 0) // we're busting
	{
		sourceRect.h -= yDistanceFromEdge;
		destRect.h -= yDistanceFromEdge;
	}

	SDL_LockTexture(highFogTexture,NULL,&highFogPixels,&highFogPitch);

	SDL_LockTexture(fogRemoverTexture,NULL,&removerPixels,&removerPitch);

	SDL_LockTexture(lowFogTexture, NULL, &lowFogPixels, &lowFogPitch);

	static bool keepFogRemoved = true;

	for (int x = 0; x < destRect.w; ++x)
	{
		for (int y = 0; y < destRect.h; ++y)
		{
			Uint32* highFogPixel = (Uint32*)highFogPixels + (y + destRect.y) * fogTextWidth + destRect.x + x;
			Uint32* lowFogPixel = (Uint32*)lowFogPixels + (y + destRect.y) * fogTextWidth + destRect.x + x;
			Uint32* removerPixel = (Uint32*)removerPixels + (y + sourceRect.y) * removerTextWidth + sourceRect.x + x;

			unsigned char* highFogPixelAlpha = (unsigned char*)highFogPixel + 3; // alpha channel
			unsigned char* lowFogPixelAlpha = (unsigned char*)lowFogPixel + 3; // alpha channel
			unsigned char* removerPixelAlpha = (unsigned char*)removerPixel + 3;

			if (keepFogRemoved == true && *removerPixelAlpha > 0)
			{
				*lowFogPixelAlpha = *removerPixelAlpha;
				continue; // skip highFog pixel
			}
			*highFogPixelAlpha = *lowFogPixelAlpha = *removerPixelAlpha;
		}
	}

	SDL_UnlockTexture(fogRemoverTexture);
	SDL_UnlockTexture(lowFogTexture);
	SDL_UnlockTexture(highFogTexture);

}

