#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"
#include "Application.h"
#include "Scene.h"
#include "Unit.h"
#include "Pathfinding.h"
#include "FogOfWar.h"
#include "Render.h"
#include "Textures.h"
#include "Resource.h"
#include "Collision.h"
#include "SceneManager.h"
#include "Hero.h"
#include "Villager.h"
#include <algorithm>

EntityManager::EntityManager() : Module()
{
	name = "entityManager";
	nextID = 1;
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node & config)
{

	return true;
}

bool EntityManager::Start()
{
	LOG("Starting EntityManager");

	bool ret = LoadGameData();
	click_timer.Start();

	return ret;
}

bool EntityManager::PreUpdate()
{
	return true;
}

bool EntityManager::Update(float dt)
{

	App->input->GetMousePosition(mouseX, mouseY);
	mouseX -= App->render->camera.x;
	mouseY -= App->render->camera.y;

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
	}
	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
		//App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
	}
	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
		//App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
	}

	// AI for enemies

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !selectedUnitList.empty()) {

		if (!placingBuilding) {
			iPoint destination = App->map->WorldToMap(mouseX, mouseY);

			Unit* commander = selectedUnitList.front();

			commander->SetDestination(destination);

			if (selectedUnitList.size() > 1) {

				selectedUnitList.pop_front();
				App->pathfinding->SharePath(commander, selectedUnitList);
				selectedUnitList.push_front(commander);

			}

			for (list<Unit*>::iterator it = selectedUnitList.begin(); it != selectedUnitList.end(); it++)
				(*it)->SetState(UNIT_MOVING);

			for (list<Villager*>::iterator it = constructors.begin(); it != constructors.end(); it++) {
				(*it)->constructingTarget = nullptr;
				constructors.remove(*it);
			}
		}
		else {
			placingBuilding = false;
		}
	}

	if (mouseY > NOTHUD.y - CAMERA_OFFSET_Y && mouseY < NOTHUD.h - CAMERA_OFFSET_Y) {

		switch (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
		{

		case KEY_DOWN:
			multiSelectionRect.x = mouseX;
			multiSelectionRect.y = mouseY;
			break;

		case KEY_REPEAT:
			multiSelectionRect.w = mouseX - multiSelectionRect.x;
			multiSelectionRect.h = mouseY - multiSelectionRect.y;
			break;

		case KEY_UP:

			selectedUnitList.clear();
			selectedBuildingList.clear();
			selectedResource = nullptr;

			if (multiSelectionRect.x == mouseX && multiSelectionRect.y == mouseY) {   // if there's no selection rect... (only clicked)

				iPoint mouse = { mouseX, mouseY };
				Collider* nearest_col = App->collision->FindNearestCollider(mouse);

				if (mouse.DistanceTo(nearest_col->pos) < nearest_col->r) {
					Unit* unit = nullptr;
					switch (nearest_col->type) {

					case COLLIDER_UNIT:
						
						unit = (Unit*)nearest_col->entity;
						if (click_timer.ReadSec() < 0.5 && nearest_col->entity == clicked_entity) { // double click

							for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {

								if (unit->type == (*it)->type)
									selectedUnitList.push_back((*it));
							}
							clicked_entity = nullptr;
						}
						else {   // single click
							selectedUnitList.push_back(unit);
							clicked_entity = nearest_col->entity;
						}
						break;
					case COLLIDER_BUILDING:
						selectedBuildingList.push_back((Building*)clicked_entity);
						clicked_entity = nearest_col->entity;
						break;
					case COLLIDER_RESOURCE:
						selectedResource = (Resource*)clicked_entity;
						clicked_entity = nearest_col->entity;
						break;
					}
				}

				click_timer.Start();
			}
			else {    // if there's selection rect (clicked and dragged)

				int selectedCount = 0;
				clicked_entity = nullptr;

				if (multiSelectionRect.w < 0) {
					multiSelectionRect.x += multiSelectionRect.w;
					multiSelectionRect.w *= -1;
				}
				if (multiSelectionRect.h < 0) {
					multiSelectionRect.y += multiSelectionRect.h;
					multiSelectionRect.h *= -1;
				}


				for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
					SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

					if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
						selectedUnitList.push_back((*it));
						selectedCount++;
					}
				}

				if (selectedCount == 0) {

					for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
						SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

						if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
							selectedBuildingList.push_back((*it));
							selectedCount++;
						}
					}
				}

				if (selectedCount == 0) {

					for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
						SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

						if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
							selectedUnitList.push_back((*it));
							selectedCount++;
						}
					}
				}

				if (selectedCount == 0) {

					for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
						SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

						if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
							selectedBuildingList.push_back((*it));
							selectedCount++;
						}
					}
				}

				/*if (selectedCount == 0) {

					for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
						SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

						if ((bool)SDL_PointInRect(&pos, &multiSelectionRect))
							resourceList.push_back((*it));
					}
				}*/

				multiSelectionRect = { 0,0,0,0 };
				break;
			}
		}
	}

	for (list<Unit*>::iterator it = selectedUnitList.begin(); it != selectedUnitList.end(); it++) {
		if ((*it)->state == UNIT_DEAD) 
			selectedUnitList.erase(it);
	}

	if (multiSelectionRect.w != 0) {
		Sprite square;

		square.rect = multiSelectionRect;
		square.priority = 100000;
		square.r = 255;
		square.g = 255;
		square.b = 255;
		square.filled = false;
		App->render->sprites_toDraw.push_back(square);
	}


	if (placingBuilding) {
		placingBuildingSprite.pos = { mouseX - placingBuildingSprite.rect.w / 2, mouseY - placingBuildingSprite.rect.h / 2 };
		App->render->sprites_toDraw.push_back(placingBuildingSprite);
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			iPoint dest = App->map->WorldToMap(mouseX, mouseY);
			for (list<Unit*>::iterator it = selectedUnitList.begin(); it != selectedUnitList.end(); it++) {
				if ((*it)->type == VILLAGER || (*it)->type == ELF_VILLAGER) {
					Villager* villager = (Villager*)*it;
					villager->SetDestination(dest);
					villager->SetState(UNIT_MOVING);
					constructors.push_back(villager);
				}
			}
			placingBuilding = false;
		}
	}

	if (constructors.size() > 0) {
		for (list<Villager*>::iterator it = constructors.begin(); it != constructors.end(); it++) {
			iPoint buildingCenterPos(placingBuildingSprite.pos.x + (placingBuildingSprite.rect.w / 2), placingBuildingSprite.pos.y + (placingBuildingSprite.rect.h / 2));
			if (buildingCenterPos.DistanceTo((*it)->entityPosition) < 60) {
				if ((*it)->constructingTarget == nullptr) {
					Building* building = CreateBuilding(buildingCenterPos.x, buildingCenterPos.y, creatingBuildingType);
					building->Life = 1;
					building->faction = FREE_MEN; //just for testing;
					for (list<Villager*>::iterator it2 = constructors.begin(); it2 != constructors.end(); it2++) {
						(*it)->constructingTarget = building;
					}
				}
				(*it)->SetState(UNIT_BUILDING);
				constructors.remove(*it);
			}
		}
	}

	return true;

}

bool EntityManager::PostUpdate()
{
	if (removeUnitList.size() > 0) {
		list<Unit*>::iterator i = removeUnitList.begin();

		while (i != removeUnitList.end())
		{
			list<Unit*>::iterator unitToDestroy = i;
			++i;
			DestroyEntity((*unitToDestroy));
		}

		removeUnitList.clear();
	}

	if (removeBuildingList.size() > 0) {
		list<Building*>::iterator i = removeBuildingList.begin();

		while (i != removeBuildingList.end())
		{
			list<Building*>::iterator buildingToDestroy = i;
			++i;
			DestroyEntity((*buildingToDestroy));
		}

		removeBuildingList.clear();
	}

	if (removeResourceList.size() > 0) {
		list<Resource*>::iterator i = removeResourceList.begin();

		while (i != removeResourceList.end())
		{
			list<Resource*>::iterator resourceToDestroy = i;
			++i;
			DestroyEntity((*resourceToDestroy));
		}

		removeResourceList.clear();
	}

	return true;
}

bool EntityManager::CleanUp()
{
	LOG("Freeing EntityManager");

	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		RELEASE((*it));
	}
	friendlyUnitList.clear();

	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		RELEASE((*it));
	}
	enemyUnitList.clear();

	for (list<Unit*>::iterator it = removeUnitList.begin(); it != removeUnitList.end(); it++) {
		RELEASE((*it));
	}
	removeUnitList.clear();

	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		RELEASE((*it));
	}
	friendlyBuildingList.clear();

	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		RELEASE((*it));
	}
	enemyBuildingList.clear();

	for (list<Building*>::iterator it = removeBuildingList.begin(); it != removeBuildingList.end(); it++) {
		RELEASE((*it));
	}
	removeBuildingList.clear();

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		RELEASE((*it));
	}
	resourceList.clear();

	for (list<Resource*>::iterator it = removeResourceList.begin(); it != removeResourceList.end(); it++) {
		RELEASE((*it));
	}
	removeResourceList.clear();

	return true;
}

bool EntityManager::IsOccupied(iPoint tile, iPoint ignore_tile) {

	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		if (tile == App->map->WorldToMap((*it)->entityPosition.x, (*it)->entityPosition.y)) {
			if (ignore_tile != tile)
				return true;
		}
	}

	return false;

}

bool EntityManager::LoadGameData()
{
	bool ret = false;
	pugi::xml_document gameDataFile;
	pugi::xml_node gameData;
	pugi::xml_node unitNodeInfo;
	pugi::xml_node buildingNodeInfo;
	pugi::xml_node resourceNodeInfo;


	gameData = App->LoadGameDataFile(gameDataFile);

	if (gameData.empty() == false)
	{
		ret = true;

		for (unitNodeInfo = gameData.child("Units").child("Unit"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Unit")) {

			Unit* unitTemplate;
			unitType type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();

			if (type == VILLAGER || type == ELF_VILLAGER)
				unitTemplate = (Unit*) new Villager();
			else if (type == GONDOR_HERO)
				unitTemplate = (Unit*) new Hero();
			else
				unitTemplate = new Unit();

			unitTemplate->type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();

			string idleTexturePath = unitNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string moveTexturePath = unitNodeInfo.child("Textures").child("Move").attribute("value").as_string();
			string attackTexturePath = unitNodeInfo.child("Textures").child("Attack").attribute("value").as_string();
			string dieTexturePath = unitNodeInfo.child("Textures").child("Die").attribute("value").as_string();


			unitTemplate->faction = (Faction)unitNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			unitTemplate->attackSpeed = 1 / unitNodeInfo.child("Stats").child("AttackSpeed").attribute("value").as_float();
			unitTemplate->Life = unitNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			unitTemplate->MaxLife = unitTemplate->Life;
			unitTemplate->Attack = unitNodeInfo.child("Stats").child("Attack").attribute("value").as_int();
			unitTemplate->Defense = unitNodeInfo.child("Stats").child("Defense").attribute("value").as_int();
			unitTemplate->unitPiercingDamage = unitNodeInfo.child("Stats").child("PiercingDamage").attribute("value").as_int();
			unitTemplate->unitMovementSpeed = unitNodeInfo.child("Stats").child("MovementSpeed").attribute("value").as_float();
			unitTemplate->unitRange = unitNodeInfo.child("Stats").child("Range").attribute("value").as_int();
			unitTemplate->unitRangeOffset = unitNodeInfo.child("Stats").child("RangeOffset").attribute("value").as_int();

			pugi::xml_node animationNode;
			int width;
			int height;
			int rows;
			int columns;

			//Anim Idle//
			animationNode = unitNodeInfo.child("Animations").child("Idle");
			width = animationNode.child("Width").attribute("value").as_int();
			height = animationNode.child("Height").attribute("value").as_int();
			rows = animationNode.child("Rows").attribute("value").as_int();
			columns = animationNode.child("Columns").attribute("value").as_int();
			for (int i = 0; i < rows; i++) {
				Animation idle;
				for (int j = 0; j < columns; j++) {
					idle.PushBack({ width*j,height*i,width,height });
				}
				idle.speed = animationNode.child("Speed").attribute("value").as_float();
				unitTemplate->idleAnimations.push_back(idle);
				if (i != 0 && i != rows - 1) {
					idle.flip = SDL_FLIP_HORIZONTAL;
					unitTemplate->idleAnimations.push_back(idle);
				}
			}

			//Anim Moving//
			animationNode = unitNodeInfo.child("Animations").child("Move");
			width = animationNode.child("Width").attribute("value").as_int();
			height = animationNode.child("Height").attribute("value").as_int();
			rows = animationNode.child("Rows").attribute("value").as_int();
			columns = animationNode.child("Columns").attribute("value").as_int();
			for (int i = 0; i < rows; i++) {
				Animation move;
				for (int j = 0; j < columns; j++) {
					move.PushBack({ width*j,height*i,width,height });
				}
				move.speed = animationNode.child("Speed").attribute("value").as_float();
				unitTemplate->movingAnimations.push_back(move);
				if (i != 0 && i != rows - 1) {
					move.flip = SDL_FLIP_HORIZONTAL;
					unitTemplate->movingAnimations.push_back(move);
				}
			}

			//Anim Attacking//
			animationNode = unitNodeInfo.child("Animations").child("Attack");
			width = animationNode.child("Width").attribute("value").as_int();
			height = animationNode.child("Height").attribute("value").as_int();
			rows = animationNode.child("Rows").attribute("value").as_int();
			columns = animationNode.child("Columns").attribute("value").as_int();
			for (int i = 0; i < rows; i++) {
				Animation attack;
				for (int j = 0; j < columns; j++) {
					attack.PushBack({ width*j,height*i,width,height });
				}
				attack.speed = animationNode.child("Speed").attribute("value").as_float();
				unitTemplate->attackingAnimations.push_back(attack);
				if (i != 0 && i != rows - 1) {
					attack.flip = SDL_FLIP_HORIZONTAL;
					unitTemplate->attackingAnimations.push_back(attack);
				}
			}

			//Anim Dying//
			animationNode = unitNodeInfo.child("Animations").child("Die");
			width = animationNode.child("Width").attribute("value").as_int();
			height = animationNode.child("Height").attribute("value").as_int();
			rows = animationNode.child("Rows").attribute("value").as_int();
			columns = animationNode.child("Columns").attribute("value").as_int();
			for (int i = 0; i < rows; i++) {
				Animation die;
				for (int j = 0; j < columns; j++) {
					die.PushBack({ width*j,height*i,width,height });
				}
				die.speed = animationNode.child("Speed").attribute("value").as_float();
				unitTemplate->dyingAnimations.push_back(die);
				if (i != 0 && i != rows - 1) {
					die.flip = SDL_FLIP_HORIZONTAL;
					unitTemplate->dyingAnimations.push_back(die);
				}
			}

			unitTemplate->unitIdleTexture = App->tex->Load(idleTexturePath.c_str());
			unitTemplate->unitMoveTexture = App->tex->Load(moveTexturePath.c_str());
			unitTemplate->unitAttackTexture = App->tex->Load(attackTexturePath.c_str());
			unitTemplate->unitDieTexture = App->tex->Load(dieTexturePath.c_str());

			if (unitTemplate->type == VILLAGER || unitTemplate->type == ELF_VILLAGER) {
				//Villager
				string chopTexturePath;
				chopTexturePath = unitNodeInfo.child("Textures").child("Cut").attribute("value").as_string();
				
				Villager* villagerTemplate = (Villager*)unitTemplate;

				//Anim Chopping//
				animationNode = unitNodeInfo.child("Animations").child("Chop");
				width = animationNode.child("Width").attribute("value").as_int();
				height = animationNode.child("Height").attribute("value").as_int();
				rows = animationNode.child("Rows").attribute("value").as_int();
				columns = animationNode.child("Columns").attribute("value").as_int();
				for (int i = 0; i < rows; i++) {
					Animation chop;
					for (int j = 0; j < columns; j++) {
						chop.PushBack({ width*j,height*i,width,height });
					}
					chop.speed = animationNode.child("Speed").attribute("value").as_float();
					villagerTemplate->choppingAnimations.push_back(chop);
					if (i != 0 && i != rows - 1) {
						chop.flip = SDL_FLIP_HORIZONTAL;
						villagerTemplate->choppingAnimations.push_back(chop);
					}
				}
				villagerTemplate->unitChoppingTexture = App->tex->Load(chopTexturePath.c_str());

				villagerTemplate->gathering_speed = unitNodeInfo.child("Stats").child("GatheringSpeed").attribute("value").as_int();
				villagerTemplate->max_capacity = unitNodeInfo.child("Stats").child("MaxCapacity").attribute("value").as_int();

			}

			if (unitTemplate->type == GONDOR_HERO) {

				Hero* heroTemplate = (Hero*)unitTemplate;
				heroTemplate->skill->type = (Skill_type)unitNodeInfo.child("Stats").child("SkillType").attribute("value").as_int();;
			}

			unitsDB.insert(pair<int, Unit*>(unitTemplate->type, unitTemplate));
		}

		for (buildingNodeInfo = gameData.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = new Building();

			string idleTexturePath = buildingNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string dieTexturePath = buildingNodeInfo.child("Textures").child("Die").attribute("value").as_string();

			buildingTemplate->Life = buildingNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			buildingTemplate->buildingWoodCost = buildingNodeInfo.child("Stats").child("WoodCost").attribute("value").as_int();
			buildingTemplate->buildingStoneCost = buildingNodeInfo.child("Stats").child("StoneCost").attribute("value").as_int();
			buildingTemplate->buildingBuildTime = buildingNodeInfo.child("Stats").child("BuildTime").attribute("value").as_int();
			buildingTemplate->canAttack = buildingNodeInfo.child("Stats").child("CanAttack").attribute("value").as_bool();

			buildingTemplate->buildingIdleTexture = App->tex->Load(idleTexturePath.c_str());
			buildingTemplate->buildingDieTexture = App->tex->Load(dieTexturePath.c_str());

			buildingTemplate->type = (buildingType)buildingNodeInfo.child("Info").child("ID").attribute("value").as_int();

			buildingsDB.insert(pair<int, Building*>(buildingTemplate->type, buildingTemplate));
			buildingTemplate->MaxLife = buildingTemplate->Life;
		}

		for (resourceNodeInfo = gameData.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource"))
		{

			Resource* resourceTemplate = new Resource();

			string idleTexturePath = resourceNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string gatheringTexturePath = resourceNodeInfo.child("Textures").child("Gathering").attribute("value").as_string();

			resourceTemplate->Life = resourceNodeInfo.child("Stats").child("Life").attribute("value").as_int();

			for (pugi::xml_node rectsNode = resourceNodeInfo.child("Rects").child("Rect"); rectsNode; rectsNode = rectsNode.next_sibling("Rect")) {
				resourceTemplate->resourceRectVector.push_back({ rectsNode.attribute("x").as_int(), rectsNode.attribute("y").as_int(), rectsNode.attribute("w").as_int(), rectsNode.attribute("h").as_int() });
			}

			resourceTemplate->resourceIdleTexture = App->tex->Load(idleTexturePath.c_str());
			resourceTemplate->resourceGatheringTexture = App->tex->Load(gatheringTexturePath.c_str());

			resourceTemplate->visual = (resourceItem)resourceNodeInfo.child("Info").child("ID").attribute("value").as_int();
			resourceTemplate->type = (resourceType)resourceNodeInfo.child("Info").child("Type").attribute("value").as_int();

			resourcesDB.insert(pair<int, Resource*>(resourceTemplate->visual, resourceTemplate));
		}
	}

	return ret;
}

Unit* EntityManager::CreateUnit(int posX, int posY, unitType type)
{
	Unit* unit;
	if (type == VILLAGER || type == ELF_VILLAGER)
		unit = (Unit*) new Villager(posX, posY, (Villager*)unitsDB[type]);
	else if (type == GONDOR_HERO)
		unit = (Unit*) new Hero(posX, posY, (Hero*)unitsDB[type]);
	else
		unit = new Unit(posX, posY, unitsDB[type]);

	unit->entityID = nextID;
	nextID++;

	if (unit->faction == FREE_MEN) 
		friendlyUnitList.push_back(unit);
	else 
		enemyUnitList.push_back(unit);
	


	return unit;
}

void EntityManager::PlaceBuilding(buildingType type)
{
	placingBuildingSprite.texture = buildingsDB[type]->buildingIdleTexture;
	placingBuilding = true;
	uint width, height;
	App->tex->GetSize(placingBuildingSprite.texture, width, height);
	placingBuildingSprite.rect = { 0,0,(int)width,(int)height };
	placingBuildingSprite.priority = 12500;
	creatingBuildingType = type;
}

Building* EntityManager::CreateBuilding(int posX, int posY,  buildingType type)
{
	Building* building = new Building(posX, posY, buildingsDB[type]);
	building->entityID = nextID;
	nextID++;
	if (building->faction == FREE_MEN)
		friendlyBuildingList.push_back(building);
	else 
		enemyBuildingList.push_back(building);

	return building;
}

Resource* EntityManager::CreateResource(int posX, int posY, resourceItem item)
{
	iPoint ret;
	ret.x = (posX - posY);
	ret.y = (posX + posY) / 2;
	Resource* resource = new Resource(ret.x, ret.y, resourcesDB[item]);
	resource->entityID = nextID;
	nextID++;
	resourceList.push_back(resource);

	return resource;
}

void EntityManager::DeleteUnit(Unit* unit)
{
	if (unit != nullptr) {
		removeUnitList.push_back(unit);
		if (unit->faction == FREE_MEN) 
			friendlyUnitList.remove(unit);
		else 
			enemyUnitList.remove(unit);
		
	}
}

void EntityManager::DeleteBuilding(Building* building)
{
	if (building != nullptr) {
		removeBuildingList.push_back(building);
		if (building->faction == FREE_MEN)
			friendlyBuildingList.remove(building);
		else 
			enemyBuildingList.remove(building);
		
	}
}

void EntityManager::DeleteResource(Resource* resource)
{
	if (resource != nullptr) {
		removeResourceList.push_back(resource);
		resourceList.remove(resource);
	}
}

void EntityManager::OnCollision(Collision_data& col_data)
{

	//possible colliding elements
	Unit* unit = col_data.c1->GetUnit();
	Unit* unit2 = nullptr;
	Building* building = nullptr;
	Resource* resource = nullptr;

	col_data.c1->colliding = true;
	col_data.c2->colliding = true;
	col_data.state = SOLVING;

	switch (col_data.c2->type) {

	case COLLIDER_RANGE:

		if (col_data.c2->entity->faction != unit->faction) {

			if (unit2 = col_data.c2->GetUnit()) {

				if (unit2->attackTarget == nullptr) {
					unit2->attackTarget = unit;
					unit2->SetState(UNIT_ATTACKING);
				}
				else
					col_data.state = UNSOLVED;
			}
			else if (building = col_data.c2->GetBuilding()) {

				if (building->attackTarget == nullptr) {
					building->attackTarget = unit;
					building->state = BUILDING_ATTACKING;
				}
				else
					col_data.state = UNSOLVED;
			}
		}
		break;

	case COLLIDER_UNIT:

		if (col_data.c2->entity->faction == unit->faction) {
			unit2 = col_data.c2->GetUnit();
			col_data.state = App->pathfinding->SolveCollision(unit, unit2);// first parameter should be the higher priority unit!
		}
		break;

	case COLLIDER_BUILDING:
		
		building = col_data.c2->GetBuilding();

		if (unit->IsVillager && building->type == TOWN_CENTER) {

			Villager* villager = (Villager*)unit;
			if (villager->curr_capacity > 0) {

				switch (villager->resource_carried) {
				case WOOD:
					App->sceneManager->level1_scene->woodCount += villager->curr_capacity;
					App->sceneManager->level1_scene->wood->SetString(to_string(App->sceneManager->level1_scene->woodCount));
					break;
				}

				villager->curr_capacity = 0;

				villager->SetDestination(FindNearestResource(villager->resource_carried, villager->entityPosition));
				villager->SetState(UNIT_MOVING);
			}
		}

		break;

	case COLLIDER_RESOURCE:

		resource = col_data.c2->GetResource();

		if (unit->IsVillager && unit->state == UNIT_IDLE) {
			Villager* villager = (Villager*)unit;
			if (villager->curr_capacity == 0) {
				villager->resource_carried = resource->type;
				villager->attackTarget = resource;
				villager->LookAt();
				villager->SetState(UNIT_GATHERING);
				resource->Damaged();
			}
			else {
				iPoint destination = App->map->WorldToMap(TOWN_HALL_POS_X, TOWN_HALL_POS_Y);
				villager->SetDestination(destination);
				villager->SetState(UNIT_MOVING);
			}
		}

		break;

	default:
		break;
	}

}



iPoint EntityManager::FindNearestResource(resourceType type, iPoint pos) {

	iPoint ret(-1, -1);

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		if ((*it)->type == type) {
			if (pos.DistanceTo((*it)->entityPosition) < pos.DistanceTo(ret))
				ret = (*it)->entityPosition;
		}
	}

	iPoint ret_map = App->map->WorldToMap(ret.x, ret.y);
	return ret_map;
}


void EntityManager::DestroyEntity(Entity* entity)
{
	if (entity != nullptr) {
		list<Unit*>::iterator it = removeUnitList.begin();

		while (it != removeUnitList.end())
		{
			if (*it == entity)
			{
				removeUnitList.remove(*it);
				delete entity;
				return;
			}
			++it;
		}
	}
}
