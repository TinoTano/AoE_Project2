#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include "Module.h"
#include <list>
#include <string>
#include <vector>
#include "EntityManager.h"

using namespace std;

#define CURSOR_WIDTH 2
#define MAX_CHAR 15
#define DEBUG_COLOR {200, 0, 100, 100}
#define BAR_COLOR {200,0,100,100}
#define THUMB_COLOR {0,50,200, 200}
#define BLIT_SQUARE_COLOR {200,0,100,100}
#define LABEL_COLOR {0,0,0 250}
#define CAMERA_OFFSET_X App->render->camera.x
#define CAMERA_OFFSET_Y App->render->camera.y


struct _TTF_Font;


enum ElementType {
	IMAGE, LABEL, BUTTON, INPUTTEXT, SCROLLBAR, QUAD, CURSOR, UNKNOWN
};

enum MouseState {
	FREE, HOVER, CLICKIN, CLICKOUT
};

enum ButtonTier {
	TIER1 = 1,
	// Has 3 SDL_Rects section: one for 'standard' state, one for 'on hover' state, and one for 'on click' state.
	TIER2 = 2,
	// Has 2 SDL_Rects section: one for 'standard' state and 'on click'.
};

enum ScrollBarModel {
	MODEL1,
	//Which we use for settings
	MODEL2
	//WHich we use for command window
};

class UnitSprite;
class Info;
class UIElement {
public:
	UIElement(bool argenabled, int argx, int argy, ElementType argtype, SDL_Texture* argtexture);
	virtual void	Update() {}
	virtual	void	Draw() {}
	virtual void	DebugMode() {}
	virtual void	Movement(pair<int, int>) {}
	virtual void	CleanUp() {}
	void			SetPos(int x, int y);
	void			SetParentPos(int x, int y);
	void			Move(int x, int y);
	bool			enabled, focused = true, debug = false;

	UIElement* parent = nullptr;
	pair<int, int> pos, parent_pos;
	ElementType type;
	MouseState current;

	SDL_Texture* texture = nullptr;
	SDL_Color debug_color;

	uint priority = 0;
};

class Image : public UIElement {
public:
	Image(SDL_Rect argsection, int x, int y, SDL_Texture* argtexture);
	Image(int x, int y, SDL_Texture* argtexture);
	MouseState MouseDetect();
	void Movement(pair<int, int> movement);


	void Update();
	void Draw();
	void DebugMode();
	void CleanUp();


	SDL_Rect section;
};


class Label : public UIElement {
public:
	Label(char * text, int x, int y, _TTF_Font* font);
	Label(string text, int x, int y, _TTF_Font* font);
	Label(char * text, SDL_Rect area, _TTF_Font* font);
	void Movement(pair<int, int> movement);

	void Update();
	void Draw();
	void SetText(char* text);
	void SetString(string text);
	void SetSize(int size);
	void SetColor(SDL_Color color);
	void CleanUp();
	string str;

private:
	int width = 0, height = 0, size = 0;
	_TTF_Font* font;


	SDL_Color color;
	SDL_Rect area;
};


class Button : public UIElement {
public:
	Button(int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier tier, SDL_Texture* texture);

	void Update();
	void CleanUp();

	void Draw(SDL_Rect section);
	void Movement(pair<int, int> movement);
	MouseState MouseDetect();
	void DebugMode();


	vector<SDL_Rect> blit_sections;
	vector<SDL_Rect> detect_sections;

	ButtonTier button_tier;
};

class InputText : public UIElement {
public:
	InputText(int x, int y, SDL_Rect detect_area, _TTF_Font* font);
	void DebugMode();
	void Update();
	void Draw();
	void CleanUp();
	MouseState MouseDetect();


	int width = 0, height = 0;
	_TTF_Font* font;

	string		str;
	string		cpy_str;
	SDL_Rect	area;

	bool editable;

private:
	uint		bar_pos = 0;
	uint		bar_pos_x = 0;
	SDL_Rect	bar;
	list<int>	words_lenght;

	void MoveCursor();
	void UpdateWordsLength();
	void DrawBar();
	void SetBarToEnd();
};

class Quad;

struct BarElements {
	SDL_Rect		BigSquare;
	Quad			*SmallSquare;
};

class ScrollBar : public UIElement {
public:
	ScrollBar(int x, int y, ScrollBarModel model);
	void Update();
	int GetData();
	// BAR 1
	int data = 0, max_y = 0, min_y = 0;
	Image* Bar = nullptr;
	Image* Thumb = nullptr;
	Button* Up = nullptr;
	Button* Down = nullptr;
	// BAR 2
	void			UpdateThumbSize(int h);
	void			SetBar(SDL_Rect BlitSquare, SDL_Rect thumb); // TO START THE BAR
	void			DebugMode();
	MouseState		MouseDetect();

	Quad* BlitSquare = nullptr;
	Quad* BarRect = nullptr;
	BarElements ABar;

	ScrollBarModel model;

};

class Quad : public UIElement {
public:
	Quad(SDL_Rect area, SDL_Color color);
	void CleanUp();
	void Update();
	void Draw();
	void SetArea(SDL_Rect area);
	void Movement(pair<int, int> movement);

	SDL_Rect	area;
	SDL_Color	color;
};

class Cursor : public UIElement {
public:
	Cursor(SDL_Texture* tex, vector<SDL_Rect> area);
	void Update();
	void Draw();
	void SetCursor(int id);
	void CleanUp();

private:
	int id = 0;
	pair<int, int> cursor_pos;
	pair<int, int> blitoffset;
	vector<SDL_Rect> sprite_list;

};

class WindowUI {
private:
	bool enabled = true;
	SDL_Rect size;
	int* x, *y, width, height;
public:
	void SetFocus(int& x, int &y, int width, int height);
	list<UIElement*> in_window;

	bool IsEnabled();
	void WindowOn();
	void WindowOff();
	SDL_Rect FocusArea();
	void CleanUp();

};
enum EntityType {
	UNIT, BUILDING, RESOURCE
};

class HUD {
public:

	enum HUDType {
		MULTIPLESELECTION, SINGLEINFO, BUILDINGINFO, RESOURCEINFO, NONE
	};

	HUDType		type = NONE;
	HUD();

	// MULTIPLESELECTION
private:
	list<Image*> multiple;
	uint x, y;
	int max_width = 700;;

	//SINGLEINFO
private:
	Unit* selected_unit;

	Image* single;			Label* name;
	Label* armor_val;		Label* damage_val;
	Label* life;

	Image* sword_img;		Image* armor_img;

	char armor[65], damage[65], currlife[65], maxlife[65];
	uint posx, posy;
	//BUILDINGINFO
	Building* selected_building;
	uint attack, defense, max_life, curr_life;

	// RESOURCEINFO
	Resource* selected_resource;
	uint gathering;

	// BUTTONS POSITIONS
	vector<SDL_Rect> buttons_positions;
public:

	void GetSelection();
	void StartBuildingInfo();
	void StartResourceInfo();
	void Start();
	void Update();

	void ClearMultiple();
	void ClearSingle();
	void ClearBuilding();
	void CleanUp();
private:
	//
	// HUD MANAGING FUNCTIONS
	// ----------------------
	//        RESOURCE
	//-----------------------
	void HUDResourceMenu();
	// -----------------------
	//        BUILDING
	// -----------------------

	enum HUDBuildingState {
		BUILDINGMENU,
		BUILDINGCREATEUNITS
	};
	HUDBuildingState building_state;

	// ----- MENU -----
	Button* create_unit_bt, *create_villager_bt;
	void HUDBuildingMenu();
	void HUDClearBuildingMenu();

	// ----- CREATE UNITS ------
	Button* create_elven_archer_bt, *create_elven_longblade_bt, *create_elven_cavalry_bt, *cancel_bt;
	void HUDCreateUnits();
	void HUDClearCreateUnits();

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

	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load and save before
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	bool	LoadHUDData();

	UIElement* CreateButton(char* path, int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier Tier);
	UIElement* CreateButton(SDL_Texture* texture, int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier Tier);
	// Blit_Sections contains de rects from the image. Tier 1 must have 3 and Tier 2 must have 2;
	UIElement* CreateImage(char* path, int x, int y, SDL_Rect section);
	UIElement* CreateImage(char* path, int x, int y);
	UIElement* CreateImage(SDL_Texture* argtexture, int x, int y, SDL_Rect section);
	UIElement* CreateLabel(char* text, int x, int y, _TTF_Font* font);
	UIElement* CreateLabel(string text, int x, int y, _TTF_Font* font);
	UIElement* CreateLabel(char* text, SDL_Rect area, _TTF_Font* font);
	UIElement* CreateInput(int x, int y, SDL_Rect detect_section, _TTF_Font* font);
	UIElement* CreateScrollBar(int x, int y, ScrollBarModel model);
	UIElement* CreateQuad(SDL_Rect size, SDL_Color color);
	UIElement* CreateCursor(char* path, vector<SDL_Rect> cursor_list);
	void DestroyUIElement(UIElement* element);
	void DestroyALLUIElements();

	void	ScreenMoves(pair<int, int> movement);
	void	SetPriority();
	void	Focus(SDL_Rect rect);
	void	Unfocus();

private:
	list<UIElement*> Elements;
	vector<Info> info;
public:
	vector<Info>GetElements(string scene);
	// ----- UNIT CLASS ----- //
	// -------------------- //
public:
	Cursor* cursor = nullptr;
	list<UnitSprite> SpriteUnits;
	list<UnitSprite> SpriteBuildings;
	list<UnitSprite> SpriteResources;
	HUD* hud;
};


class UnitSprite {
	SDL_Rect rectangle;
	uint ID;
	EntityType type;
	string name;
public:
	UnitSprite(EntityType argtype, SDL_Rect argrectangle, uint argID, string argname) : type(argtype), rectangle(argrectangle), ID(argID), name(argname) {
	}
	SDL_Rect GetRect() {
		return rectangle;
	}
	uint GetID() {
		return ID;
	}
	EntityType GetType() {
		return type;
	}
	string GetName() {
		return name;
	}
};

class Info {
public:
	string name;
	uint id;
	pair<int, int> position;
	string path;
	SDL_Rect rect;

	string scene;
	SDL_Texture* texture;
	ElementType type;

	vector<SDL_Rect> blit_sections;
	vector<SDL_Rect> detect_sections;
	ButtonTier tier;
	Info::Info(string argname, uint argid, pair<int, int> argpos, string argpath, SDL_Rect argrect, string argscene, ElementType argtype) :
		name(argname), id(argid), position(argpos), path(argpath), rect(argrect), scene(argscene), type(argtype) {}
};

#endif // __j1GUI_H__