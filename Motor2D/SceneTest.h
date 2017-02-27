#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "Module.h"
#include "Gui.h"
#include <string>

class GuiImage;
class GuiText;

class SceneTest : public Module
{
public:

	SceneTest();

	// Destructor
	virtual ~SceneTest();

	// Called before render is available
	bool Awake();

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
	GuiImage* banner;
	//GuiText* text;


	char mierda, mierda2;
	uint hero_y;
	list<Image*> heroes;
	ScrollBar* scroll, *scroll2;
	Label* val, *val2;

};

#endif // __j1SCENE_H__
