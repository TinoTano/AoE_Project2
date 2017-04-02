#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"
#include "Application.h"
#include "Scene.h"
#include "Unit.h"
#include "FogOfWar.h"
#include "Render.h"
#include "Textures.h"

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
	if (ret) {
		App->scene->Start();
	}
	
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

	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
		App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
		App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}	

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !selectedUnitList.empty()) {

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

	}
	
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

		bool selectedCount = 0;

		if (multiSelectionRect.w < 0) {
			multiSelectionRect.x += multiSelectionRect.w;
			multiSelectionRect.w *= -1;
		}
		if (multiSelectionRect.h < 0) {
			multiSelectionRect.y += multiSelectionRect.h;
			multiSelectionRect.h *= -1;
		}

		for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
			SDL_Point pos;
			pos.x = (*it)->entityPosition.x;
			pos.y = (*it)->entityPosition.y;

			if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
				selectedCount++;
				selectedUnitList.push_back((*it));
			}
			
		}

		if (selectedCount == 0) {

			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				SDL_Point pos;
				pos.x = (*it)->entityPosition.x;
				pos.y = (*it)->entityPosition.y;

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect))
					selectedUnitList.push_back((*it));

			}
		}

		if (doubleClickTimer <= 0.5f) {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if (!selectedUnitList.empty()) {
					if ((*it) != selectedUnitList.front()) {

						if ((*it)->GetType() == selectedUnitList.front()->GetType()) 
							selectedUnitList.push_back((*it));
						
					}
				}
			}
			doubleClickTimer = 0;
		}

		multiSelectionRect = { 0,0,0,0 };
		break;
	}

	for (list<Unit*>::iterator it = selectedUnitList.begin(); it != selectedUnitList.end(); it++) {

		Unit* unit = (*it);

		if (unit->state == UNIT_DEAD) {
			selectedUnitList.erase(it);
			continue;
		}
		int percent = ((unit->MaxLife - unit->Life) * 100) / unit->MaxLife;
		int barPercent = (percent * unit->hpBarWidth) / 100;
		int hpbar_y_position = unit->entityPosition.y - ((unit->collider->pos.y - unit->entityPosition.y)* 1.5f) ;
		App->render->DrawCircle(unit->collider->pos.x, unit->collider->pos.y, 15, 255, 255, 255, 255);
		App->render->DrawQuad({ unit->entityPosition.x - (unit->hpBarWidth / 2), hpbar_y_position, unit->hpBarWidth, 5 }, 255, 0, 0);
		App->render->DrawQuad({ unit->entityPosition.x - (unit->hpBarWidth / 2), hpbar_y_position, min(unit->hpBarWidth, max(unit->hpBarWidth - barPercent , 0)), 5 }, 0, 255, 0);
	}

	if (multiSelectionRect.w != 0) 
		App->render->DrawQuad(multiSelectionRect, 255, 255, 255, 255, false);
	
	doubleClickTimer += dt;

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
		if (tile == App->map->WorldToMap((*it)->entityPosition.x, (*it)->entityPosition.y)){
			if(ignore_tile != tile)
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

			Unit* unitTemplate = new Unit();

			string idleTexturePath = unitNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string moveTexturePath = unitNodeInfo.child("Textures").child("Move").attribute("value").as_string();
			string attackTexturePath = unitNodeInfo.child("Textures").child("Attack").attribute("value").as_string();
			string dieTexturePath = unitNodeInfo.child("Textures").child("Die").attribute("value").as_string();

			unitTemplate->faction = (unitFaction)unitNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			unitTemplate->attackSpeed = 1 / unitNodeInfo.child("Stats").child("AttackSpeed").attribute("value").as_float();
			unitTemplate->unitLife = unitNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			unitTemplate->unitMaxLife = unitTemplate->unitLife;
			unitTemplate->unitAttack = unitNodeInfo.child("Stats").child("Attack").attribute("value").as_int();
			unitTemplate->unitDefense = unitNodeInfo.child("Stats").child("Defense").attribute("value").as_int();
			unitTemplate->unitPiercingDamage = unitNodeInfo.child("Stats").child("PiercingDamage").attribute("value").as_int();
			unitTemplate->unitMovementSpeed = unitNodeInfo.child("Stats").child("MovementSpeed").attribute("value").as_float();

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

			unitTemplate->type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();

			unitsDB.insert(pair<int, Unit*>(unitTemplate->type, unitTemplate));
		}

		for (buildingNodeInfo = gameData.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {
			
			Building* buildingTemplate = new Building();

			string idleTexturePath = buildingNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string dieTexturePath = buildingNodeInfo.child("Textures").child("Die").attribute("value").as_string();

			buildingTemplate->buildingLife = buildingNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			buildingTemplate->buildingWoodCost = buildingNodeInfo.child("Stats").child("WoodCost").attribute("value").as_int();
			buildingTemplate->buildingStoneCost = buildingNodeInfo.child("Stats").child("StoneCost").attribute("value").as_int();
			buildingTemplate->buildingBuildTime = buildingNodeInfo.child("Stats").child("BuildTime").attribute("value").as_int();
			buildingTemplate->canAttack = buildingNodeInfo.child("Stats").child("CanAttack").attribute("value").as_bool();
			buildingTemplate->buildingIdleTexture = App->tex->Load(idleTexturePath.c_str());
			buildingTemplate->buildingDieTexture = App->tex->Load(dieTexturePath.c_str());
			buildingTemplate->type = (buildingType)buildingNodeInfo.child("Info").child("ID").attribute("value").as_int();
			buildingsDB.insert(pair<int, Building*>(buildingTemplate->type, buildingTemplate));

			// My changes ------------------------

			buildingTemplate->buildingMaxLife = buildingTemplate->buildingLife;

			// -----------------------------------
		}

		for (resourceNodeInfo = gameData.child("Resources").child("Resouce"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resouce")) {

			Resource* resourceTemplate = new Resource();

			string idleTexturePath = resourceNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string gatheringTexturePath = resourceNodeInfo.child("Textures").child("Gathering").attribute("value").as_string();

			resourceTemplate->resourceLife = resourceNodeInfo.child("Stats").child("Life").attribute("value").as_int();

			resourceTemplate->resourceIdleTexture = App->tex->Load(idleTexturePath.c_str());
			resourceTemplate->resourceGatheringTexture = App->tex->Load(gatheringTexturePath.c_str());

			resourceTemplate->type = (resourceType)resourceNodeInfo.child("Info").child("ID").attribute("value").as_int();

			resourcesDB.insert(pair<int, Resource*>(resourceTemplate->type, resourceTemplate));
		}
	}

	return ret;
}

Unit* EntityManager::CreateUnit(int posX, int posY, bool isEnemy, unitType type)
{
	Unit* unit = new Unit(posX, posY, isEnemy, unitsDB[type]);
	unit->entityID = nextID;
	nextID++;
	if (!isEnemy) {
		friendlyUnitList.push_back(unit);
	}
	else {
		enemyUnitList.push_back(unit);
	}


	return unit;
}

Building* EntityManager::CreateBuilding(int posX, int posY, bool isEnemy, buildingType type)
{
	Building* building = new Building(posX, posY, isEnemy, buildingsDB[type]);
	building->entityID = nextID;
	nextID++;
	if (!isEnemy) {
		friendlyBuildingList.push_back(building);
	}
	else {
		enemyBuildingList.push_back(building);
	}


	return building;
}

Resource* EntityManager::CreateResource(int posX, int posY, resourceType type)
{
	Resource* resource = new Resource(posX, posY, resourcesDB[type]);
	resource->entityID = nextID;
	nextID++;
	resourceList.push_back(resource);

	return resource;
}

void EntityManager::DeleteUnit(Unit* unit, bool isEnemy)
{
	if (unit != nullptr) {
		removeUnitList.push_back(unit);
		if (isEnemy) {
			enemyUnitList.remove(unit);
		}
		else {
			friendlyUnitList.remove(unit);
		}
	}
}

void EntityManager::DeleteBuilding(Building* building, bool isEnemy)
{
	if (building != nullptr) {
		removeBuildingList.push_back(building);
		if (isEnemy) {
			enemyBuildingList.remove(building);
		}
		else {
			friendlyBuildingList.remove(building);
		}
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

	Unit* unit = nullptr;
	Unit* unit2 = nullptr;
	Building* building = nullptr;

	col_data.c1->colliding = true;
	col_data.c2->colliding = true;
	
	if (col_data.c1->type == COLLIDER_UNIT) {

		unit = col_data.c1->GetUnit();

		switch (col_data.c2->type) {

		case COLLIDER_UNIT:

			unit2 = col_data.c2->GetUnit();

			if (unit2->faction != unit->faction) {
				if (unit->attackTarget == nullptr) {
					unit->attackTarget = unit2;
					unit->SetState(UNIT_ATTACKING);
				}

				if (unit2->attackTarget == nullptr) {
					unit2->attackTarget = unit;
					unit2->SetState(UNIT_ATTACKING);
				}

				col_data.state = SOLVING;
			}
			else             
				col_data.state = App->pathfinding->SolveCollision(unit, unit2);// first parameter should be the higher priority unit!
			
			break;

		case COLLIDER_BUILDING:			
       building = col_data.c2->GetBuilding();

			if (building->faction != unit->faction) {
				if (unit->attackTarget == nullptr) {
					unit->attackTarget = building;
					unit->SetState(UNIT_ATTACKING);
				}

				if (building->canAttack && building->attackTarget == nullptr) {
					building->attackTarget = unit;
					building->state = BUILDING_ATTACKING;
				}
			}
			col_data.state = SOLVING;

			break;
        
case COLLIDER_RESOURCE:

			col_data.state = SOLVED;

			break;

		default:
			break;

		}
	}
}

void EntityManager::DestroyEntity(Entity * entity)
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


