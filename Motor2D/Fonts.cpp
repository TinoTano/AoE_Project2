#include "Application.h"
#include "Textures.h"
#include "FileSystem.h"
#include "Fonts.h"
#include "p2Log.h"

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

Fonts::Fonts() : Module()
{
	name = "fonts";
}

// Destructor
Fonts::~Fonts()
{}

// Called before render is available
bool Fonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	bool ret = true;

	if (TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		ret = false;
	}
	else
	{
		default_path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		default = Load(default_path, size);
	}

	return ret;
}

bool Fonts::Start()
{
	Load(nullptr, 16);
	Load(nullptr, 20);
	Load(nullptr, 14);
	Load(nullptr, 18);
	Load(nullptr, 26);
	Load(nullptr, 80);
	return true;
}

// Called before quitting
bool Fonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");

	for (vector<TTF_Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it)
	{
		TTF_CloseFont((*it));
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const Fonts::Load(const char* path, int size)
{
	TTF_Font* font;
	if (path != nullptr)
		font = TTF_OpenFontRW(App->fs->Load(path), 1, size);
	else {
		font = TTF_OpenFontRW(App->fs->Load(default_path), 1, size);
	}
	if (font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		LOG("Successfully loaded font %s size %d", path, size);
		fonts.push_back(font);
	}

	return font;
}

// Print text using font
SDL_Texture* Fonts::Print(const char* text, SDL_Color color, _TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default, text, color);

	if (surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		ret = App->tex->LoadStaticSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if (TTF_SizeText((font) ? font : default, text, &width, &height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	else
		ret = true;

	return ret;
}

bool Fonts::DeleteFont(_TTF_Font * font)
{
	bool ret = true;
	if (font != nullptr) {
		if (font != default) {
			for (vector<_TTF_Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it) {

				if ((*it) == font)
				{
					TTF_CloseFont(font);
					fonts.erase(it);
				}
			}
		}
		else {
			LOG("Cannot delete default font :S");
			ret = false;
		}
	}
	else {
		LOG("Cannot delete font.");
		ret = false;
	}
	return ret;
}

