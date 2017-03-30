#ifndef __SCENE_MANAGER__H
#define __SCENE_MANAGER__H

#include "Module.h"
#include "Scene.h"
#include "SceneTest.h"

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

	void ChangeScene(SceneElement* new_scene);

	Scene*		level1_scene = nullptr;
	SceneTest*		UI_scene = nullptr;


	list<Scene*>	scenes;
	SceneElement*	current_scene = nullptr;
};

#endif // !__SCENE_MANAGER__
