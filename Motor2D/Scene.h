#ifndef __SCENE_H__
#define __SCENE_H__


#include "SceneElement.h"
#include "Timer.h"
#include "Gui.h"
#include <string>


#define TOWN_HALL_POS_X -1000
#define TOWN_HALL_POS_Y 2250

#define STARTING_CAMERA_X 1650
#define STARTING_CAMERA_Y -1900

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

	void LoadScene();
	void SaveScene();
	bool SpendResourcesIn(Cost& cost);

	uint villagers_curr = 0, villagers_total = 0;
	bool game_finished;
private:
	SDL_Texture* debug_tex = nullptr;
	bool debug = false;
	Resource* tree = nullptr;
	bool start = false;
	// TIMER

	// UI ELEMENTS
	WindowUI ui_menu;
	Label* back_to_menu_lbl = nullptr, *quit_game_lbl = nullptr, *save_game_lbl = nullptr, *cancel_lbl = nullptr;

	vector<Info> elements;
	vector<Image*> images;
	vector<Button*> buttons;

public:
	void UpdateResources(Label* resource, uint new_val);
	void UpdateVillagers(uint available_villagers, uint total_villagers);
private:
	// TIMER

	Timer timer;
	int quadtree_flag = 0;
	Label* Timer_lbl = nullptr;
	void UpdateTime(float time);
	void TimeEvents();
	Timer orc_timer, troll_timer;
	Timer spawn_timer;
	uint wave = 0, orcs_to_spawn = 0, trolls_to_spawn = 0;
public:
	///////
	Building* my_townCenter = nullptr;
	Building* enemy_townCenter = nullptr;
	Unit* hero = nullptr;
	int woodCount = 0;
	int foodCount = 0;
	int goldCount = 0;
	int stoneCount = 0;
	// RESOURCES 
	Label* wood = nullptr, *food = nullptr, *gold = nullptr, *stone = nullptr, *villagers = nullptr;
	Unit* guard1= nullptr;
	Unit* guard2= nullptr;

	enum BUTTONS { MENU = 0, BACKTOMENU, QUITGAME, SAVEGAME, CANCEL };
	enum IMAGES { TOP = 0, BOTTOM, MINIMAP, WINDOW };

};

#endif // __SCENE_H__