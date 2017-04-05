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

Scene::Scene() : SceneElement("scene")
{
	blit_sections.push_back({ 0, 0, 220, 30 });
	blit_sections.push_back({ 0, 30, 220, 30 });
	detect_sections.push_back({ 0, 0, 220, 30 });
	blit_sections_menu.push_back({ 0, 0, 50, 19 });
	blit_sections_menu.push_back({ 50, 0, 50, 19 });
	detect_sections_menu.push_back({ 1330 , 5, 50, 19 });
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

	// LOADING FX


	// LOADING UI BB
	// ----------------------------------------
	// LOADING BUTTONS RECTS


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

	// RESOURCE LABELS
	wood = (Label*)App->gui->CreateLabel(to_string(woodCount), 50, 5, nullptr);
	wood->SetColor({ 255, 255, 255 ,255 });
	//*food, *gold, *stone, *villagers
	food = (Label*)App->gui->CreateLabel(to_string(foodCount), 150, 5, nullptr);
	food->SetColor({ 255, 255, 255 ,255 });

	gold = (Label*)App->gui->CreateLabel(to_string(goldCount), 280, 5, nullptr);
	gold->SetColor({ 255, 255, 255 ,255 });

	stone = (Label*)App->gui->CreateLabel(to_string(stoneCount), 360, 5, nullptr);
	stone->SetColor({ 255, 255, 255 ,255 });

	villagers = (Label*)App->gui->CreateLabel("0/0", 480, 5, nullptr);
	villagers->SetColor({ 255, 255, 255 ,255 });

	// SET UI PRIORITY

	App->gui->SetPriority();

	// MUSIC

	App->audio->PlayMusic("audio/music/m_scene.ogg");
	// ---------------------
	// DONE!


	//Test
	App->entityManager->CreateUnit(400, 1900, false, VILLAGER);
	UpdateVillagers(1, 1);

	troll = App->entityManager->CreateUnit(450, 2150, false, ELVEN_CAVALRY);
	troll->isHero = true;

	my_townCenter = App->entityManager->CreateBuilding(580, 1600, false, TOWN_CENTER);
	//enemy_townCenter = App->entityManager->CreateBuilding(-700, 500, true, ORC_BARRACKS);

	//App->fog->CreateFog(App->map->data.mapWidth, App->map->data.mapHeight);

	timer.Start();
	troll_timer.Start();

	Timer_lbl = (Label*)App->gui->CreateLabel("00:00", 665, 40, nullptr);
	Timer_lbl->SetColor({ 255, 255, 255, 255 });
	Timer_lbl->SetSize(26);

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
		debug = !debug;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		SaveScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		LoadScene();
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

	if (ui_menu.IsEnabled()) App->gui->Focus(ui_menu.FocusArea());

	if (menu_bt->current == HOVER || menu_bt->current == CLICKIN) App->gui->cursor->SetCursor(3);
	else App->gui->cursor->SetCursor(0);

	if (menu_bt->current == CLICKIN) {
		UpdateVillagers(5, 10);
		ui_menu.WindowOn();
	}
	if (quit_game_bt->current == CLICKIN) App->quit = true;
	else if (cancel_bt->current == CLICKIN) {
		ui_menu.WindowOff();
		App->gui->Unfocus();
	}
	UpdateTime(timer.ReadSec());
	TimeEvents();
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (back_to_menu_bt->current == CLICKIN) {
		App->audio->PlayFx(App->sceneManager->menu_scene->fx_button_click);
		App->sceneManager->ChangeScene(this, App->sceneManager->menu_scene);
	}
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	App->gui->DestroyALLUIElements();
	ui_menu.CleanUp();
	App->entityManager->CleanUp();
	return true;
}

void Scene::TimeEvents() {

	if ((int)timer.ReadSec() > 140) {
		Timer_lbl->SetColor({ 255, 0,0,255 });
	}

	if ((int)timer.ReadSec() == 140) {
		orc_timer.Start();
		wave = 2;
	}
	else if ((int)timer.ReadSec() < 140) {
		orc_timer.Start();
	}
	if ((int)troll_timer.ReadSec() == 300) {
		App->entityManager->CreateUnit(2400, 2100, true, TROLL_MAULER);
		troll_timer.Start();
	}
	if ((int)orc_timer.ReadSec() == 40)
	{
		orcs_to_spawn = wave;
		orc_timer.Start();
		if ((int)orcs_to_spawn > 0 && (int)spawn_timer.ReadSec() > 2) {
			+App->entityManager->CreateUnit(2400, 2100, true, ORC_SOLDIER);
			+orcs_to_spawn--;
			spawn_timer.Start();
		}
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
		if ((*it)->state != UNIT_DEAD) {
			pugi::xml_node unitNodeInfo = unitsNode.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->unitLife;
			unitNodeInfo.append_child("IsEnemy").append_attribute("value") = (*it)->isEnemy;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			destTileNode.append_attribute("x") = (*it)->path.back().x;
			destTileNode.append_attribute("y") = (*it)->path.back().y;
		}
	}

	for (list<Unit*>::iterator it = App->entityManager->enemyUnitList.begin(); it != App->entityManager->enemyUnitList.end(); it++) {
		if ((*it)->state != UNIT_DEAD) {
			pugi::xml_node unitNodeInfo = unitsNode.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->unitLife;
			unitNodeInfo.append_child("IsEnemy").append_attribute("value") = (*it)->isEnemy;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			destTileNode.append_attribute("x") = (*it)->path.back().x;
			destTileNode.append_attribute("y") = (*it)->path.back().y;
		}
	}

	pugi::xml_node buildingNode = rootNode.append_child("Buildings");
	for (list<Building*>::iterator it = App->entityManager->friendlyBuildingList.begin(); it != App->entityManager->friendlyBuildingList.end(); it++) {
		if ((*it)->state != BUILDING_DESTROYING) {
			pugi::xml_node buildingNodeInfo = buildingNode.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->buildingLife;
			buildingNodeInfo.append_child("IsEnemy").append_attribute("value") = (*it)->isEnemy;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			buildingNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
		}
	}

	for (list<Building*>::iterator it = App->entityManager->enemyBuildingList.begin(); it != App->entityManager->enemyBuildingList.end(); it++) {
		if ((*it)->state != BUILDING_DESTROYING) {
			pugi::xml_node buildingNodeInfo = buildingNode.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->buildingLife;
			buildingNodeInfo.append_child("IsEnemy").append_attribute("value") = (*it)->isEnemy;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			buildingNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
		}
	}

	pugi::xml_node resourcesNode = rootNode.append_child("Resources");
	for (list<Resource*>::iterator it = App->entityManager->resourceList.begin(); it != App->entityManager->resourceList.end(); it++) {
			pugi::xml_node resourceNodeInfo = resourcesNode.append_child("Resource");
			resourceNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = resourceNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			resourceNodeInfo.append_child("Life").append_attribute("value") = (*it)->resourceLife;
			resourceNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			resourceNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
			resourceNodeInfo.append_child("IndexRect").append_attribute("value") = (*it)->rectIndex;
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
				unitNodeInfo.child("IsEnemy").attribute("value").as_bool(),
				(unitType)unitNodeInfo.child("Type").attribute("value").as_int());

			unitTemplate->direction = (unitDirection)unitNodeInfo.child("Direction").attribute("value").as_int();
			unitTemplate->unitLife = unitNodeInfo.child("Life").attribute("value").as_int();
			unitTemplate->isVisible = unitNodeInfo.child("IsVisible").attribute("value").as_bool();
			if (unitNodeInfo.child("State").attribute("value").as_int() == UNIT_MOVING) {
				unitTemplate->SetDestination({ unitNodeInfo.child("DestinationTile").attribute("x").as_int(), unitNodeInfo.child("DestinationTile").attribute("y").as_int() });
			}
		}

		for (pugi::xml_node buildingNodeInfo = scene.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNodeInfo.child("Position").attribute("x").as_int(),
				buildingNodeInfo.child("Position").attribute("y").as_int(),
				buildingNodeInfo.child("IsEnemy").attribute("value").as_bool(),
				(buildingType)buildingNodeInfo.child("Type").attribute("value").as_int());

			buildingTemplate->buildingLife = buildingNodeInfo.child("Life").attribute("value").as_int();
			buildingTemplate->isVisible = buildingNodeInfo.child("IsVisible").attribute("value").as_bool();
			//if (buildingNodeInfo.child("State").attribute("value").as_int() == BUILDING_DESTROYING) {
			//	buildingTemplate->
			//}
		}

		for (pugi::xml_node resourceNodeInfo = scene.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource")) {

			Resource* resourceTemplate = App->entityManager->CreateResource(resourceNodeInfo.child("Position").attribute("x").as_int(),
				resourceNodeInfo.child("Position").attribute("y").as_int(),
				(resourceType)resourceNodeInfo.child("Type").attribute("value").as_int(),
				resourceNodeInfo.child("IndexRect").attribute("value").as_int());

			resourceTemplate->resourceLife = resourceNodeInfo.child("Life").attribute("value").as_int();
			resourceTemplate->isVisible = resourceNodeInfo.child("IsVisible").attribute("value").as_bool();
			//if (resourceNodeInfo.child("State").attribute("value").as_int() == RESOURCE_GATHERING) {
			//	resourceTemplate->
			//}
		}
	}

}
