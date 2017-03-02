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

	void DrawFog(SDL_Surface* in_Surface, int in_X, int in_Y);

	void CreateFog(int widht, int height);

	void removeFog(int posX, int posY);

private:
	SDL_Surface* fogSurface;
	SDL_Texture* fogTexture;
	SDL_Surface* fogRemover;
};

#endif // !__FOG_OF_WAR__