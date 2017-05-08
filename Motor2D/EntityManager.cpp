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
#include "Orders.h"
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

bool EntityManager::Update(float arg_dt)
{
	dt = arg_dt;

	App->input->GetMousePosition(mouseX, mouseY);
	mouseX -= App->render->camera.x;
	mouseY -= App->render->camera.y;

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition) && (*it)->isActive)
			(*it)->Draw();
	}
	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition))
			(*it)->Draw();
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
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
		if (App->render->CullingCam((*it)->entityPosition) && (*it)->isActive)
			(*it)->Draw();
	}

	for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
		if ((*it)->state == DESTROYED)
			selectedEntityList.erase(it);
	}

	// AI for enemies
	Unit* unit = nullptr;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !selectedEntityList.empty() && selectedListType == COLLIDER_UNIT) {

		for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
			Unit* unit = (Unit*)(*it);
			for (list<Order*>::iterator it2 = unit->order_list.begin(); it2 != unit->order_list.end(); it2++) {
				RELEASE(*it2);

				unit->order_list.clear();
			}
		}

		iPoint mouse = { mouseX, mouseY };
		Collider* clicked_on = CheckCursorHover(mouse);
		list<Entity*> aux_list;

		switch (cursor_hover) {

		case HOVERING_ENEMY:

			if (clicked_on->entity->state != DESTROYED) {
				for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
					unit = (Unit*)(*it);
					unit->order_list.push_front(new UnitAttackOrder(clicked_on->entity));
					unit->state = ATTACKING;
				}
			}
			break;

		case HOVERING_RESOURCE:

			for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
				unit = (Unit*)(*it);

				if (unit->IsVillager)
					unit->order_list.push_front(new GatherOrder((Resource*)clicked_on->entity));
				else
					aux_list.push_back((*it));
			}
			break;

		case HOVERING_ALLY_BUILDING:

			if (clicked_on->entity->state == BEING_BUILT) {

				for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
					unit = (Unit*)(*it);

					if (unit->IsVillager)
						unit->order_list.push_front(new BuildOrder((Building*)clicked_on->entity));
				}
			}

		default:

			for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++)
				aux_list.push_back((*it));

			break;
		}

		if (!aux_list.empty()) {

			iPoint destination = App->map->WorldToMap(mouseX, mouseY);
			Unit* commander = (Unit*)aux_list.front();

			if (commander->SetDestination(destination)) {

				if (aux_list.size() > 1) {

					aux_list.pop_front();
					App->pathfinding->SharePath(commander, aux_list);
					aux_list.push_front(commander);

				}

				for (list<Entity*>::iterator it = aux_list.begin(); it != aux_list.end(); it++) {
					unit = (Unit*)(*it);
					unit->order_list.push_front(new FollowPathOrder());
				}
				
			}

		}

		if (buildingToCreate != nullptr && buildingToCreate->waitingToPlace) {
			placingBuilding = false;
			buildingToCreate->Destroy();
			buildingToCreate = nullptr;
		}
	}
	
	if (mouseY > NotHUD.y - CAMERA_OFFSET_Y && mouseY < NotHUD.h - CAMERA_OFFSET_Y) {

		if (placingBuilding) {
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && buildingToCreate->canBePlaced) {
				buildingToCreate->Life = 1;
				buildingToCreate->state = BEING_BUILT;
				buildingToCreate->entityTexture = buildingToCreate->constructingPhase1;
				buildingToCreate->GetBuildingBoundaries();
				buildingToCreate->collider->type = COLLIDER_BUILDING;
				buildingToCreate->waitingToPlace = false;
				App->fog->AddEntity(buildingToCreate);

				if (!selectedEntityList.empty() && selectedListType == COLLIDER_UNIT) {

					for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
						unit = (Unit*)(*it);
						if (unit->IsVillager) {
							unit->order_list.push_front(new BuildOrder(buildingToCreate));
							unit->buildingToCreate = buildingToCreate;
						}
					}
				}
				placingBuilding = false;
			}
		}
		else {
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

				FillSelectedList();
				break;
			}
		}
	}

	if (!selectedEntityList.empty())
		DrawSelectedList();

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
		buildingToCreate->entityPosition = { mouseX, mouseY };
		buildingToCreate->collider->pos = { buildingToCreate->entityPosition.x,buildingToCreate->entityPosition.y };
		buildingToCreate->range->pos = { buildingToCreate->entityPosition.x,buildingToCreate->entityPosition.y };
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

			unitTemplate->cost.woodCost = unitNodeInfo.child("Stats").child("Cost").child("woodCost").attribute("value").as_int();
			unitTemplate->cost.stoneCost = unitNodeInfo.child("Stats").child("Cost").child("stoneCost").attribute("value").as_int();
			unitTemplate->cost.foodCost = unitNodeInfo.child("Stats").child("Cost").child("foodCost").attribute("value").as_int();
			unitTemplate->cost.goldCost = unitNodeInfo.child("Stats").child("Cost").child("goldCost").attribute("value").as_int();

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
			string constructingPhase1Path = buildingNodeInfo.child("Textures").child("ConstructingPhase1").attribute("value").as_string();
			string constructingPhase2Path = buildingNodeInfo.child("Textures").child("ConstructingPhase2").attribute("value").as_string();
			string constructingPhase3Path = buildingNodeInfo.child("Textures").child("ConstructingPhase3").attribute("value").as_string();

			buildingTemplate->faction = (Faction)buildingNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			buildingTemplate->Life = buildingNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			buildingTemplate->buildingBuildTime = buildingNodeInfo.child("Stats").child("BuildTime").attribute("value").as_int();
			buildingTemplate->canAttack = buildingNodeInfo.child("Stats").child("CanAttack").attribute("value").as_bool();

			buildingTemplate->buildingIdleTexture = App->tex->Load(idleTexturePath.c_str());
			buildingTemplate->buildingDieTexture = App->tex->Load(dieTexturePath.c_str());
			buildingTemplate->constructingPhase1 = App->tex->Load(constructingPhase1Path.c_str());
			buildingTemplate->constructingPhase2 = App->tex->Load(constructingPhase2Path.c_str());
			buildingTemplate->constructingPhase3 = App->tex->Load(constructingPhase3Path.c_str());

			buildingTemplate->cost.woodCost = buildingNodeInfo.child("Stats").child("Cost").child("woodCost").attribute("value").as_int();
			buildingTemplate->cost.stoneCost = buildingNodeInfo.child("Stats").child("Cost").child("stoneCost").attribute("value").as_int();
			buildingTemplate->cost.foodCost = buildingNodeInfo.child("Stats").child("Cost").child("foodCost").attribute("value").as_int();
			buildingTemplate->cost.goldCost = buildingNodeInfo.child("Stats").child("Cost").child("goldCost").attribute("value").as_int();

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

	App->fog->AddEntity(unit);

	return unit;
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

	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {

		if ((*it)->IsVillager && (*it)->state == GATHERING) {
			for (list<Order*>::iterator it2 = (*it)->order_list.begin(); it2 != (*it)->order_list.end(); it2++) {

				if ((*it2)->order_type == GATHER) {

					GatherOrder* gth_order = (GatherOrder*)(*it2);
					if (gth_order->resource = resource)
						gth_order->resource = nullptr;
				}
			}
		}
	}


	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {

		if ((*it)->IsVillager && (*it)->state == GATHERING) {
			for (list<Order*>::iterator it2 = (*it)->order_list.begin(); it2 != (*it)->order_list.end(); it2++) {

				if ((*it2)->order_type == GATHER) {

					GatherOrder* gth_order = (GatherOrder*)(*it2);
					if (gth_order->resource = resource)
						gth_order->resource = nullptr;
				}
			}
		}
	}
}

void EntityManager::OnCollision(Collision_data& col_data)
{

	//possible colliding elements
	Unit* unit = nullptr;
	Building* building = nullptr;
	Resource* resource = nullptr;

	col_data.c1->colliding = true;
	col_data.c2->colliding = true;
	col_data.state = SOLVING;

	if (col_data.c1->type == COLLIDER_RANGE) {

		if (col_data.c1->entity->faction != col_data.c2->entity->faction) {

			if (building = col_data.c1->entity->collider->GetBuilding()) 
				building->order_list.push_front(new BuildingAttackOrder(col_data.c2->entity));
		}
	}
	else if (col_data.c1->type == COLLIDER_UNIT) {    // c1->type == COLLIDER_UNIT

		unit = col_data.c1->GetUnit();
		Unit* unit2 = nullptr;

		switch (col_data.c2->type) {

		case COLLIDER_UNIT:

			//if (col_data.c2->entity->faction == unit->faction) {
			//	unit2 = col_data.c2->GetUnit();
			//	col_data.state = App->pathfinding->SolveCollision(unit, unit2);// first parameter should be the higher priority unit!
			//}
			break;

		case COLLIDER_BUILDING:

			if (unit->path)
				App->pathfinding->Repath(unit->path, unit->entityPosition);

			if (!unit->order_list.empty()) {
				if (unit->order_list.front()->order_type == MOVING) {

					unit->order_list.pop_front();
					unit->SetDestination(unit->destinationTileWorld);
					unit->order_list.push_front(new FollowPathOrder());
				}
				else if (unit->order_list.front()->order_type == REACH) {

					ReachOrder* old_order = (ReachOrder*)unit->order_list.front();
					Order* new_order = new ReachOrder(old_order->entity);

					unit->order_list.pop_front();
					unit->order_list.push_front(new_order);
				}
			}
		
			break;

		default:
			break;
		}
	}

}



Resource* EntityManager::FindNearestResource(resourceType type, iPoint pos) {

	Resource* ret = resourceList.front();

	if (type != NONE) {
		for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
			if ((*it)->type == type) {
				if (pos.DistanceTo((*it)->entityPosition) < pos.DistanceTo(ret->entityPosition))
					ret = (*it);
			}
		}
	}
	else
		ret = nullptr;

	return ret;
}

Entity* EntityManager::FindTarget(Unit* unit) {

	list<Unit*>* enemy_units = nullptr;

	if (unit->faction == FREE_MEN)
		enemy_units = &enemyUnitList;
	else
		enemy_units = &friendlyUnitList;

	Entity* ret = nullptr;
	iPoint aux{ -1,-1 };
	for (list<Unit*>::iterator it = enemy_units->begin(); it != enemy_units->end(); it++) {
		if(unit->entityPosition.DistanceTo((*it)->entityPosition) < unit->los->r && (*it)->state != DESTROYED &&
			unit->entityPosition.DistanceTo((*it)->entityPosition) < aux.DistanceTo((*it)->entityPosition)) {
			ret = (*it);
			aux = (*it)->entityPosition;
		}
	}

	if (ret == nullptr) {

		list<Building*>* enemy_buildings = nullptr;

		if (unit->faction == FREE_MEN)
			enemy_buildings = &enemyBuildingList;
		else
			enemy_buildings = &friendlyBuildingList;

		aux.create(-1, -1);
		for (list<Building*>::iterator it = enemy_buildings->begin(); it != enemy_buildings->end(); it++) {
			if (unit->entityPosition.DistanceTo((*it)->entityPosition) < unit->range->r && (*it)->state != DESTROYED &&
				unit->entityPosition.DistanceTo((*it)->entityPosition) < aux.DistanceTo((*it)->entityPosition)) {
				ret = (*it);
				aux = (*it)->entityPosition;
			}
		}
	}

	return ret;
}

void EntityManager::Untarget(Entity* destroyed_entity) {

	list<Unit*>* enemy_units = nullptr;

	if (destroyed_entity->faction == FREE_MEN)
		enemy_units = &enemyUnitList;
	else
		enemy_units = &friendlyUnitList;


	for (list<Unit*>::iterator it = enemy_units->begin(); it != enemy_units->end(); it++) {

		if ((*it)->state == ATTACKING) {

			for (list<Order*>::iterator it2 = (*it)->order_list.begin(); it2 != (*it)->order_list.end(); it2++) {

				if ((*it2)->order_type == ATTACK) {
					UnitAttackOrder* atk_order = (UnitAttackOrder*)(*it);

					if (atk_order->target == destroyed_entity)
						atk_order->state = COMPLETED;
				}
			}
		}
	}


	list<Building*>* enemy_buildings = nullptr;

	if (destroyed_entity->faction == FREE_MEN)
		enemy_buildings = &enemyBuildingList;
	else
		enemy_buildings = &friendlyBuildingList;


	for (list<Building*>::iterator it3 = enemy_buildings->begin(); it3 != enemy_buildings->end(); it3++) {

		if ((*it3)->state == ATTACKING) {

			for (list<Order*>::iterator it4 = (*it3)->order_list.begin(); it4 != (*it3)->order_list.end(); it4++) {

				if ((*it4)->order_type == ATTACK) {
					UnitAttackOrder* atk_order = (UnitAttackOrder*)(*it3);

					if (atk_order->target == destroyed_entity)
						atk_order->state = COMPLETED;
				}
			}
		}
	}
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

Collider* EntityManager::CheckCursorHover(iPoint cursor_pos) {

	Collider* nearest_col = App->collision->FindNearestCollider(cursor_pos);

	if (cursor_pos.DistanceTo(nearest_col->pos) < nearest_col->r) {

		if (nearest_col->type == COLLIDER_RESOURCE)
			cursor_hover = HOVERING_RESOURCE;
		else if (nearest_col->entity->faction == SAURON_ARMY)
			cursor_hover = HOVERING_ENEMY;
		else {
			if (nearest_col->type == COLLIDER_UNIT)
				cursor_hover = HOVERING_ALLY_UNIT;
			else
				cursor_hover = HOVERING_ALLY_BUILDING;
		}
	}
	else
		 cursor_hover = HOVERING_TERRAIN;

	return nearest_col;
}

void EntityManager::FillSelectedList() {

	selectedEntityList.clear();

	if (multiSelectionRect.w == 0 && multiSelectionRect.h == 0) {   // if there's no selection rect... (only clicked)

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
							selectedEntityList.push_back((Entity*)(*it));
					}
					clicked_entity = nullptr;
				}
				else {   // single click
					clicked_entity = nearest_col->entity;
					selectedEntityList.push_back(clicked_entity);
				}
				selectedListType = COLLIDER_UNIT;
				break;
			case COLLIDER_BUILDING:
				clicked_entity = nearest_col->entity;
				selectedEntityList.push_back(clicked_entity);
				selectedListType = COLLIDER_BUILDING;
				break;
			case COLLIDER_RESOURCE:
				clicked_entity = nearest_col->entity;
				selectedEntityList.push_back(clicked_entity);
				selectedListType = COLLIDER_BUILDING;
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
				selectedEntityList.push_back((Entity*)(*it));
				selectedCount++;
			}
		}

		if (selectedCount == 0) {

			for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
				SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
					selectedEntityList.push_back((Entity*)(*it));
					selectedCount++;
				}
			}
		}

		if (selectedCount == 0) {

			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
					selectedEntityList.push_back((Entity*)(*it));
					selectedCount++;
				}
			}
		}

		if (selectedCount == 0) {

			for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
				SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
					selectedEntityList.push_back((Entity*)(*it));
					selectedCount++;
				}
			}
		}

		if (selectedCount == 0) {

			for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
				SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect))
					selectedEntityList.push_back((Entity*)(*it));
			}
		}
	}

	if (!selectedEntityList.empty())
		selectedListType = selectedEntityList.front()->collider->type;

	multiSelectionRect = { 0,0,0,0 };
}

void EntityManager::DrawSelectedList() {

	for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {

		Sprite circle;

		if (selectedListType == COLLIDER_UNIT) {
			Unit* unit = (Unit*)(*it);
			circle.pos = { unit->entityPosition.x, unit->entityPosition.y + (unit->r.h / 2) };
			circle.priority = (*it)->entityPosition.y - (unit->r.h / 2) + unit->r.h - 1;
		}
		else {
			circle.pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };
			circle.priority = (*it)->entityPosition.y;// -(r.h / 2) + r.h - 1;
		}

		circle.radius = (*it)->collider->r;
		circle.r = 255;
		circle.g = 255;
		circle.b = 255;

		App->render->sprites_toDraw.push_back(circle);
	}
}



