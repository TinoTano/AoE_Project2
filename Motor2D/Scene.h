#ifndef __SCENE_H__
#define __SCENE_H__


#include "SceneElement.h"
#include "Timer.h"
#include "Gui.h"
#include "QuestHUD.h"
#include <string>


#define TOWN_HALL_POS_X -1000
#define TOWN_HALL_POS_Y 2250

#define STARTING_CAMERA_X 1650
#define STARTING_CAMERA_Y -1900

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

	uint villagers_curr = 0, villagers_max = 0;
	bool game_finished;

private:
	bool start = false;
	// TIMER

	// UI ELEMENTS
	WindowUI ui_menu, surrender_menu;

	vector<Info> elements;
	vector<Image*> images;
	vector<Button*> buttons;

public:
	void UpdateResources();
	void UpdatePopulation();
	bool CheckUnitsRoom();
private:
	// TIMER
	Timer timer;
	Label* Timer_lbl = nullptr;
	void UpdateTime(float time);
public:
	QuestHUD questHUD;
	// RESOURCES 
	Label* wood = nullptr, *food = nullptr, *gold = nullptr, *stone = nullptr, *villagers = nullptr;

	enum BUTTONS { MENU = 0, BACKTOMENU, QUITGAME, SAVEGAME, CANCEL, LOADGAME, SURRENDER, YES, NO };
	enum IMAGES { TOP = 0, BOTTOM, MINIMAP, WINDOW };

};

#endif // __SCENE_H__