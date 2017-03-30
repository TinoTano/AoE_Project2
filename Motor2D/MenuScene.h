#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__


#include "SceneElement.h"
#include "Timer.h"

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

};

#endif // __SCENE_H__#pragma once
