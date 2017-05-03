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
#include <sstream>
#include "FileSystem.h"
#include "SceneManager.h"
#include "QuadTree.h"
#include "Building.h"
#include "FogOfWar.h"

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

	// LOADING CAMERA POSITION

	App->render->camera.x = STARTING_CAMERA_X;

	App->render->camera.y = STARTING_CAMERA_Y;

	uint x, y;
	App->win->GetWindowSize(x, y);

	// LOADING UI BB
	// ---------------------------------------
	// LOADING SCENE UI

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

	Label* back_to_menu_lbl = (Label*)App->gui->CreateLabel("Back To Main Menu", buttons[BACKTOMENU]->pos.first + x / 30, buttons[BACKTOMENU]->pos.second, nullptr);
	Label* quit_game_lbl = (Label*)App->gui->CreateLabel("Quit Game", buttons[QUITGAME]->pos.first + x / 20, buttons[QUITGAME]->pos.second, nullptr);
	Label* save_game_lbl = (Label*)App->gui->CreateLabel("Save Game", buttons[SAVEGAME]->pos.first + x / 20, buttons[SAVEGAME]->pos.second, nullptr);
	Label* cancel_lbl = (Label*)App->gui->CreateLabel("Cancel", buttons[CANCEL]->pos.first + x / 15, buttons[CANCEL]->pos.second, nullptr);

	back_to_menu_lbl->SetSize(16);
	quit_game_lbl->SetSize(16);
	save_game_lbl->SetSize(16);
	cancel_lbl->SetSize(16);

	ui_menu.in_window.push_back(images[WINDOW]);
	ui_menu.in_window.push_back(buttons[BACKTOMENU]);
	ui_menu.in_window.push_back(buttons[QUITGAME]);
	ui_menu.in_window.push_back(buttons[SAVEGAME]);
	ui_menu.in_window.push_back(buttons[CANCEL]);
	ui_menu.in_window.push_back(back_to_menu_lbl);
	ui_menu.in_window.push_back(quit_game_lbl);
	ui_menu.in_window.push_back(save_game_lbl);
	ui_menu.in_window.push_back(cancel_lbl);

	ui_menu.WindowOff();
	ui_menu.SetFocus(images[3]->pos.first, images[3]->pos.second, 280, 280);

	// RESOURCE LABELS
	wood = (Label*)App->gui->CreateLabel(to_string(woodCount), -STARTING_CAMERA_X + 50, -STARTING_CAMERA_Y + 5, nullptr);
	wood->SetColor({ 255, 255, 255 ,255 });
	//*food, *gold, *stone, *villagers
	food = (Label*)App->gui->CreateLabel(to_string(foodCount), -STARTING_CAMERA_X + 150, -STARTING_CAMERA_Y + 5, nullptr);
	food->SetColor({ 255, 255, 255 ,255 });

	gold = (Label*)App->gui->CreateLabel(to_string(goldCount), -STARTING_CAMERA_X + 280, -STARTING_CAMERA_Y + 5, nullptr);
	gold->SetColor({ 255, 255, 255 ,255 });

	stone = (Label*)App->gui->CreateLabel(to_string(stoneCount), -STARTING_CAMERA_X + 360, -STARTING_CAMERA_Y + 5, nullptr);
	stone->SetColor({ 255, 255, 255 ,255 });

	villagers = (Label*)App->gui->CreateLabel("0/0", -STARTING_CAMERA_X + 480, -STARTING_CAMERA_Y + 5, nullptr);
	villagers->SetColor({ 255, 255, 255 ,255 });

	// SET UI PRIORITY

	App->gui->SetPriority();

	// MUSIC

	App->audio->PlayMusic("audio/music/m_scene.ogg", 0.0f);
	// ---------------------
	// DONE!

	App->map->LoadResources(App->map->map_file.child("map"));

	// Fog of war, entities & resources

	App->fog->Start(); // Goes first!

	hero = App->entityManager->CreateUnit(TOWN_HALL_POS_X - 50, TOWN_HALL_POS_Y - 180, GONDOR_HERO);
	App->fog->AddEntity(hero);
	App->fog->AddEntity(App->entityManager->CreateUnit(TOWN_HALL_POS_X - 100, TOWN_HALL_POS_Y + 150, ELF_VILLAGER));
	App->fog->AddEntity(App->entityManager->CreateUnit(TOWN_HALL_POS_X + 220, TOWN_HALL_POS_Y + 150, GOBLIN_SOLDIER));
	App->fog->AddEntity(App->entityManager->CreateUnit(TOWN_HALL_POS_X + 250, TOWN_HALL_POS_Y - 180, GOBLIN_SOLDIER));

	UpdateVillagers(3, 3);

	//App->entityManager->CreateUnit(TOWN_HALL_POS_X + 150, TOWN_HALL_POS_Y - 180, true, TROLL_MAULER);

	my_townCenter = App->entityManager->CreateBuilding(TOWN_HALL_POS_X, TOWN_HALL_POS_Y, TOWN_CENTER);
	enemy_townCenter = App->entityManager->CreateBuilding(3200, 1800, SAURON_TOWER);

	//App->fog->CreateFog(App->map->data.mapWidth, App->map->data.mapHeight);

	timer.Start();
	troll_timer.Start();

	Timer_lbl = (Label*)App->gui->CreateLabel("00:00", -STARTING_CAMERA_X + 665, -STARTING_CAMERA_Y + 40, nullptr);
	Timer_lbl->SetColor({ 255, 255, 255, 255 });
	Timer_lbl->SetSize(26);

	game_finished = false;

	wave = 0;
	orcs_to_spawn = 0;
	trolls_to_spawn = 0;
	woodCount = 100;
	foodCount = 0;
	goldCount = 0;
	stoneCount = 0;
	villagers_curr = 0;
	villagers_total = 0;

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

	/*
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		SaveScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		LoadScene();
	}
*/
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		if (App->entityManager->selectedEntityList.size() > 0 && App->entityManager->selectedListType == COLLIDER_UNIT)
			App->entityManager->placingBuilding = !App->entityManager->placingBuilding;
	}

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

	if (buttons[MENU]->current == CLICKUP) {
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


	// ---------------------------------------


	if (timer.ReadSec() > (quadtree_flag + 20)) {
		App->collision->quadTree->UpdateTree();
		quadtree_flag = timer.ReadSec();
	}
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (my_townCenter->Life <= 0 && game_finished == false) {
		Timer_lbl->SetString("DEFEAT");
		Timer_lbl->SetColor({255, 0,0,255});
		game_finished = true;
	}
	else if (enemy_townCenter->Life <= 0 && game_finished == false) {
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

	App->gui->DestroyALLUIElements();
	elements.clear();
	images.clear();
	buttons.clear();
	ui_menu.CleanUp();

	App->entityManager->selectedEntityList.clear();
	App->entityManager->CleanUp();
	return true;
}

void Scene::TimeEvents() {

	if ((int)timer.ReadSec() > 120) {
		Timer_lbl->SetColor({ 255, 0,0,255 });
	}

	if ((int)timer.ReadSec() == 120) {
		orc_timer.Start();
		wave = 2;
	}
	else if ((int)timer.ReadSec() < 120) {
		orc_timer.Start();
	}
	if ((int)troll_timer.ReadSec() == 300) {
		App->entityManager->CreateUnit(2800, 2100, TROLL_MAULER);
		troll_timer.Start();
	}
	if ((int)orc_timer.ReadSec() == 40)
	{
		orcs_to_spawn = wave;
		wave++;
		orc_timer.Start();
	}
		if ((int)orcs_to_spawn > 0 && (int)spawn_timer.ReadSec() > 2) {
			App->entityManager->CreateUnit(2800, 2100, ORC_SOLDIER);
			orcs_to_spawn--;
			spawn_timer.Start();
		}
}
void Scene::UpdateTime(float time)
{
	Timer_lbl->SetString(to_string((int)time / 60 / 10) + to_string((int)time / 60 % 10) + ':' + to_string((int)time % 60 / 10) + to_string((int)time % 60 % 10));
}

void Scene::UpdateResources(Label* resource, uint new_val)
{
	resource->SetString(to_string(new_val));
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
	for (list<Unit*>::iterator it = App->entityManager->friendlyUnitList.begin(); it != App->entityManager->friendlyUnitList.end(); it++) {
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
			destTileNode.append_attribute("x") = (*it)->path->back().x;
			destTileNode.append_attribute("y") = (*it)->path->back().y;
		}
	}

	for (list<Unit*>::iterator it = App->entityManager->enemyUnitList.begin(); it != App->entityManager->enemyUnitList.end(); it++) {
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
			destTileNode.append_attribute("x") = (*it)->path->back().x;
			destTileNode.append_attribute("y") = (*it)->path->back().y;
		}
	}

	pugi::xml_node buildingNode = rootNode.append_child("Buildings");
	for (list<Building*>::iterator it = App->entityManager->friendlyBuildingList.begin(); it != App->entityManager->friendlyBuildingList.end(); it++) {
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

	for (list<Building*>::iterator it = App->entityManager->enemyBuildingList.begin(); it != App->entityManager->enemyBuildingList.end(); it++) {
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
	for (list<Resource*>::iterator it = App->entityManager->resourceList.begin(); it != App->entityManager->resourceList.end(); it++) {
			pugi::xml_node resourceNodeInfo = resourcesNode.append_child("Resource");
			resourceNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = resourceNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			resourceNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
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
				unitTemplate->SetDestination({ unitNodeInfo.child("DestinationTile").attribute("x").as_int(), unitNodeInfo.child("DestinationTile").attribute("y").as_int() });
			}
		}

		for (pugi::xml_node buildingNodeInfo = scene.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNodeInfo.child("Position").attribute("x").as_int(),
				buildingNodeInfo.child("Position").attribute("y").as_int(),
				(buildingType)buildingNodeInfo.child("Type").attribute("value").as_int());

			buildingTemplate->Life = buildingNodeInfo.child("Life").attribute("value").as_int();
			//if (buildingNodeInfo.child("State").attribute("value").as_int() == BUILDING_DESTROYING) {
			//	buildingTemplate->
			//}
		}

		for (pugi::xml_node resourceNodeInfo = scene.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource")) {

			Resource* resourceTemplate = App->entityManager->CreateResource(resourceNodeInfo.child("Position").attribute("x").as_int(),
				resourceNodeInfo.child("Position").attribute("y").as_int(),
				(resourceItem)resourceNodeInfo.child("Type").attribute("value").as_int());

			resourceTemplate->Life = resourceNodeInfo.child("Life").attribute("value").as_int();
			//if (resourceNodeInfo.child("State").attribute("value").as_int() == RESOURCE_GATHERING) {
			//	resourceTemplate->
			//}
		}
	}

}
