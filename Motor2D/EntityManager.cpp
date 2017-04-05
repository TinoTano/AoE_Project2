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

	SDL_Rect cam = App->render->culling_cam;

	for (list<Resource*>::iterator it = resourceList.begin(); it != resourceList.end(); it++) {
		if ((*it)->entityPosition.x >= (cam.x - 150) && (*it)->entityPosition.x <= (cam.x + cam.w + 150) &&
			(*it)->entityPosition.y > (cam.y - 150) && (*it)->entityPosition.y < (cam.y + cam.h + 150)) {
			(*it)->isVisible = true;
		}
		else {
			(*it)->isVisible = false;
		}
		if ((*it)->isVisible) {
			(*it)->Draw();
		}
	}
	for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
		(*it)->Update(dt);
		//App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
		(*it)->Update(dt);
	}
	for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
		(*it)->Update(dt);
		//App->fog->removeFog((*it)->entityPosition.x, (*it)->entityPosition.y);
	}
	for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
		(*it)->Update(dt);
	}

	if (mouseY > NOTHUD.y - CAMERA_OFFSET_Y && mouseY < NOTHUD.h - CAMERA_OFFSET_Y)
	{

		// Enemies
		for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++)
		{
			if ((*it)->state == UNIT_IDLE && (*it)->destinationReached)
			{
				(*it)->attackUnitTarget = nullptr;
				iPoint target;
				target = App->sceneManager->level1_scene->my_townCenter->GetPosition();
				target = App->map->WorldToMap(target.x, target.y);
				(*it)->SetDestination(target);
				(*it)->attackBuildingTarget = App->sceneManager->level1_scene->my_townCenter;
				(*it)->checkTile = true;
			}
		}

		// Allies
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			for (list<Unit*>::iterator it = selectedUnitList.begin(); it != selectedUnitList.end(); it++)
			{
				if (!(*it)->isEnemy) {
					(*it)->attackBuildingTarget = nullptr;
					(*it)->attackUnitTarget = nullptr;
					(*it)->resourceTarget = nullptr;
					iPoint target;
					App->input->GetMousePosition(target.x, target.y);
					for (list<Unit*>::iterator it2 = enemyUnitList.begin(); it2 != enemyUnitList.end(); it2++)
					{
						if (mouseX < (*it2)->entityPosition.x + ((*it2)->collider->rect.w / 2) && mouseX >(*it2)->entityPosition.x - ((*it2)->collider->rect.w / 2) &&
							mouseY < (*it2)->entityPosition.y + ((*it2)->collider->rect.h / 2) && mouseY >(*it2)->entityPosition.y - ((*it2)->collider->rect.h / 2))
						{
							clickedUnit = *it2;
							break;
						}
					}
					if (clickedUnit != nullptr)
					{
						(*it)->attackUnitTarget = clickedUnit;
						clickedUnit = nullptr;
					}
					else {
						for (list<Building*>::iterator it3 = enemyBuildingList.begin(); it3 != enemyBuildingList.end(); it3++)
						{
							if ((*it3)->type == SAURON_TOWER) {
								LOG("%d %d", mouseY, (*it3)->entityPosition.y);
								if (mouseX < (*it3)->entityPosition.x + ((*it3)->collider->rect.w / 2) && mouseX >(*it3)->entityPosition.x - ((*it3)->collider->rect.w / 2) &&
									mouseY < (*it3)->entityPosition.y + ((*it3)->collider->rect.h * 2) && mouseY >(*it3)->entityPosition.y + 150)
								{
									clickedBuilding = *it3;
									break;
								}
							}
							else {
								if (mouseX < (*it3)->entityPosition.x + ((*it3)->collider->rect.w / 2) && mouseX >(*it3)->entityPosition.x - ((*it3)->collider->rect.w / 2) &&
									mouseY < (*it3)->entityPosition.y + ((*it3)->collider->rect.h / 2) && mouseY >(*it3)->entityPosition.y - ((*it3)->collider->rect.h / 2))
								{
									clickedBuilding = *it3;
									break;
								}
							}
						}
						if (clickedBuilding != nullptr) {
							(*it)->attackBuildingTarget = clickedBuilding;
							clickedBuilding = nullptr;
						}
						else {
							if ((*it)->type == VILLAGER || (*it)->type == ELF_VILLAGER) {
								for (list<Resource*>::iterator it4 = resourceList.begin(); it4 != resourceList.end(); it4++)
								{
									if (mouseX < (*it4)->entityPosition.x + (90 / 2) && mouseX >(*it4)->entityPosition.x - (90 / 2) &&
										mouseY < (*it4)->entityPosition.y + (150 / 2) && mouseY >(*it4)->entityPosition.y - (150 / 2))
									{
										clickedResource = *it4;
										break;
									}
								}
								if (clickedResource != nullptr)
								{
									(*it)->resourceTarget = clickedResource;
									clickedResource = nullptr;
								}
							}
						}
					}
					target = App->map->WorldToMap(target.x - App->render->camera.x, target.y - App->render->camera.y);
					(*it)->SetDestination(target);
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
					else {
						if ((*it)->isSelected) {
							(*it)->isSelected = false;
							selectedUnitList.remove(*it);
						}
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
				bool selectedUnit = false;
				for (list<Unit*>::iterator it = friendlyUnitList.begin(); it != friendlyUnitList.end(); it++) {
					if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
						mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
						if (selectedUnitList.empty() || selectedUnitList.front() != (*it)) {
							(*it)->isSelected = true;
							selectedUnitList.push_back(*it);
							timesClicked = 0;
						}
						for (list<Unit*>::iterator it2 = selectedUnitList.begin(); it2 != selectedUnitList.end(); it2++) {
							if (*it != *it2) {
								(*it2)->isSelected = false;
								selectedUnitList.remove(*it2);
							}
						}
						timesClicked++;
						doubleClickTimer = 0;
						selectedUnit = true;
						break;
					}
					else {
						if ((*it)->isSelected) {
							(*it)->isSelected = false;
							selectedUnitList.remove(*it);
						}
					}
				}
				if (!selectedUnit) {
					bool selectedEnemyUnit = false;
					for (list<Unit*>::iterator it = enemyUnitList.begin(); it != enemyUnitList.end(); it++) {
						if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
							mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
							selectedUnitList.clear();
							(*it)->isSelected = true;
							selectedUnitList.push_back(*it);
							selectedEnemyUnit = true;
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
					if (!selectedEnemyUnit) {
						bool selectedBuilding = false;
						for (list<Building*>::iterator it = friendlyBuildingList.begin(); it != friendlyBuildingList.end(); it++) {
							if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
								mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h / 2) && mouseY >(*it)->entityPosition.y - ((*it)->collider->rect.h / 2)) {
								(*it)->isSelected = true;
								selectedBuildingList.push_back(*it);
								selectedBuilding = true;
								break;
							}
						}
						if (!selectedBuilding) {
							bool selectedEnemyBuilding = false;
							for (list<Building*>::iterator it = enemyBuildingList.begin(); it != enemyBuildingList.end(); it++) {
								if (mouseX < (*it)->entityPosition.x + ((*it)->collider->rect.w / 2) && mouseX >(*it)->entityPosition.x - ((*it)->collider->rect.w / 2) &&
									mouseY < (*it)->entityPosition.y + ((*it)->collider->rect.h * 2) && mouseY >(*it)->entityPosition.y + 150) {
									(*it)->isSelected = true;
									selectedBuildingList.push_back(*it);
									selectedBuilding = true;
									break;
								}
							}
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

			unitTemplate->type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();

			string idleTexturePath = unitNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string moveTexturePath = unitNodeInfo.child("Textures").child("Move").attribute("value").as_string();
			string attackTexturePath = unitNodeInfo.child("Textures").child("Attack").attribute("value").as_string();
			string dieTexturePath = unitNodeInfo.child("Textures").child("Die").attribute("value").as_string();

			//Villager
			string chopTexturePath;
			if (unitTemplate->type == VILLAGER || unitTemplate->type == ELF_VILLAGER) {
				chopTexturePath = unitNodeInfo.child("Textures").child("Cut").attribute("value").as_string();
			}

			unitTemplate->faction = (unitFaction)unitNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			unitTemplate->attackSpeed = 1 / unitNodeInfo.child("Stats").child("AttackSpeed").attribute("value").as_float();
			unitTemplate->unitLife = unitNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			unitTemplate->unitMaxLife = unitTemplate->unitLife;
			unitTemplate->unitAttack = unitNodeInfo.child("Stats").child("Attack").attribute("value").as_int();
			unitTemplate->unitDefense = unitNodeInfo.child("Stats").child("Defense").attribute("value").as_int();
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
					unitTemplate->cuttingAnimations.push_back(chop);
					if (i != 0 && i != rows - 1) {
						chop.flip = SDL_FLIP_HORIZONTAL;
						unitTemplate->cuttingAnimations.push_back(chop);
					}
				}
				unitTemplate->unitChoppingTexture = App->tex->Load(chopTexturePath.c_str());
			}

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
			buildingTemplate->buildingMaxLife = buildingTemplate->buildingLife;
		}

		for (resourceNodeInfo = gameData.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource"))
		{

			Resource* resourceTemplate = new Resource();

			string idleTexturePath = resourceNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string gatheringTexturePath = resourceNodeInfo.child("Textures").child("Gathering").attribute("value").as_string();

			resourceTemplate->resourceLife = resourceNodeInfo.child("Stats").child("Life").attribute("value").as_int();

			for (pugi::xml_node rectsNode = resourceNodeInfo.child("Rects").child("Rect"); rectsNode; rectsNode = rectsNode.next_sibling("Rect")) {
				resourceTemplate->resourceRectVector.push_back({ rectsNode.attribute("x").as_int(), rectsNode.attribute("y").as_int(), rectsNode.attribute("w").as_int(), rectsNode.attribute("h").as_int() });
			}

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

Resource* EntityManager::CreateResource(int posX, int posY, resourceType type, int resourceRectIndex)
{
	iPoint ret;
	ret.x = (posX - posY);
	ret.y = (posX + posY) / 2;
	Resource* resource = new Resource(ret.x, ret.y, resourcesDB[type], resourceRectIndex);
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
	/*if (c1->type == COLLIDER_FRIENDLY_UNIT && c2->type == COLLIDER_ENEMY_UNIT)
	{
	for (list<Unit*>::iterator friendly_unit = friendlyUnitList.begin(); friendly_unit != friendlyUnitList.end(); friendly_unit++)
	{
	if ((*friendly_unit)->collider == c1)
	{
	(*friendly_unit)->SetState(UNIT_ATTACKING);

	for (list<Unit*>::iterator enemy_unit = enemyUnitList.begin(); enemy_unit != enemyUnitList.end(); enemy_unit++)
	{
	if ((*enemy_unit)->collider == c2)
	{
	(*enemy_unit)->SetState(UNIT_ATTACKING);
	if ((*friendly_unit)->attackUnitTarget == nullptr) (*friendly_unit)->attackUnitTarget = (*enemy_unit);
	if ((*enemy_unit)->attackUnitTarget == nullptr) (*enemy_unit)->attackUnitTarget = (*friendly_unit);
	}
	}
	}
	}
	}

	if (c1->type == COLLIDER_ENEMY_UNIT && c2->type == COLLIDER_FRIENDLY_BUILDING)
	{
	for (list<Unit*>::iterator enemy_unit = enemyUnitList.begin(); enemy_unit != enemyUnitList.end(); enemy_unit++)
	{
	if ((*enemy_unit)->collider == c1 && (*enemy_unit)->attackUnitTarget == nullptr)
	{
	for (list<Building*>::iterator friendly_building = friendlyBuildingList.begin(); friendly_building != friendlyBuildingList.end(); friendly_building++)
	{
	if ((*friendly_building)->collider == c2)
	{
	(*enemy_unit)->SetState(UNIT_ATTACKING);
	(*enemy_unit)->attackBuildingTarget = (*friendly_building);
	break;
	}
	}
	}
	}
	}

	if (c1->type == COLLIDER_FRIENDLY_UNIT && c2->type == COLLIDER_ENEMY_BUILDING)
	{
	for (list<Unit*>::iterator friendly_unit = friendlyUnitList.begin(); friendly_unit != friendlyUnitList.end(); friendly_unit++)
	{
	if ((*friendly_unit)->collider == c1 && (*friendly_unit)->attackUnitTarget == nullptr)
	{
	for (list<Building*>::iterator enemy_building = enemyBuildingList.begin(); enemy_building != enemyBuildingList.end(); enemy_building++)
	{
	if ((*enemy_building)->collider == c2)
	{
	(*friendly_unit)->SetState(UNIT_ATTACKING);
	(*friendly_unit)->attackBuildingTarget = (*enemy_building);
	break;
	}
	}
	}
	}
	}*/
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