#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "SceneManager.h"
#include "Timer.h"

struct SDL_Texture;
class Unit;
class Building;
class Resource;

class Scene : public SceneManager
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
};

#endif // __SCENE_H__