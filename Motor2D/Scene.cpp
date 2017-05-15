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
#include "FileSystem.h"
#include "SceneManager.h"
#include "QuadTree.h"
#include "Building.h"
#include "FogOfWar.h"
#include "Minimap.h"
#include "Fonts.h"

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

	// Loading sounds ================================================

	soundAttack = App->audio->LoadFx("audio/fx/Swords.wav");
	soundSpiderAttack = App->audio->LoadFx("audio/fx/AmbientSounds/Spiders.wav");
	//soundTrollAttack = App->audio->LoadFx("audio/fx/"); //no hay
	//soundBalrogAttack = App->audio->LoadFx("audio/fx/"); //no hay
	soundArcherAttack = App->audio->LoadFx("audio/fx/bow.wav");
	soundHorseAttack = App->audio->LoadFx("audio/fx/Dead_Horse_1.wav");

	//soundWood = App->audio->LoadFx("audio/fx/Fight_2.wav"); //no hay
	//soundStone = App->audio->LoadFx("audio/fx/Fight_2.wav");
	//soundFruit = App->audio->LoadFx("audio/fx/Fight_2.wav"); //no hay
	//soundBuilding = App->audio->LoadFx("audio/fx/Fight_2.wav"); //no hay
	
	soundCreateVillager = App->audio->LoadFx("audio/fx/Swords.wav");
	soundCreateUnit = App->audio->LoadFx("audio/fx/Swords.wav");
	soundCreateLegolas = App->audio->LoadFx("audio/fx/Swords.wav");

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

		debug_tex = App->tex->Load("maps/path2.png");
		start = true;
	}

	// Loading camera position && limits =======================================

	App->render->camera.x = STARTING_CAMERA_X;
	App->render->camera.y = STARTING_CAMERA_Y;

	App->render->cameraScene.down = -3090;
	App->render->cameraScene.up = -1180;
	App->render->cameraScene.left = 1680;
	App->render->cameraScene.right = -1410;

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

	Label* back_to_menu_lbl = (Label*)App->gui->CreateLabel("Back To Main Menu", buttons[BACKTOMENU]->pos.first + x / 30, buttons[BACKTOMENU]->pos.second , App->font->fonts[SIXTEEN]);
	Label* quit_game_lbl = (Label*)App->gui->CreateLabel("Quit Game", buttons[QUITGAME]->pos.first + x / 20, buttons[QUITGAME]->pos.second, App->font->fonts[SIXTEEN]);
	Label* save_game_lbl = (Label*)App->gui->CreateLabel("Save Game", buttons[SAVEGAME]->pos.first + x / 20, buttons[SAVEGAME]->pos.second, App->font->fonts[SIXTEEN]);
	Label* cancel_lbl = (Label*)App->gui->CreateLabel("Cancel", buttons[CANCEL]->pos.first + x / 15, buttons[CANCEL]->pos.second, App->font->fonts[SIXTEEN]);
	Label* load_game_lbl = (Label*)App->gui->CreateLabel("Load Game", buttons[LOADGAME]->pos.first + x / 35, buttons[LOADGAME]->pos.second, App->font->fonts[SIXTEEN]);

	ui_menu.in_window.push_back(images[WINDOW]);
	ui_menu.in_window.push_back(buttons[BACKTOMENU]);
	ui_menu.in_window.push_back(buttons[QUITGAME]);
	ui_menu.in_window.push_back(buttons[SAVEGAME]);
	ui_menu.in_window.push_back(buttons[CANCEL]);
	ui_menu.in_window.push_back(buttons[LOADGAME]);
	ui_menu.in_window.push_back(back_to_menu_lbl);
	ui_menu.in_window.push_back(quit_game_lbl);
	ui_menu.in_window.push_back(save_game_lbl);
	ui_menu.in_window.push_back(cancel_lbl);
	ui_menu.in_window.push_back(load_game_lbl);

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

	// Music ==========================================================================================================

	App->audio->PlayMusic("audio/music/m_scene.ogg", 0.0f);

	// Fog of war, entities & resources ===============================================================================

	/*App->fog->Start();*/

	//Resources
	App->map->LoadResources(App->map->map_file.child("map"));

	// Units
	hero = App->entityManager->CreateUnit(TOWN_HALL_POS_X + 300, TOWN_HALL_POS_Y, ELVEN_CAVALRY);
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 500, TOWN_HALL_POS_Y + 1100, VENOMOUS_SPIDER);
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 380, TOWN_HALL_POS_Y + 1070, TROLL_MAULER);
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 400, TOWN_HALL_POS_Y + 1200, VENOMOUS_SPIDER);
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 300, TOWN_HALL_POS_Y + 1020, VENOMOUS_SPIDER);

	// Buildings
	App->entityManager->player->Town_center = App->entityManager->CreateBuilding(TOWN_HALL_POS_X, TOWN_HALL_POS_Y, TOWN_CENTER);
	App->fog->AddEntity(App->entityManager->player->Town_center);

	iPoint enemyTownCenterPos{ 1800, 2800, };
	App->entityManager->AI_faction->Town_center = App->entityManager->CreateBuilding(enemyTownCenterPos.x, enemyTownCenterPos.y, SAURON_TOWER);
	App->fog->AddEntity(App->entityManager->AI_faction->Town_center);

	// Villager
	App->entityManager->CreateUnit(TOWN_HALL_POS_X + 250, TOWN_HALL_POS_Y + 50, ELF_VILLAGER);

	App->entityManager->CreateUnit(enemyTownCenterPos.x - 250, enemyTownCenterPos.y + 200, VILLAGER);
	App->entityManager->CreateUnit(enemyTownCenterPos.x - 280, enemyTownCenterPos.y + 200, VILLAGER);

	// Enable AI

	App->ai->enabled = true;
	App->ai->LoadExplorationMap();
	App->collision->quadTree;


	// ================================================================================================================

	timer.Start();

	Timer_lbl = (Label*)App->gui->CreateLabel("00:00", -STARTING_CAMERA_X + 665, -STARTING_CAMERA_Y + 40, App->font->fonts[TWENTYSIX]);
	Timer_lbl->SetColor({ 255, 255, 255, 255 });

	game_finished = false;

	App->entityManager->player->resources.food += 100;
	App->entityManager->player->resources.stone += 100;
	App->entityManager->player->resources.gold += 100;
	App->entityManager->player->resources.wood += 100;
	UpdateResources();

	villagers_curr = villagers_total = 1;
	UpdateVillagers(villagers_curr, villagers_total);

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

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		questHUD.RemoveQuest(2);
	}
	/*
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		SaveScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		LoadScene();
	}
*/
	
	App->gui->ScreenMoves(App->render->MoveCameraWithCursor(dt));
	App->map->Draw();

	if (debug) {
		const list<iPoint>* path = App->pathfinding->GetLastPath();

		for (list<iPoint>::const_iterator it = path->begin(); it != path->end(); it++) {
			iPoint pos = App->map->MapToWorld((*it).x, (*it).y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}

	// --------------------------------------------
	//						UI
	//---------------------------------------------
	if (ui_menu.IsEnabled()) App->gui->Focus(ui_menu.FocusArea());

	if (buttons[MENU]->current == CLICKUP || App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		UpdateVillagers(5, 10);
		ui_menu.WindowOn();
	}
	if (buttons[QUITGAME]->current == CLICKUP) App->quit = true;

	else if (buttons[CANCEL]->current == CLICKUP) {
		ui_menu.WindowOff();
		App->gui->Unfocus();
	}

	bool cursoron = false;
	for (uint i = 0; i < buttons.size(); ++i) {
		if (buttons[i]->current == HOVER || buttons[i]->current == CLICKIN)
			cursoron = true;
	}

	if (cursoron == true)
		App->gui->cursor->SetCursor(3);
	else App->gui->cursor->SetCursor(0);

	questHUD.Update();

	App->minimap->GetClickableArea(images[MINIMAP]->pos);

	// ---------------------------------------

	UpdateTime(timer.ReadSec());

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (App->entityManager->player->Town_center->Life <= 0 && game_finished == false) {
		Timer_lbl->SetString("DEFEAT");
		Timer_lbl->SetColor({255, 0,0,255});
		game_finished = true;
	}
	else if (App->entityManager->AI_faction->Town_center->Life <= 0 && game_finished == false) {
		Timer_lbl->SetString("VICTORY");
		Timer_lbl->SetColor({ 0, 255 ,0 , 255 });
		game_finished = true;
	}
	if (buttons[BACKTOMENU]->current == CLICKIN) {
		App->audio->PlayFx(App->sceneManager->menu_scene->fx_button_click);
	}
	else if (buttons[BACKTOMENU]->current == CLICKUP) {
		App->sceneManager->ChangeScene(this, App->sceneManager->menu_scene);
	}
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	questHUD.CleanUp();
	ui_menu.CleanUp();
	App->gui->DestroyALLUIElements();
	elements.clear();
	images.clear();
	buttons.clear();

	App->ai->enabled = false;

	App->entityManager->CleanUp();
	App->collision->CleanUp();
	App->fog->CleanUp();
	return true;
}

void Scene::TimeEvents() {
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

void Scene::UpdateVillagers(uint available_villagers, uint total_villagers)
{
	villagers->SetString(to_string(available_villagers) + '/' + to_string(total_villagers));
}

void Scene::SaveScene()
{
	pugi::xml_document saveFile;;
	pugi::xml_node rootNode;

	rootNode = saveFile.append_child("Scene");

	pugi::xml_node unitsNode = rootNode.append_child("Units");
	for (list<Unit*>::iterator it = App->entityManager->player->units.begin(); it != App->entityManager->player->units.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node unitNodeInfo = unitsNode.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			if ((*it)->path != nullptr) {
				destTileNode.append_attribute("x") = (*it)->path->back().x;
				destTileNode.append_attribute("y") = (*it)->path->back().y;
			}
		}
	}

	for (list<Unit*>::iterator it = App->entityManager->AI_faction->units.begin(); it != App->entityManager->AI_faction->units.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node unitNodeInfo = unitsNode.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			if ((*it)->path != nullptr) {
				destTileNode.append_attribute("x") = (*it)->path->back().x;
				destTileNode.append_attribute("y") = (*it)->path->back().y;
			}
		}
	}

	pugi::xml_node buildingNode = rootNode.append_child("Buildings");
	for (list<Building*>::iterator it = App->entityManager->player->buildings.begin(); it != App->entityManager->player->buildings.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node buildingNodeInfo = buildingNode.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
		}
	}

	for (list<Building*>::iterator it = App->entityManager->AI_faction->buildings.begin(); it != App->entityManager->AI_faction->buildings.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node buildingNodeInfo = buildingNode.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
		}
	}

	pugi::xml_node resourcesNode = rootNode.append_child("Resources");
	for (list<Resource*>::iterator it = App->entityManager->aux_resource_list.begin(); it != App->entityManager->aux_resource_list.end(); it++) {
			pugi::xml_node resourceNodeInfo = resourcesNode.append_child("Resource");
			resourceNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = resourceNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			resourceNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			resourceNodeInfo.append_child("State").append_attribute("value") = (*it)->isDamaged;
	}

	stringstream stream;
	saveFile.save(stream);

	App->fs->Save("SavedGame.xml", stream.str().c_str(), stream.str().length());
}

void Scene::LoadScene() {

	pugi::xml_document savedFile;
	pugi::xml_node scene;

	char* buf = NULL;
	int size = App->fs->Load("save/SavedGame.xml", &buf);
	pugi::xml_parse_result result = savedFile.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
	{
		LOG("Could not load xml file %s. PUGI error: %s", "SavedGame.xml", result.description());
		return;
	}
	else {
		scene = savedFile.child("Scene");
	}
	
	if (!savedFile.empty()) {
		for (pugi::xml_node unitNodeInfo = scene.child("Units").child("Unit"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Unit")) {

			Unit* unitTemplate = App->entityManager->CreateUnit(unitNodeInfo.child("Position").attribute("x").as_int(),
				unitNodeInfo.child("Position").attribute("y").as_int(),
				(unitType)unitNodeInfo.child("Type").attribute("value").as_int());

			unitTemplate->currentDirection = (unitDirection)unitNodeInfo.child("Direction").attribute("value").as_int();
			unitTemplate->Life = unitNodeInfo.child("Life").attribute("value").as_int();
			if (unitNodeInfo.child("State").attribute("value").as_int() == MOVING) {
				Order* moveOrder = new MoveToOrder({ unitNodeInfo.child("DestinationTile").attribute("x").as_int(), unitNodeInfo.child("DestinationTile").attribute("y").as_int() });
				unitTemplate->order_list.push_back(moveOrder);
			}
			/*else if (unitNodeInfo.child("State").attribute("value").as_int() == ATTACKING) {
			Order* attackOrder = new UnitAttackOrder()
			}*/
		}

		for (pugi::xml_node buildingNodeInfo = scene.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNodeInfo.child("Position").attribute("x").as_int(),
				buildingNodeInfo.child("Position").attribute("y").as_int(),
				(buildingType)buildingNodeInfo.child("Type").attribute("value").as_int());

			buildingTemplate->Life = buildingNodeInfo.child("Life").attribute("value").as_int();
			buildingTemplate->state = (EntityState)buildingNodeInfo.child("State").attribute("value").as_int();
		}

		for (pugi::xml_node resourceNodeInfo = scene.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource")) {

			Resource* resourceTemplate = App->entityManager->CreateResource(resourceNodeInfo.child("Position").attribute("x").as_int(),
				resourceNodeInfo.child("Position").attribute("y").as_int(),
				(resourceItem)resourceNodeInfo.child("Type").attribute("value").as_int());

			resourceTemplate->Life = resourceNodeInfo.child("Life").attribute("value").as_int();
			if (resourceNodeInfo.child("State").attribute("value").as_bool()) {
				resourceTemplate->Damaged();
			}
		}
	}

}
