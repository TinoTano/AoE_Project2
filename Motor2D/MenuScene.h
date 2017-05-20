#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__


#include "SceneElement.h"
#include "Timer.h"
#include "Gui.h"
struct SDL_Texture;
class Unit;
class Building;
class Resource;


class MenuScene : public SceneElement
{
public:

	MenuScene();

	// Destructor
	virtual ~MenuScene();

	// Called before render is available
	bool Awake(pugi::xml_node & config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	vector<Info> elements;
	vector<Image*> images;
	vector<Button*> buttons;

	// UI ELEMENTS
	WindowUI ui_menu;
	WindowUI skirmish_menu;

	Label* settings_lbl = nullptr, *mute_lbl = nullptr, *window_lbl = nullptr;
	Label* new_game_lbl = nullptr, *load_game_lbl = nullptr, *map_lbl = nullptr, *freepeople_lbl = nullptr, *sauronarmy_lbl = nullptr;
	Image* ring = nullptr;
	int team = -1;
	int final_team = -1;
	enum IMAGES { BACKGROUND, YELLOW, SETTINGS, BACKGROUND_SKIRMISH, FREEPEOPLE, SAURONARMY };

	enum BUTTONS { SKIRMISH, OPTIONS, QUIT, MUTE, SCREEN, NEWGAME, LOADGAME };

};

#endif // __SCENE_H__#pragma once

