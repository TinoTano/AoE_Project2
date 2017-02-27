#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include "Module.h"
#include <list>
#include <string>
#include <vector>

using namespace std;

#define CURSOR_WIDTH 2
#define MAX_CHAR 15
#define DEBUG_COLOR {200, 0, 100, 100}

struct _TTF_Font;


enum ElementType {
	IMAGE,
	LABEL,
	BUTTON,
	INPUTTEXT,
	SCROLLBAR,
	QUAD,
	UNKNOWN
};

enum MouseState {
	FREE,
	HOVER,
	CLICKIN,
	CLICKOUT
};
enum ButtonTier {
	TIER1,
	// Has 3 SDL_Rects section: one for 'standard' state, one for 'on hover' state, and one for 'on click' state.
	TIER2,
	// Has 2 SDL_Rects section: one for 'standard' state and 'on click'.
};

enum ScrollBarModel {
	MODEL1
	//Which we use for settings
};


class UIElement {
public:
	UIElement(bool argenabled, int argx, int argy, ElementType argtype, SDL_Texture* argtexture);
	virtual void	Update() {}
	virtual	void	Draw() {}
	virtual void	DebugMode() {}
	void			SetPos(int x, int y);
	void			SetParentPos(int x, int y);
	void			Move(int x, int y);
	bool enabled, debug = false;
	UIElement* parent;
	pair<int, int> pos, parent_pos;
	ElementType type;
	MouseState current;

	SDL_Texture* texture;
	SDL_Color debug_color;
};

class Image : public UIElement {
public:
	Image(SDL_Rect argsection, int x, int y, SDL_Texture* argtexture);
	Image(int x, int y, SDL_Texture* argtexture);
	MouseState MouseDetect();


	void Update();
	void Draw();
	void DebugMode();


	SDL_Rect section;
};


class Label : public UIElement {
public:
	Label(char * text, int x, int y, _TTF_Font* font);
	Label(char * text, SDL_Rect area, _TTF_Font* font);

	void Update();
	void Draw();
	void SetText(char* text);

	int width, height, size;
	_TTF_Font* font;
	string str;

	SDL_Color color;
	SDL_Rect area;
};


class Button : public UIElement {
public:
	Button(int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier tier, SDL_Texture* texture);

	void Update();
	void Draw(SDL_Rect section);
	MouseState MouseDetect();
	void DebugMode();


	vector<SDL_Rect> blit_sections;
	vector<SDL_Rect> detect_sections;

	ButtonTier button_tier;
};

class InputText : public UIElement {
public:
	InputText(int x, int y, SDL_Rect detect_area, _TTF_Font* font);

	void Update();
	void Draw();
	MouseState MouseDetect();


	int width, height;
	_TTF_Font* font;

	string		str;
	string		cpy_str;
	SDL_Rect	area;

	bool editable;

private:
	uint		bar_pos;
	uint		bar_pos_x;
	SDL_Rect	bar;
	list<int>	words_lenght;

	void MoveCursor();
	void UpdateWordsLength();
	void DrawBar();
	void SetBarToEnd();
};

class ScrollBar : public UIElement {
public:
	ScrollBar(int x, int y, ScrollBarModel model);
	void Update();
	int GetData();

	int data, max_y, min_y;
	Image* Bar;
	Image* Thumb;
	Button* Up;
	Button* Down;

	ScrollBarModel model;

};

class Quad : public UIElement {
public:
	Quad(SDL_Rect area, SDL_Color color);
	void Update();
	void Draw();
	void SetArea(SDL_Rect area);

	SDL_Rect	area;
	SDL_Color	color;
};
// ---------------------------------------------------

class Gui : public Module
{
public:
	Gui();

	// Destructor
	virtual ~Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load and save before
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	UIElement* CreateButton(char* path, int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier Tier);
	// Blit_Sections contains de rects from the image. Tier 1 must have 3 and Tier 2 must have 2;
	UIElement* CreateImage(char* path, int x, int y, SDL_Rect section);
	UIElement* CreateImage(char* path, int x, int y);
	UIElement* CreateLabel(char* text, int x, int y, _TTF_Font* font);
	UIElement* CreateLabel(char* text, SDL_Rect area, _TTF_Font* font);
	UIElement* CreateInput(int x, int y, SDL_Rect detect_section, _TTF_Font* font);
	UIElement* CreateScrollBar(int x, int y, ScrollBarModel model);
	UIElement* CreateQuad(SDL_Rect size, SDL_Color color);
	SDL_Texture* GetAtlas() const;

	list<UIElement*> Elements;

private:
	SDL_Texture* atlas;
	string atlas_file_name;
};



#endif // __j1GUI_H__
