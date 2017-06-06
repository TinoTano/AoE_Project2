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
#include "Orders.h"
#include "EntityManager.h"
#include "Unit.h"
#include "FogOfWar.h"
#include "GameFaction.h"
#include "Gui.h"
#include <sstream>
#include "StaticQuadtree.h"
#include "AI.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include "Building.h"
#include "FogOfWar.h"
#include "Minimap.h"
#include "Fonts.h"
#include "CutSceneManager.h"

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
	active = false;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	active = true;

	// Creating map ==================================================

	if (start == false)
	{
		if (App->map->Load("map_1.tmx") == true)
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
				App->pathfinding->SetMap(w, h, data);

			RELEASE_ARRAY(data);
		}

		start = true;
	}

	App->pathfinding->isGameScene = true;

	// Loading camera position && limits =======================================

	App->render->camera.x = STARTING_CAMERA_X;
	App->render->camera.y = STARTING_CAMERA_Y;

	App->render->cameraScene.down = -3745;
	App->render->cameraScene.up = -245;
	App->render->cameraScene.left = 3993;
	App->render->cameraScene.right = -3000;

	// Loading UI ====================================================

	uint x, y;
	App->win->GetWindowSize(x, y);

	elements = App->gui->GetElements("LEVEL");

	elements[0].position.first = -STARTING_CAMERA_X + 0;
	elements[0].position.second = -STARTING_CAMERA_Y + 0;

	elements[1].position.first = -STARTING_CAMERA_X + 0;
	elements[1].position.second = -STARTING_CAMERA_Y + y - elements[1].rect.h;

	elements[2].position.first = -STARTING_CAMERA_X + x - elements[2].rect.w;
	elements[2].position.second = -STARTING_CAMERA_Y + y - elements[2].rect.h;

	SDL_Rect NotHUD{ 0,30, x, y - elements[2].rect.h };

	App->entityManager->NotHUD = NotHUD;
	elements[3].position.first = -STARTING_CAMERA_X + x / 3 + x / 50;
	elements[3].position.second = -STARTING_CAMERA_Y + y / 8 + y / 16;

	elements[4].position.first = -STARTING_CAMERA_X + x - (x / 4);
	elements[4].position.second = -STARTING_CAMERA_Y + y / 100;

	elements[5].position.first = elements[3].position.first + x / 50;
	elements[5].position.second = elements[3].position.second + y / 50;

	elements[6].position.first = elements[5].position.first;
	elements[6].position.second = elements[5].position.second + elements[5].detect_sections.front().h + y / 100;

	elements[7].position.first = elements[6].position.first;
	elements[7].position.second = elements[6].position.second + elements[6].detect_sections.front().h + y / 100;

	elements[8].position.first = elements[7].position.first;
	elements[8].position.second = elements[7].position.second + elements[7].detect_sections.front().h + y / 100;

	elements[9].position.first = elements[8].position.first;
	elements[9].position.second = elements[8].position.second + elements[8].detect_sections.front().h + y / 100;

	elements[10].position.first = elements[4].position.first + elements[10].detect_sections.front().w + 5;
	elements[10].position.second = elements[4].position.second;

	elements[11].position.first = elements[8].position.first;
	elements[11].position.second = elements[8].position.second;

	elements[12].position.first = elements[9].position.first;
	elements[12].position.second = elements[9].position.second;
	

	for (uint it = 0; it < elements.size(); ++it) {
		switch (elements[it].type)
		{
		case IMAGE:
			images.push_back((Image*)App->gui->CreateImage(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].rect));
			images.back()->loaded_tex = true;
			break;
		case BUTTON:
			buttons.push_back((Button*)App->gui->CreateButton(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].blit_sections, elements[it].detect_sections, elements[it].tier));
			buttons.back()->loaded_tex = true;
			break;
		}
	}

	Label* back_to_menu_lbl = (Label*)App->gui->CreateLabel("Back To Main Menu", buttons[BACKTOMENU]->pos.first + x / 30, buttons[BACKTOMENU]->pos.second, App->font->fonts[SIXTEEN]);
	Label* quit_game_lbl = (Label*)App->gui->CreateLabel("Quit Game", buttons[QUITGAME]->pos.first + x / 20, buttons[QUITGAME]->pos.second, App->font->fonts[SIXTEEN]);
	Label* save_game_lbl = (Label*)App->gui->CreateLabel("Save Game", buttons[SAVEGAME]->pos.first + x / 20, buttons[SAVEGAME]->pos.second, App->font->fonts[SIXTEEN]);
	Label* cancel_lbl = (Label*)App->gui->CreateLabel("Cancel", buttons[CANCEL]->pos.first + x / 18, buttons[CANCEL]->pos.second, App->font->fonts[SIXTEEN]);
	Label* load_game_lbl = (Label*)App->gui->CreateLabel("Load Game", buttons[LOADGAME]->pos.first + x / 20, buttons[LOADGAME]->pos.second, App->font->fonts[SIXTEEN]);
	
	Label* yes_lbl = (Label*)App->gui->CreateLabel("YES", buttons[LOADGAME]->pos.first + x / 13, buttons[LOADGAME]->pos.second + y/500, App->font->fonts[SIXTEEN]);
	Label* no_lbl = (Label*)App->gui->CreateLabel("NO", buttons[CANCEL]->pos.first + x / 13, buttons[CANCEL]->pos.second + y / 500, App->font->fonts[SIXTEEN]);
	Label* surrender_lbl = (Label*)App->gui->CreateLabel("Do you want to surrender?", buttons[BACKTOMENU]->pos.first + x / 50, buttons[BACKTOMENU]->pos.second, App->font->fonts[SIXTEEN]);
	
	surrender_menu.in_window.push_back(images[WINDOW]);
	surrender_menu.in_window.push_back(buttons[YES]);
	surrender_menu.in_window.push_back(buttons[NO]);
	surrender_menu.in_window.push_back(yes_lbl);
	surrender_menu.in_window.push_back(no_lbl);
	surrender_menu.in_window.push_back(surrender_lbl);

	surrender_menu.WindowOff();

	surrender_menu.SetFocus(images[3]->pos.first, images[3]->pos.second, 280, 280);

	ui_menu.in_window.push_back(images[WINDOW]);
	ui_menu.in_window.push_back(buttons[BACKTOMENU]);
	ui_menu.in_window.push_back(buttons[QUITGAME]);
	ui_menu.in_window.push_back(buttons[SAVEGAME]);
	ui_menu.in_window.push_back(buttons[LOADGAME]);
	ui_menu.in_window.push_back(buttons[CANCEL]);
	ui_menu.in_window.push_back(back_to_menu_lbl);
	ui_menu.in_window.push_back(quit_game_lbl);
	ui_menu.in_window.push_back(save_game_lbl);
	ui_menu.in_window.push_back(load_game_lbl);
	ui_menu.in_window.push_back(cancel_lbl);

	ui_menu.WindowOff();
	ui_menu.SetFocus(images[3]->pos.first, images[3]->pos.second, 280, 280);

	// Labels
	wood = (Label*)App->gui->CreateLabel(to_string(App->entityManager->player->resources.wood), -STARTING_CAMERA_X + 50, -STARTING_CAMERA_Y + 5, nullptr);
	wood->SetColor({ 255, 255, 255 ,255 });
	food = (Label*)App->gui->CreateLabel(to_string(App->entityManager->player->resources.food), -STARTING_CAMERA_X + 150, -STARTING_CAMERA_Y + 5, nullptr);
	food->SetColor({ 255, 255, 255 ,255 });
	gold = (Label*)App->gui->CreateLabel(to_string(App->entityManager->player->resources.gold), -STARTING_CAMERA_X + 280, -STARTING_CAMERA_Y + 5, nullptr);
	gold->SetColor({ 255, 255, 255 ,255 });
	stone = (Label*)App->gui->CreateLabel(to_string(App->entityManager->player->resources.stone), -STARTING_CAMERA_X + 360, -STARTING_CAMERA_Y + 5, nullptr);
	stone->SetColor({ 255, 255, 255 ,255 });
	villagers = (Label*)App->gui->CreateLabel("0/0", -STARTING_CAMERA_X + 480, -STARTING_CAMERA_Y + 5, nullptr);
	villagers->SetColor({ 255, 255, 255 ,255 });

	// Fog of war, entities & resources ===============================================================================

	App->fog->Start();

	//Resources
	App->map->LoadResources(App->map->map_file.child("map"));

	// Units
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 300, TOWN_HALL_POS_Y, ELVEN_CAVALRY);

	// Buildings
	App->entityManager->player->Town_center = App->entityManager->CreateBuilding(TOWN_HALL_POS_X, TOWN_HALL_POS_Y, TOWN_CENTER);
	App->entityManager->player->Town_center;


	// Villager
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 250, TOWN_HALL_POS_Y + 50, ELF_VILLAGER);

	//AI 
	App->ai->enabled = true;

	iPoint enemyTownCenterPos{ 1800, 3100 };
	App->entityManager->AI_faction->Town_center;
	App->ai->selected_building = App->entityManager->AI_faction->Town_center = App->entityManager->CreateBuilding(enemyTownCenterPos.x, enemyTownCenterPos.y, SAURON_TOWER);

	App->entityManager->CreateUnit(enemyTownCenterPos.x + 150, enemyTownCenterPos.y , TROLL_MAULER);
	App->entityManager->CreateUnit(enemyTownCenterPos.x - 150, enemyTownCenterPos.y , TROLL_MAULER);

	Building* aux = App->entityManager->CreateBuilding(-100, 800, ORC_BARRACKS);
	aux->Life = aux->MaxLife;
	aux->entityTexture = App->entityManager->buildingsDB[ORC_BARRACKS]->entityTexture;
	aux->GetBuildingBoundaries();
	aux->state = IDLE;
	aux = App->entityManager->CreateBuilding(0, 4100, ORC_ARCHERY_RANGE);
	aux->Life = aux->MaxLife;
	aux->entityTexture = App->entityManager->buildingsDB[ORC_ARCHERY_RANGE]->entityTexture;
	aux->GetBuildingBoundaries();
	aux->state = IDLE;

	App->entityManager->CreateBuilding(enemyTownCenterPos.x + 250, enemyTownCenterPos.y - 250, BEAST_PIT);
	App->entityManager->CreateBuilding(enemyTownCenterPos.x - 250, enemyTownCenterPos.y - 250, ORC_ARCHERY_RANGE);
	App->entityManager->CreateBuilding(enemyTownCenterPos.x + 400, enemyTownCenterPos.y, ORC_BARRACKS);
	App->entityManager->CreateBuilding(enemyTownCenterPos.x - 400, enemyTownCenterPos.y, URUK_HAI_PIT);
	App->entityManager->CreateBuilding(enemyTownCenterPos.x, enemyTownCenterPos.y + 300, ORC_BLACKSMITH);

	// ================================================================================================================

	game_finished = false;

	App->entityManager->player->resources.food += 0;
	App->entityManager->player->resources.stone += 0;
	App->entityManager->player->resources.gold += 0;
	App->entityManager->player->resources.wood += 0;

	UpdateResources();

	App->quest->Start();
	questHUD.Start();

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
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		//debug = !debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		App->map->godmode = !App->map->godmode;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		App->entityManager->player->resources.wood += 500;
		App->sceneManager->level1_scene->UpdateResources();
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		App->entityManager->player->resources.food += 500;
		App->sceneManager->level1_scene->UpdateResources();
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		App->entityManager->player->resources.gold += 500;
		App->sceneManager->level1_scene->UpdateResources();
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		App->entityManager->player->resources.stone += 500;
		App->sceneManager->level1_scene->UpdateResources();
	}

	// Move camera by shortcut
	if (App->input->GetKey(App->input->controls[MOVE_CAMERA]) == KEY_DOWN) {

		iPoint cameraOldPos = { App->render->camera.x, App->render->camera.y };


		App->render->camera.x = STARTING_CAMERA_X;
		App->render->camera.y = STARTING_CAMERA_Y;
		App->render->culling_cam.x = -App->render->camera.x - 300;
		App->render->culling_cam.y = -App->render->camera.y - 300;

		std::pair<int, int> movement;
		movement.first = App->render->camera.x - cameraOldPos.x;
		movement.second = App->render->camera.y - cameraOldPos.y;

		App->gui->ScreenMoves(movement);
	}


	App->gui->ScreenMoves(App->render->MoveCameraWithCursor(dt));
	App->map->Draw();


	// --------------------------------------------
	//						UI
	//---------------------------------------------
	if (ui_menu.IsEnabled())
	{
		App->gui->Focus(ui_menu.FocusArea());

		if (buttons[QUITGAME]->current == CLICKUP) App->quit = true;

		if (buttons[CANCEL]->current == CLICKUP) {
			ui_menu.WindowOff();
			App->gui->Unfocus();
			App->entityManager->game_stops = false;
		}
		if (buttons[LOADGAME]->current == CLICKUP)
		{
			ui_menu.WindowOff();
			App->gui->Unfocus();
			App->entityManager->game_stops = false;
			App->LoadGame();
		}
		if (buttons[SAVEGAME]->current == CLICKUP)
		{
			ui_menu.WindowOff();
			App->gui->Unfocus();
			App->entityManager->game_stops = false;
			App->SaveGame();
		}
	}
	if (buttons[MENU]->current == CLICKUP)
	{
		ui_menu.WindowOn();
		App->entityManager->game_stops = true;
	}

	if (buttons[SURRENDER]->current == CLICKUP)
	{
		surrender_menu.WindowOn();
		App->entityManager->game_stops = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		if (ui_menu.IsEnabled()) {
			ui_menu.WindowOff();
			App->entityManager->game_stops = false;
			App->gui->Unfocus();
		}
		else if (!ui_menu.IsEnabled() && !surrender_menu.IsEnabled())
		{
			ui_menu.WindowOn();
			App->entityManager->game_stops = true;
		}
		else if (!ui_menu.IsEnabled() && surrender_menu.IsEnabled())
		{
			surrender_menu.WindowOff();
			App->entityManager->game_stops = false;
			App->gui->Unfocus();
		}
	}

	questHUD.Update();
	UpdatePopulation();
	App->minimap->GetClickableArea(images[MINIMAP]->pos);

	if (surrender_menu.IsEnabled()) {
		App->gui->Focus(surrender_menu.FocusArea());

		if (buttons[YES]->current == CLICKUP)
		{
			//App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);
			App->cutscene->Start();
			App->cutscene->Play("cutscene/first_cutscene.xml", App->sceneManager->level1_scene);
		}
		if (buttons[NO]->current == CLICKUP)
		{
			surrender_menu.WindowOff();
			App->gui->Unfocus();
			App->entityManager->game_stops = false;
		}
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if ((App->entityManager->player->Town_center->Life <= 0 && game_finished == false) || (App->entityManager->player->units.size() <= 0 && App->entityManager->player->resources.food < 50 && game_finished == false)) {
		uint w, h;
		App->win->GetWindowSize(w, h);
		game_finished = true;
		Label* defeat = (Label*)App->gui->CreateLabel("DEFEAT", w/3 - App->render->camera.x, h/3 - App->render->camera.y, App->font->fonts[EIGHTY]);
		defeat->SetColor({ 255, 0, 0, 255 });
		App->audio->PlayFx(DEFEAT);
		App->entityManager->game_stops = true;
	}

	else if (App->entityManager->AI_faction->Town_center->Life <= 0 && game_finished == false) {
		uint w, h;
		App->win->GetWindowSize(w, h);
		Label* victory = (Label*)App->gui->CreateLabel("VICTORY", w/3 - App->render->camera.x, h/3 - App->render->camera.y, App->font->fonts[EIGHTY]);
		App->audio->PlayFx(VICTORY);
		victory->SetColor({ 0, 255, 0, 255 });
		game_finished = true;
		App->entityManager->game_stops = true;
	}

	else if (buttons[BACKTOMENU]->current == CLICKUP) {
		App->sceneManager->ChangeScene(this, App->sceneManager->menu_scene);
		App->entityManager->game_stops = false;
			
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	App->entityManager->player->tech_tree->Reset(FREE_MEN);
	App->entityManager->AI_faction->tech_tree->Reset(SAURON_ARMY);

	questHUD.CleanUp();
	ui_menu.CleanUp();
	surrender_menu.CleanUp();

	App->gui->hud->ClearAll();
	App->gui->DestroyALLUIElements();

	elements.clear();
	images.clear();
	buttons.clear();

	App->ai->enabled = false;

	App->entityManager->CleanUp();
	App->collision->CleanUp();
	App->fog->CleanUp();
	App->quest->CleanUp();


	App->entityManager->game_stops = false;


	return true;
}

void Scene::UpdateTime(float time)
{
	Timer_lbl->SetString(to_string((int)time / 60 / 10) + to_string((int)time / 60 % 10) + ':' + to_string((int)time % 60 / 10) + to_string((int)time % 60 % 10));
}

void Scene::UpdateResources()
{
	wood->SetString(to_string(App->entityManager->player->resources.wood));
	food->SetString(to_string(App->entityManager->player->resources.food));
	stone->SetString(to_string(App->entityManager->player->resources.stone));
	gold->SetString(to_string(App->entityManager->player->resources.gold));
}

void Scene::UpdatePopulation()
{
	uint houses_count = 0;
	uint units_in_queue = 0;
	for (list<Building*>::iterator it = App->entityManager->player->buildings.begin(); it != App->entityManager->player->buildings.end(); ++it) {
		if ((*it)->type == HOUSE) houses_count++;
		units_in_queue += (*it)->units_in_queue.size();
	}
	villagers->SetString(to_string(App->entityManager->player->units.size() + units_in_queue) + '/' + to_string((houses_count * 5) + 2));
}

bool Scene::CheckUnitsRoom()
{
	uint houses_count = 0;
	uint units_in_queue = 0;
	for (list<Building*>::iterator it = App->entityManager->player->buildings.begin(); it != App->entityManager->player->buildings.end(); ++it) {
		if ((*it)->type == HOUSE) houses_count++;
		units_in_queue += (*it)->units_in_queue.size();
	}
	return ((App->entityManager->player->units.size() + units_in_queue) < (houses_count * 5) + 2);
}