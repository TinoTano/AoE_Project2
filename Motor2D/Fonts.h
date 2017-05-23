#ifndef __FONTS_H__
#define __FONTS_H__

#include "Module.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "fonts/open_sans/timesbd.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class Fonts : public Module
{
public:

	Fonts();

	// Destructor
	virtual ~Fonts();
	

	bool Start();
	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = {255, 255, 255, 255}, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;
	bool DeleteFont(_TTF_Font* font = NULL);

public:

	vector<_TTF_Font*>	fonts;

	_TTF_Font*			default = nullptr;

private:
	const char* default_path;
};

enum fontslist { EIGHT, SIXTEEN, TWENTY, FOURTEEN, EIGHTEEN, TWENTYSIX };


#endif // __FONTS_H__