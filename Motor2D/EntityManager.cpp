#include "EntityManager.h"
#include "p2Log.h"
#include "Collision.h"
#include "Application.h"
#include "Unit.h"
#include "Pathfinding.h"
#include "FogOfWar.h"
#include "Render.h"
#include "Textures.h"
#include "Resource.h"
#include "SceneManager.h"

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
		if (App->sceneManager->current_scene->name == "scene")
			App->sceneManager->current_scene->Start();
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

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		(*it)->Update(dt);
		(*it)->Draw();
	}
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

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		Unit* clickedUnit = nullptr;
		for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
			if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
				mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
				//if ((*it)->isVisible) {
					clickedUnit = (*it);
				//}
			}
		}

		for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
			if ((*it)->isSelected) {
				(*it)->SetDestination();
				if (clickedUnit != nullptr) {
					(*it)->attackUnitTarget = clickedUnit;
				}
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		multiSelectionRect.x = mouseX;
		multiSelectionRect.y = mouseY;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		multiSelectionRect.w = mouseX - multiSelectionRect.x;
		multiSelectionRect.h = mouseY - multiSelectionRect.y;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		if (multiSelectionRect.w != 0) {
			
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
					(*it)->isSelected = true;
					selectedCount++;
					selectedUnitList.push_back((*it));
				}
			}
			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				if ((*it)->isSelected) {
					(*it)->isSelected = false;
					selectedUnitList.remove(*it);
				}
			}
			if (selectedCount == 0) {
				for (list<Unit*>::iterator it2 = selectedUnitList.begin(); it2 != selectedUnitList.end(); it2++) {
					if ((*it2)->isSelected) {
						(*it2)->isSelected = false;
						selectedUnitList.remove(*it2);
					}
				}
				for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
					SDL_Point pos;
					pos.x = (*it)->entityPosition.x;
					pos.y = (*it)->entityPosition.y;

					if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
						(*it)->isSelected = true;
					}
					else {
						if ((*it)->isSelected) {
							(*it)->isSelected = false;
							selectedUnitList.remove(*it);
						}
					}
				}
			}
			multiSelectionRect = { 0,0,0,0 };
		}
		else {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
					mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
					if (selectedUnitList.empty() || selectedUnitList.front() != (*it)) {
						(*it)->isSelected = true;
						selectedUnitList.push_back(*it);
						timesClicked = 0;
					}
					for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
						if (*it != *it2) {
							if ((*it2)->isSelected) {
								(*it2)->isSelected = false;
								selectedUnitList.remove(*it2);
							}
						}
					}
					timesClicked++;
					doubleClickTimer = 0;
					break;
				}
				else {
					if ((*it)->isSelected) {
						(*it)->isSelected = false;
						selectedUnitList.remove(*it);
					}
				}
			}
			for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
				if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
					mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
					(*it)->isSelected = true;
					for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
						if (*it != *it2) {
							if ((*it2)->isSelected) {
								(*it2)->isSelected = false;
								selectedUnitList.remove(*it2);
							}
						}
					}
					break;
				}
				else {
					if ((*it)->isSelected) {
						(*it)->isSelected = false;
						selectedUnitList.remove(*it);
					}
				}
			}
		}
	}

	if (multiSelectionRect.w != 0) {
		App->render->DrawQuad(multiSelectionRect, 255, 255, 255, 255, false);
	}

	if (doubleClickTimer <= 0.5f) {
		if (timesClicked == 2) {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if (!selectedUnitList.empty()) {
					if ((*it) != selectedUnitList.front()) {
						if ((*it)->GetType() == selectedUnitList.front()->GetType()) {
							(*it)->isSelected = true;
							selectedUnitList.push_back((*it));
						}
					}
				}
			}
			timesClicked = 0;
		}
		doubleClickTimer += dt;
	}
	else {
		timesClicked = 0;
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

bool EntityManager::IsOccupied(iPoint tile, Unit* ignore_unit) {

	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		if ((*it) != ignore_unit) {
			if (tile == App->map->WorldToMap((*it)->entityPosition.x, (*it)->entityPosition.y) && (*it)->state == UNIT_IDLE)
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
		}

		for (resourceNodeInfo = gameData.child("Resources").child("Trees"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resouce")) {

			Resource* resourceTemplate = new Resource();

			string idleTexturePath = resourceNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string gatheringTexturePath = resourceNodeInfo.child("Textures").child("Gathering").attribute("value").as_string();

			resourceTemplate->resourceLife = resourceNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			resourceTemplate->resourceRect = { 0,0,121,195 };

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

void EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	//Uncomment for combat

	//What about creating a var in collider class that stores the unit or buildings?
	//and we take out all these for

	if (c1->type == COLLIDER_FRIENDLY_UNIT) {
		if (c2->type == COLLIDER_ENEMY_UNIT || c2->type == COLLIDER_ENEMY_BUILDING) {
			for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
				if ((*it)->collider == c1 && (*it)->attackUnitTarget != nullptr) {
					(*it)->SetState(UNIT_ATTACKING);
					/*if (c2->type == COLLIDER_ENEMY_UNIT) {
						for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackUnitTarget = (*it2);
								break;
							}
						}
					}
					if (c2->type == COLLIDER_ENEMY_BUILDING) {
						for (list<Building*>::iterator it2 = enemyBuildingList.begin(); it2 != enemyBuildingList.end(); it2++) {
							if ((*it2)->collider == c2) {
								(*it)->attackBuildingTarget = (*it2);
								break;
							}
						}
					}*/
					break;
				}
			}
		}
		else if (c2->type == COLLIDER_FRIENDLY_UNIT) {
			for (list<Unit*>::iterator unit1 = friendlyUnitList.begin(); unit1 != friendlyUnitList.end(); unit1++) {
				if ((*unit1)->collider == c1) {
					for (list<Unit*>::iterator unit2 = friendlyUnitList.begin(); unit2 != friendlyUnitList.end(); unit2++) {
						if ((*unit2)->collider == c2) {

							if ((*unit1)->state == UNIT_MOVING && (*unit2)->state == UNIT_IDLE) {

								if (!(*unit1)->path.empty())
									(*unit1)->path.pop_front();

								(*unit1)->path.push_front(App->pathfinding->FindNearestAvailable((*unit1)));
								(*unit1)->SetState(UNIT_MOVING);
							}
						}
					}
				}
			}
		}
	}
	//if (c1->type == COLLIDER_FRIENDLY_BUILDING && c2->type == COLLIDER_ENEMY_UNIT) {
	//	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
	//		if ((*it)->collider == c1) {
	//			if ((*it)->canAttack) {
	//				(*it)->state = BUILDING_ATTACKING;
	//				/*for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++) {
	//					if ((*it2)->collider == c2) {
	//						(*it)->attackUnitTarget = (*it2);
	//						break;
	//					}
	//				}*/
	//			}
	//			break;
	//		}
	//	}
	//}

	////Enemy
	//if (c2->type == COLLIDER_ENEMY_UNIT) {
	//	if (c1->type == COLLIDER_FRIENDLY_UNIT || c1->type == COLLIDER_FRIENDLY_BUILDING) {
	//		for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
	//			if ((*it)->collider == c2 && (*it)->attackUnitTarget != nullptr) {
	//				(*it)->SetState(UNIT_ATTACKING);
	//				/*if (c1->type == COLLIDER_FRIENDLY_UNIT) {
	//					for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
	//						if ((*it2)->collider == c1) {
	//							(*it)->attackUnitTarget = (*it2);
	//							break;
	//						}
	//					}
	//				}
	//				if (c1->type == COLLIDER_FRIENDLY_BUILDING) {
	//					for (list<Building*>::iterator it2 = friendlyBuildingList.begin(); it2 != friendlyBuildingList.end(); it2++) {
	//						if ((*it2)->collider == c1) {
	//							(*it)->attackBuildingTarget = (*it2); 
	//							break;
	//						}
	//					}
	//				}*/
	//				break;
	//			}
	//		}
	//	}
	//}
	//if (c2->type == COLLIDER_ENEMY_BUILDING && c1->type == COLLIDER_FRIENDLY_UNIT) {
	//	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
	//		if ((*it)->collider == c2 && (*it)->attackUnitTarget != nullptr) {
	//			if ((*it)->canAttack) {
	//				(*it)->state = BUILDING_ATTACKING;
	//				/*for (list<Unit*>::iterator it2 = friendlyUnitList.begin(); it2 != friendlyUnitList.end(); it2++) {
	//					if ((*it2)->collider == c1) {
	//						(*it)->attackUnitTarget = (*it2);
	//						break;
	//					}
	//				}*/
	//			}
	//			break;
	//		}
	//	}
	//}
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