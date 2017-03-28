#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "Module.h"
#include "SceneManager.h"
#include "Gui.h"
#include <string>

class GuiImage;
class GuiText;

class SceneTest : public SceneManager
{
public:

	SceneTest();

	// Destructor
	virtual ~SceneTest();

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
	GuiImage* banner;
	//GuiText* text;


	char mierda;
	uint hero_y;
	list<Image*> heroes;
	Image* top, * bottom;
	Button* menu_bt;


	WindowUI ui_menu;
	Image* menu_bg_img;
	Button* quit_game_bt;
	Button* back_to_menu_bt;
	Button* save_game_bt;
	Button* cancel_bt;
};

#endif // __j1SCENE_H__
