#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include "Module.h"
#include <list>
#include <string>
#include <vector>
#include "TechTree.h"
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
#define NUM_UI_BUTTONS 12
#define TECH_UI_BUTTON 6
#define TEXTALERTPOS_X x/3 + x/15
#define TEXTALERTPOS_Y y/5

struct _TTF_Font;
struct unit_button;
struct building_button;
struct tech_button;

enum ElementType {
	IMAGE, LABEL, BUTTON, INPUTTEXT, SCROLLBAR, QUAD, CURSOR, UNKNOWN
};

enum MouseState {
	FREE, HOVER, CLICKIN, CLICKOUT, CLICKUP
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
	bool			enabled, focused = true, debug = false, loaded_tex = false;

	uint ID = 0;
	UIElement* parent = nullptr;
	pair<int, int> pos, parent_pos;
	ElementType type = UNKNOWN;
	MouseState current = FREE;

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
	_TTF_Font* font = nullptr;


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
	string name;
	string desc;

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
	_TTF_Font* font = nullptr;

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

class TextAlert {
public:
	Timer text_duration, text_flicker;
	Label* textalert_lbl = nullptr;
	uint duration;
	int color = 0;
	void Start(string text, uint duration, uint x, uint y);
	void Update();
	void CleanUp();
	bool active = false;
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
	Unit* selected_unit = nullptr;

	Image* single = nullptr;			Label* name = nullptr;
	Label* armor_val = nullptr;		Label* damage_val = nullptr;
	Label* life = nullptr;

	Image* sword_img = nullptr;		Image* armor_img = nullptr;
	uint id;
	char armor[65], damage[65], currlife[65], maxlife[65];
	uint posx, posy;
	//BUILDINGINFO
	Building* selected_building = nullptr;
	uint attack, defense, max_life, curr_life;

	// RESOURCEINFO
	Resource* selected_resource = nullptr;
	uint gathering;

	// BUTTONS POSITIONS
	vector<SDL_Rect> buttons_positions;
public:
	void Start();
	void Update();
	void CleanUp();
	void ClearAll();
private:
	void GetSelection();
	void StartBuildingInfo();
	void StartResourceInfo();
	SDL_Rect GetRect(uint id);
	void ClearMultiple();
	void ClearSingle();
	void ClearBuilding();
	void ClearResource();
	vector<Button*> all_bt;

private:
	//
	// HUD MANAGING FUNCTIONS
	// ----------------------
	//        RESOURCE
	//-----------------------
	void HUDResourceMenu();
	void DrawResourceBar();
	// -----------------------
	//		  VILLAGERS
	// -----------------------
	enum HUDVillagerState {
		VILLAGERNULL,
		VILLAGERMENU,
		VILLAGERCREATEBUILDINGS
	};
	HUDVillagerState villager_state;
	vector<vector<SDL_Rect> > buildings_rects;
	// ------- MENU ----------
	Button* create_building_bt = nullptr;
	void HUDVillagerMenu();
	void HUDClearVillagerMenu();
	// -----------------------
	//		  HEROES
	// -----------------------
	enum HUDHeroState {
		HERONULL,
		HEROMENU
	};
	HUDHeroState hero_state;
	// ------- MENU ----------
	Button* skill_bt = nullptr;
	void HUDHeroMenu();
	void HUDClearHeroMenu();
	// ------ CREATE BUILDINGS -------
	void HUDCreateBuildings();
	void HUDClearCreateBuildings();
	// -----------------------
	//        BUILDING
	// -----------------------

	enum HUDBuildingState { BUILDINGNULL, BUILDINGMENU, BUILDINGCREATEUNITS, BUILDINGCREATEHERO };
	TechType tech_studied;
	bool studying_tech = false;
	HUDBuildingState building_state;
	vector<vector<SDL_Rect> > units_rects;
	void DrawBuildingBar();
	// ----- MENU -----
	Button* create_unit_bt = nullptr, *create_hero_bt = nullptr, *create_villager_bt = nullptr;
	void HUDBuildingMenu();
	void HUDClearBuildingMenu();

	// ----- CREATE HERO -------
	void HUDCreateHero();
	void HUDClearCreateHero();

	// ----- CREATE UNITS ------
	void HUDCreateUnits();
	void HUDClearCreateUnits();
	// GENERAL HELPINGS
	Button* cancel_bt = nullptr;
	vector<SDL_Rect> blit_sections;

	// BLITING INFO
	void BlitInfoUnit(unit_button bt);
	void BlitInfoBuilding(building_button bt);
	void BlitInfoTech(tech_button bt);
	Label* info_lbl = nullptr, *desc_lbl = nullptr, *cost_lbl = nullptr;

public:
	// ------- OTHER USEFUL UTENSILS ------------
	TextAlert alert;
	void AlertText(string text, uint duration);

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
	bool SaveToScreen(pugi::xml_node&) const;
	bool LoadToScreen(pugi::xml_node&);

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
	uint elements_counter = 0;
	list<UIElement*> Elements;
	vector<Info> info;
public:
	vector<Info>GetElements(string scene);
	vector<building_button> building_bt;
	vector<unit_button> unit_bt;
	vector<tech_button> tech_bt;
	vector<tech_button> sauron_tech_bt;

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
	SDL_Texture* texture = nullptr;
	ElementType type;

	vector<SDL_Rect> blit_sections;
	vector<SDL_Rect> detect_sections;
	ButtonTier tier;
	Info::Info(string argname, uint argid, pair<int, int> argpos, string argpath, SDL_Rect argrect, string argscene, ElementType argtype) :
		name(argname), id(argid), position(argpos), path(argpath), rect(argrect), scene(argscene), type(argtype) {}
};


struct building_button {
	Button* button = nullptr;
	string name;
	string desc;
	string cost;
	vector<SDL_Rect> blit_sections;
	buildingType type;
};

struct unit_button {
	Button* button = nullptr;
	string name;
	string desc;
	string cost;
	vector<SDL_Rect> blit_sections;
	unitType type;
};

struct tech_button {
	Button* button = nullptr;
	string name;
	string desc;
	string cost;
	vector<SDL_Rect> blit_sections;
	TechType type;
};
#endif // __j1GUI_H__