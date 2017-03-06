#ifndef __FOG_OF_WAR__
#define __FOG_OF_WAR__

#include "Module.h"

class FogOfWar : public Module
{
public:
	FogOfWar();
	virtual ~FogOfWar();

	bool Start();

	bool Update(float dt);

	void DrawFog();

	void CreateFog(int widht, int height);

	void removeFog(int posX, int posY);

private:
	SDL_Surface* highFogSurface;
	SDL_Surface* removerSurface;
	SDL_Surface* lowFogSurface;
	SDL_Texture* highFogTexture;
	SDL_Texture* fogRemoverTexture;
	SDL_Texture* lowFogTexture;
	void* highFogPixels;
	int highFogPitch;
	void* removerPixels;
	int removerPitch;
	void* lowFogPixels;
	int lowFogPitch;
	uint fogTextWidth;
	uint fogTextHeight;
	uint removerTextWidth;
	uint removerTextHeight;
	//SDL_Rect lastPixelsRect = { 0,0,0,0 };
};

#endif // !__FOG_OF_WAR__