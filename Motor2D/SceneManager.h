#ifndef __SCENE_MANAGER__H
#define __SCENE_MANAGER__H

#include "Module.h"


class SceneManager;

class SceneManager : public Module {
public:
	SceneManager();

	// Destructor
	virtual ~SceneManager();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();



	list<Module*>	scenes;
};

#endif // !__SCENE_MANAGER__
