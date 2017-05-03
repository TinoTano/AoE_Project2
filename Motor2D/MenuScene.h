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
	uint fx_button_click;

	// UI ELEMENTS
	WindowUI ui_menu;
	Label* settings_lbl, *mute_lbl, *window_lbl;

	enum IMAGES { BACKGROUND, YELLOW, SETTINGS };

	enum BUTTONS { SKIRMISH, OPTIONS, QUIT, MUTE, SCREEN };

};

#endif // __SCENE_H__#pragma once

