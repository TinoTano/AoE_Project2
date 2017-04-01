#ifndef __SCENE_H__
#define __SCENE_H__


#include "SceneElement.h"
#include "Timer.h"
#include "Gui.h"
#include <string>


struct SDL_Texture;
class Unit;
class Building;
class Resource;

class Scene : public SceneElement
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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

private:
	SDL_Texture* debug_tex;
	bool debug = false;
	Unit* elvenArcher;
	Unit* troll;
	Building* testBuilding;
	Resource* tree;
	Building* build;
// UI ELEMENTS
	Image* top, *bottom;
	Button* menu_bt;
	WindowUI ui_menu;
	Image* menu_bg_img;
	Button* quit_game_bt;
	Button* back_to_menu_bt;
	Button* save_game_bt;
	Button* cancel_bt;
	Label* back_to_menu_lbl;
	Label* quit_game_lbl;
	Label* save_game_lbl;
	Label* cancel_lbl;

};

#endif // __SCENE_H__