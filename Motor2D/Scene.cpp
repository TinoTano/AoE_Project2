#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "Scene.h"
#include "p2Log.h"
#include "EntityManager.h"
#include "Unit.h"
#include "FogOfWar.h"
#include "Gui.h"
#include "SceneManager.h"

Scene::Scene() : SceneElement("scene")
{
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node & config)
{
	LOG("Loading Scene");
	bool ret = true;
	active = true;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	active = true;
	if (App->map->Load("rivendell.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	// LOADING UI BB
	// ----------------------------------------
	// LOADING BUTTONS RECTS
	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 0, 0, 220, 30 });
	blit_sections.push_back({ 0, 30, 220, 30 });

	vector<SDL_Rect> detect_sections;
	detect_sections.push_back({ 0, 0, 220, 30 });


	vector<SDL_Rect> blit_sections_menu;
	blit_sections_menu.push_back({ 0, 0, 50, 19 });
	blit_sections_menu.push_back({ 50, 0, 50, 19 });

	vector<SDL_Rect> detect_sections_menu;
	detect_sections_menu.push_back({ 1330 , 5, 50, 19 });

	// LOADING SCENE UI

	top = (Image*)App->gui->CreateImage("gui/ingame_layer.png", 0, 0, { 0,0,1920, 30 });
	bottom = (Image*)App->gui->CreateImage("gui/ingame_layer.png", 0, 622, { 0, 40, 1408, 172 });

	menu_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", 1330, 5, blit_sections_menu, detect_sections_menu, TIER2);



	// MENU WINDOW

	menu_bg_img = (Image*)App->gui->CreateImage("gui/tech-tree-parchment-drop-down-background.png", 540, 200, { 0,0, 280, 280 });
	back_to_menu_bt = (Button*)App->gui->CreateButton("gui/button.png", 570, 210, blit_sections, detect_sections, TIER2);
	quit_game_bt = (Button*)App->gui->CreateButton("gui/button.png", 570, 250, blit_sections, detect_sections, TIER2);
	save_game_bt = (Button*)App->gui->CreateButton("gui/button.png", 570, 290, blit_sections, detect_sections, TIER2);
	cancel_bt = (Button*)App->gui->CreateButton("gui/button.png", 570, 330, blit_sections, detect_sections, TIER2);
	Label* back_to_menu_lbl = (Label*)App->gui->CreateLabel("Back To Main Menu", 605, 213, nullptr);
	Label* quit_game_lbl = (Label*)App->gui->CreateLabel("Quit Game", 640, 253, nullptr);
	Label* save_game_lbl = (Label*)App->gui->CreateLabel("Save Game", 640, 293, nullptr);
	Label* cancel_lbl = (Label*)App->gui->CreateLabel("Cancel", 650, 333, nullptr);

	back_to_menu_lbl->SetSize(16);
	quit_game_lbl->SetSize(16);
	save_game_lbl->SetSize(16);
	cancel_lbl->SetSize(16);

	ui_menu.in_window.push_back(menu_bg_img);
	ui_menu.in_window.push_back(quit_game_bt);
	ui_menu.in_window.push_back(back_to_menu_bt);
	ui_menu.in_window.push_back(cancel_bt);
	ui_menu.in_window.push_back(save_game_bt);
	ui_menu.in_window.push_back(back_to_menu_lbl);
	ui_menu.in_window.push_back(quit_game_lbl);
	ui_menu.in_window.push_back(save_game_lbl);
	ui_menu.in_window.push_back(cancel_lbl);

	ui_menu.WindowOff();
	ui_menu.SetFocus(menu_bg_img->pos.first, menu_bg_img->pos.second, 280, 280);

	// SET UI PRIORITY

	App->gui->SetPriority();
	// ---------------------
	// DONE!


	//Test
	elvenArcher = App->entityManager->CreateUnit(350, 350, false, ELVEN_ARCHER);
	elvenArcher = App->entityManager->CreateUnit(100, 500, false, ELVEN_ARCHER);
	elvenArcher = App->entityManager->CreateUnit(320, 350, false, ELVEN_ARCHER);
	elvenArcher = App->entityManager->CreateUnit(100, 520, false, ELVEN_ARCHER);
	elvenArcher = App->entityManager->CreateUnit(380, 350, false, ELVEN_ARCHER);
	elvenArcher = App->entityManager->CreateUnit(140, 500, false, ELVEN_ARCHER);
	troll = App->entityManager->CreateUnit(600, 400, true, TROLL_MAULER);
	//testBuilding = App->entityManager->CreateBuilding(150, 100, true, ORC_BARRACKS);
	tree = App->entityManager->CreateResource(400, 400, WOOD);

	build = App->entityManager->CreateBuilding(100, 100, false, TOWN_CENTER);

	//App->fog->CreateFog(App->map->data.mapWidth, App->map->data.mapHeight);
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	App->map->Draw();
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
	}
	App->gui->ScreenMoves(App->render->MoveCameraWithCursor(dt));

	if (debug) {
		const list<iPoint>* path = App->pathfinding->GetLastPath();

		for (list<iPoint>::const_iterator it = path->begin(); it != path->end(); it++) {
			iPoint pos = App->map->MapToWorld((*it).x, (*it).y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}


	if (ui_menu.IsEnabled()) App->gui->Focus(ui_menu.FocusArea());



	if (menu_bt->current == HOVER || menu_bt->current == CLICKIN) App->gui->cursor->SetCursor(3);
	else App->gui->cursor->SetCursor(0);

	if (menu_bt->current == CLICKIN) ui_menu.WindowOn();

	if (quit_game_bt->current == CLICKIN) App->quit = true;
	else if (cancel_bt->current == CLICKIN) {
		ui_menu.WindowOff();
		App->gui->Unfocus();
	}
	


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (back_to_menu_bt->current == CLICKIN) {
		App->sceneManager->ChangeScene(this, App->sceneManager->menu_scene);
	}
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");


	App->gui->DestroyUIElement(top);
	App->gui->DestroyUIElement(bottom);
	App->gui->DestroyUIElement(menu_bt);
	App->gui->DestroyUIElement(menu_bg_img);
	App->gui->DestroyUIElement(quit_game_bt);
	App->gui->DestroyUIElement(back_to_menu_bt);
	App->gui->DestroyUIElement(save_game_bt);
	App->gui->DestroyUIElement(cancel_bt);
	App->gui->DestroyUIElement(back_to_menu_lbl);
	App->gui->DestroyUIElement(quit_game_lbl);
	App->gui->DestroyUIElement(save_game_lbl);
	App->gui->DestroyUIElement(cancel_lbl);
	ui_menu.CleanUp();
	App->entityManager->CleanUp();


	return true;
}