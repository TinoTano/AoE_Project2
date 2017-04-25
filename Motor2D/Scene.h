#ifndef __SCENE_H__
#define __SCENE_H__


#include "SceneElement.h"
#include "Timer.h"
#include "Gui.h"
#include <string>


#define TOWN_HALL_POS_X -2600
#define TOWN_HALL_POS_Y 2250

#define STARTING_CAMERA_X 3250
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
	uint villagers_curr = 0, villagers_total = 0;
	bool game_finished;
private:
	SDL_Texture* debug_tex;
	bool debug = false;
	Resource* tree;
	bool start = false;
	// TIMER

	// UI ELEMENTS
	Image* top, *bottom;
	Button* menu_bt, *quit_game_bt, *back_to_menu_bt, *save_game_bt, *cancel_bt;
	WindowUI ui_menu;
	Image* menu_bg_img;
	Label* back_to_menu_lbl, *quit_game_lbl, *save_game_lbl, *cancel_lbl;

	vector<SDL_Rect> blit_sections;		 vector<SDL_Rect> detect_sections;
	vector<SDL_Rect> blit_sections_menu; vector<SDL_Rect> detect_sections_menu;

public:
	void UpdateResources(Label* resource, uint new_val);
	void UpdateVillagers(uint available_villagers, uint total_villagers);
private:
	// TIMER

	Timer timer;
	int quadtree_flag = 0;
	Label* Timer_lbl;
	void UpdateTime(float time);
	void TimeEvents();
	Timer orc_timer, troll_timer;
	Timer spawn_timer;
	uint wave = 0, orcs_to_spawn = 0, trolls_to_spawn = 0;
public:
	///////
	Building* my_townCenter;
	Building* enemy_townCenter;
	Unit* hero;
	int woodCount = 0;
	int foodCount = 0;
	int goldCount = 0;
	int stoneCount = 0;
	// RESOURCES 
	Label* wood, *food, *gold, *stone, *villagers;
	Unit* guard1;
	Unit* guard2;
};

#endif // __SCENE_H__