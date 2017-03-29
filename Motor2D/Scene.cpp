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

Scene::Scene() : Module()
{
	name = "scene";
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
	if (App->map->Load("rivendell.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	//Test
	App->entityManager->CreateUnit(350, 350, ELVEN_ARCHER);
	App->entityManager->CreateUnit(100, 500, ELVEN_ARCHER);
	App->entityManager->CreateUnit(320, 350, ELVEN_ARCHER);
	App->entityManager->CreateUnit(100, 520, ELVEN_ARCHER);
	App->entityManager->CreateUnit(380, 350, ELVEN_ARCHER);
	App->entityManager->CreateUnit(140, 500, ELVEN_ARCHER);
	App->entityManager->CreateUnit(600, 400, TROLL_MAULER);
	App->entityManager->CreateBuilding(150, 100, ORC_BARRACKS);
	App->entityManager->CreateResource(400, 400, BLACK_TREE, 5);

	App->fog->CreateFog(App->map->data.mapWidth, App->map->data.mapHeight);
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
	
	list<iPoint> path = App->pathfinding->GetPath();

	if (debug && !path.empty()) {

		for (list<iPoint>::const_iterator it = path.begin(); it != path.end(); it++) {
			iPoint pos = App->map->MapToWorld((*it).x, (*it).y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}
	

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
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
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Faction").append_attribute("value") = (*it)->faction;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			destTileNode.append_attribute("x") = (*it)->path->back().x;
			destTileNode.append_attribute("y") = (*it)->path->back().y;
		}
	}

	for (list<Unit*>::iterator it = App->entityManager->enemyUnitList.begin(); it != App->entityManager->enemyUnitList.end(); it++) {
		if ((*it)->state != UNIT_DEAD) {
			pugi::xml_node unitNodeInfo = unitsNode.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Faction").append_attribute("value") = (*it)->faction;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("IsVisible").append_attribute("value") = (*it)->isVisible;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			destTileNode.append_attribute("x") = (*it)->path->back().x;
			destTileNode.append_attribute("y") = (*it)->path->back().y;
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
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("Faction").append_attribute("value") = (*it)->faction;
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
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("Faction").append_attribute("value") = (*it)->faction;
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
			resourceNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
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
				(unitType)unitNodeInfo.child("Type").attribute("value").as_int());

			unitTemplate->faction = (Faction)unitNodeInfo.child("Faction").attribute("value").as_int(),
			unitTemplate->direction = (unitDirection)unitNodeInfo.child("Direction").attribute("value").as_int();
			unitTemplate->Life = unitNodeInfo.child("Life").attribute("value").as_int();
			unitTemplate->isVisible = unitNodeInfo.child("IsVisible").attribute("value").as_bool();
			if (unitNodeInfo.child("State").attribute("value").as_int() == UNIT_MOVING) {
				unitTemplate->SetDestination({ unitNodeInfo.child("DestinationTile").attribute("x").as_int(), unitNodeInfo.child("DestinationTile").attribute("y").as_int() });
			}
		}

		for (pugi::xml_node buildingNodeInfo = scene.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNodeInfo.child("Position").attribute("x").as_int(),
				buildingNodeInfo.child("Position").attribute("y").as_int(),
				(buildingType)buildingNodeInfo.child("Type").attribute("value").as_int());


			buildingTemplate->faction = (Faction)buildingNodeInfo.child("Faction").attribute("value").as_int(),
			buildingTemplate->Life = buildingNodeInfo.child("Life").attribute("value").as_int();
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

			resourceTemplate->Life = resourceNodeInfo.child("Life").attribute("value").as_int();
			resourceTemplate->isVisible = resourceNodeInfo.child("IsVisible").attribute("value").as_bool();
			//if (resourceNodeInfo.child("State").attribute("value").as_int() == RESOURCE_GATHERING) {
			//	resourceTemplate->
			//}
		}
	}

}
