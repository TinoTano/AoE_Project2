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
#include "Audio.h"
#include "Entity.h"
#include "Collision.h"
#include "SceneManager.h"
#include "Hero.h"
#include "AI.h"
#include "Villager.h"
#include "Orders.h"
#include <algorithm>

EntityManager::EntityManager() : Module()
{
	name = "entityManager";
	nextID = 1;

	player = new GameFaction(FREE_MEN);
	AI_faction = new GameFaction(SAURON_ARMY);
}

EntityManager::~EntityManager()
{
	RELEASE(player);
	RELEASE(AI_faction);

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
	iPoint mouse = { mouseX, mouseY };

	for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
		if ((*it)->state == DESTROYED)
			selectedEntityList.erase(it);
	}

	for (list<Entity*>::iterator it = WorldEntityList.begin(); it != WorldEntityList.end(); it++) {

		(*it)->Update(dt);

		if (App->render->CullingCam((*it)->entityPosition))
		{
			if ((*it)->faction == SAURON_ARMY && (*it)->isActive == true || App->map->godmode) (*it)->Draw();
			else if ((*it)->faction == FREE_MEN) (*it)->Draw();
		}
	}

	for (list<Resource*>::iterator it = resource_list.begin(); it != resource_list.end(); it++) {
		if (App->render->CullingCam((*it)->entityPosition) && ((*it)->isActive == true || App->map->godmode))
			(*it)->Draw();
	}

	if (!game_stops)
	{
		player->tech_tree->Update();
		AI_faction->tech_tree->Update();

		Unit* unit = nullptr;
		Villager* villager = nullptr;
		Resource* resource = nullptr;
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !selectedEntityList.empty() && selectedListType == COLLIDER_UNIT) {

			if (selectedEntityList.front()->faction == FREE_MEN && !placingBuilding) {

				for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
					Unit* unit = (Unit*)(*it);

					for (list<Order*>::iterator it2 = unit->order_list.begin(); it2 != unit->order_list.end(); it2++) {
						RELEASE(*it2);
						unit->order_list.clear();
					}
				}

				Collider* clicked_on = CheckCursorHover(mouse);

				switch (cursor_hover) {

				case HOVERING_ENEMY:

					if (clicked_on->entity->state != DESTROYED && clicked_on->entity->isActive) {
						for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
							unit = (Unit*)(*it);
							unit->order_list.push_back(new UnitAttackOrder());
							unit->state = ATTACKING;
						}
					}
					break;

				case HOVERING_RESOURCE:

					resource = (Resource*)clicked_on->entity;

					if (resource->contains != NONE) {
						for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
							unit = (Unit*)(*it);
							if (unit->IsVillager) {
								villager = (Villager*)unit;
								villager->resource_carried = resource->contains;
								villager->order_list.push_back(new GatherOrder());
							}
						}
					}
					break;

				case HOVERING_ALLY_BUILDING:

					if (clicked_on->entity->state == BEING_BUILT) {

						for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
							unit = (Unit*)(*it);
							if (unit->IsVillager)
								unit->order_list.push_back(new BuildOrder());
						}
					}

				default:
					for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
						Unit* unit = (Unit*)(*it);
						unit->order_list.push_front(new MoveToOrder(unit, mouse));
					}
					break;
				}
			}

			if (placingBuilding)
				placingBuilding = false;
		}

		if (mouseY > NotHUD.y - CAMERA_OFFSET_Y && mouseY < NotHUD.h - CAMERA_OFFSET_Y) {

			if (placingBuilding) {

				Building* building = buildingsDB[placing_type];
				Sprite aux;

				aux.texture = building->entityTexture;
				aux.pos.x = mouse.x - (building->imageWidth / 2);
				aux.pos.y = mouse.y - (building->imageHeight / 1.5f);
				aux.priority = mouseY - (building->imageHeight / 2) + building->imageHeight;
				aux.rect.w = building->imageWidth;
				aux.rect.h = building->imageHeight;

				iPoint mouseMap = App->map->WorldToMap(mouseX, mouseY);
				if (!App->collision->FindCollider(mouse, building->imageWidth / 2) && App->fog->Get(mouseMap.x, mouseMap.y) != 0) {

					if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

						if (player->resources.Spend(building->cost)) {

							building = CreateBuilding(mouse.x, mouse.y, placing_type);
							placingBuilding = false;
							App->fog->AddEntity(building);

							if (!selectedEntityList.empty() && selectedListType == COLLIDER_UNIT) {

								for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {
									unit = (Unit*)(*it);
									if (unit->IsVillager)
										unit->order_list.push_front(new BuildOrder());
								}
							}
						}
					}
				}
				else {
					aux.change_color = true;
					aux.r = 255;
				}

				App->render->sprites_toDraw.push_back(aux);
			}
			else {
				// Selecting units by clicking/dragging
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

				// Selecting ALL units by shortcut
				if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
				{
					selectedEntityList.clear();
					for (list<Entity*>::iterator it = WorldEntityList.begin(); it != WorldEntityList.end(); it++) 
					{
						if (App->render->CullingCam((*it)->entityPosition))
						{
							if ((*it)->faction == FREE_MEN && (*it)->entityType == ENTITY_UNIT)
								selectedEntityList.push_back((Entity*)(*it));
						}
					}

					if (selectedEntityList.size() == 1)
					{
						list<Entity*>::iterator it = selectedEntityList.begin();
						clicked_entity = (*it);
					}
				}
			}
		}

		if (!selectedEntityList.empty())
			DrawSelectedList();

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			Sprite square;

			square.rect = multiSelectionRect;
			square.priority = 10000;
			square.r = 255;
			square.g = 255;
			square.b = 255;
			square.filled = false;
			App->render->sprites_toDraw.push_back(square);
		}
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	if (removeEntityList.size() > 0) {
		list<Entity*>::iterator i = removeEntityList.begin();

		while (i != removeEntityList.end())
		{
			list<Entity*>::iterator unitToDestroy = i;
			++i;

			if ((*unitToDestroy)->entityType == ENTITY_BUILDING) App->quest->StepCallback((Building*)(*unitToDestroy));

			DestroyEntity((*unitToDestroy));
		}

		removeEntityList.clear();
	}

	return true;
}

bool EntityManager::CleanUp()
{
	LOG("Freeing EntityManager");

	for (list<Entity*>::iterator it = WorldEntityList.begin(); it != WorldEntityList.end(); it++) 
		RELEASE((*it));

	for (list<Resource*>::iterator it2 = resource_list.begin(); it2 != resource_list.end(); it2++) 
		RELEASE((*it2));
	
	WorldEntityList.clear();

	if (player != nullptr) {
		player->Reset();
		AI_faction->Reset();
	}

	NotHUD = { 0,0,0,0 };

	resource_list.clear();
	selectedEntityList.clear();
	removeEntityList.clear();
	clicked_entity = nullptr;
	mouseX = 0;
	mouseY = 0;
	nextID = 0;
	selectedListType = COLLIDER_NONE;
	placingBuilding = false;
	dt = 0;

	return true;
}

bool EntityManager::Load(pugi::xml_node & data)
{
	//App->fog->entities_not_in_fog.push_back(AI_faction->Town_center);
	//App->fog->entities_on_fog.push_back(player->Town_center->)
	//App->fog->entities_on_fog.push_back(player->Town_center);

	// Destroying all entities created before Save

	for (list<Resource*>::iterator it = App->entityManager->resource_list.begin(); it != App->entityManager->resource_list.end(); ++it)
	{
		if (*it != nullptr) {
			(*it)->Destroy();
		}
	}

	App->entityManager->resource_list.clear();

	for (list<Entity*>::iterator it = App->entityManager->WorldEntityList.begin(); it != App->entityManager->WorldEntityList.end(); ++it)
	{
		if (*it != nullptr) {
			if ((*it) != player->Town_center && (*it) != AI_faction->Town_center)
			{
				(*it)->Destroy();
			}
		}
	}

	App->entityManager->WorldEntityList.clear();
	App->entityManager->selectedEntityList.clear();
	App->fog->entities_not_in_fog.clear();
	App->fog->entities_on_fog.clear();

	// Creating all again

	// -------------------------------------------
	//               ENTITIES
	// -------------------------------------------

	// ------------------- UNITS ------------------

	for (pugi::xml_node unitNode = data.child("Unit"); unitNode; unitNode = unitNode.next_sibling("Unit"))
	{
		Unit* unitTemplate = App->entityManager->CreateUnit(unitNode.child("Position").attribute("x").as_int(),
			unitNode.child("Position").attribute("y").as_int(),
			(unitType)unitNode.child("Type").attribute("value").as_int());

		unitTemplate->currentDirection = (unitDirection)unitNode.child("Direction").attribute("value").as_int();
		unitTemplate->Life = unitNode.child("Life").attribute("value").as_int();
		unitTemplate->entityID = unitNode.child("ID").attribute("value").as_int();

		for (pugi::xml_node order = unitNode.child("Order"); order; order = order.next_sibling("Order"))
		{
			Order* unitOrder = new Order();
			unitOrder->order_type = (OrderType)order.attribute("OrderType").as_int();
			unitOrder->state = (Order_state)order.attribute("OrderState").as_int();
			unitTemplate->order_list.push_back(unitOrder);
		}
	}

	// -------------------------------------------
	// ------------------- BUILDINGS ------------------

	for (pugi::xml_node buildingNode = data.child("Building"); buildingNode; buildingNode = buildingNode.next_sibling("Building"))
	{
		if ((buildingType)buildingNode.child("Type").attribute("value").as_int() != TOWN_CENTER && (buildingType)buildingNode.child("Type").attribute("value").as_int() != SAURON_TOWER)
		{
			Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNode.child("Position").attribute("x").as_int(),
				buildingNode.child("Position").attribute("y").as_int(),
				(buildingType)buildingNode.child("Type").attribute("value").as_int());

			buildingTemplate->Life = buildingNode.child("Life").attribute("value").as_int();
			buildingTemplate->state = (EntityState)buildingNode.child("State").attribute("value").as_int();
			buildingTemplate->entityID = buildingNode.child("ID").attribute("value").as_int();
		}
		else {
			if ((buildingType)buildingNode.child("Type").attribute("value").as_int() == TOWN_CENTER)
			{
				if (player->Town_center->Life <= 0) {
					player->Town_center->Destroy();

					Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNode.child("Position").attribute("x").as_int(),
						buildingNode.child("Position").attribute("y").as_int(),
						(buildingType)buildingNode.child("Type").attribute("value").as_int());
					buildingTemplate->Life = buildingNode.child("Life").attribute("value").as_int();
					buildingTemplate->state = (EntityState)buildingNode.child("State").attribute("value").as_int();
					buildingTemplate->entityID = buildingNode.child("ID").attribute("value").as_int();
					player->Town_center = buildingTemplate;
				}
				else {
					player->Town_center->Life = buildingNode.child("Life").attribute("value").as_int();
					player->Town_center->state = (EntityState)buildingNode.child("State").attribute("value").as_int();
					App->entityManager->WorldEntityList.push_back(player->Town_center);
					App->fog->AddEntity(App->entityManager->player->Town_center);
				}
			}
			else {
				if ((buildingType)buildingNode.child("Type").attribute("value").as_int() == SAURON_TOWER)
				{
					if (AI_faction->Town_center->Life <= 0) {
						Building* buildingTemplate = App->entityManager->CreateBuilding(buildingNode.child("Position").attribute("x").as_int(),
							buildingNode.child("Position").attribute("y").as_int(),
							(buildingType)buildingNode.child("Type").attribute("value").as_int());
						buildingTemplate->Life = buildingNode.child("Life").attribute("value").as_int();
						buildingTemplate->state = (EntityState)buildingNode.child("State").attribute("value").as_int();
						buildingTemplate->entityID = buildingNode.child("ID").attribute("value").as_int();
						AI_faction->Town_center = buildingTemplate;
					}
					else {
						AI_faction->Town_center->Life = buildingNode.child("Life").attribute("value").as_int();
						AI_faction->Town_center->state = (EntityState)buildingNode.child("State").attribute("value").as_int();
						App->entityManager->WorldEntityList.push_back(AI_faction->Town_center);
						App->fog->AddEntity(App->entityManager->AI_faction->Town_center);
					}
				}
			
			}
		}
	}

	// -------------------------------------------
	// ------------------- RESOURCES ------------------

	for (pugi::xml_node resourceNode = data.child("Resource"); resourceNode; resourceNode = resourceNode.next_sibling("Resource")) {

		SDL_Rect rect = { resourceNode.child("Rect").attribute("x").as_int(), resourceNode.child("Rect").attribute("y").as_int(),
			resourceNode.child("Rect").attribute("w").as_int(), resourceNode.child("Rect").attribute("h").as_int() };

		Resource* resourceTemplate = App->entityManager->ReLoadResource(resourceNode.child("Position").attribute("x").as_int(),
			resourceNode.child("Position").attribute("y").as_int(),
			(resourceItem)resourceNode.child("Type").attribute("value").as_int(), rect);

		resourceTemplate->Life = resourceNode.child("Life").attribute("value").as_int();
	}

	// FACTIONS

	player->resources.wood = data.child("Player").attribute("wood").as_int();
	player->resources.gold = data.child("Player").attribute("gold").as_int();
	player->resources.food = data.child("Player").attribute("food").as_int();
	player->resources.stone = data.child("Player").attribute("stone").as_int();

	player->faction = FREE_MEN;
	player->tech_tree->Reset(FREE_MEN);
	
	player->tech_tree->available_techs.clear();
	for (pugi::xml_attribute available_techs = data.child("Player").child("TechTree").child("AvailableTechs").attribute("TechType");
	available_techs; available_techs = available_techs.next_attribute())
	{
		player->tech_tree->available_techs.push_back((TechType)available_techs.as_int());
	}
	player->tech_tree->available_buildings.clear();

	for (pugi::xml_attribute available_buildings = data.child("Player").child("TechTree").child("AvailableBuildings").attribute("BuildingType");
	available_buildings; available_buildings = available_buildings.next_attribute())
	{
		player->tech_tree->available_buildings.push_back((buildingType)available_buildings.as_int());
	}
	player->tech_tree->available_units.clear();
	for (pugi::xml_node available_units = data.child("Player").child("TechTree").child("AvailableUnits").child("Unit");
	available_units; available_units = available_units.next_sibling("Unit"))
	{
		pair<unitType, buildingType> element = { (unitType)available_units.attribute("UnitType").as_int(), (buildingType)available_units.attribute("BuildingType").as_int() };
		player->tech_tree->available_units.push_back(element);
	}
	player->tech_tree->multiplier_list.clear();
	for (pugi::xml_node multipliers = data.child("Player").child("Multipliers"); multipliers; multipliers = multipliers.next_sibling("Multipliers"))
	{
		player->tech_tree->multiplier_list.push_back(multipliers.attribute("Multiplier").as_float());
	}

	AI_faction->resources.wood = data.child("Enemy").attribute("wood").as_int();
	AI_faction->resources.gold = data.child("Enemy").attribute("gold").as_int();
	AI_faction->resources.food = data.child("Enemy").attribute("food").as_int();
	AI_faction->resources.stone = data.child("Enemy").attribute("stone").as_int();

	AI_faction->faction = SAURON_ARMY;
	AI_faction->tech_tree->Reset(SAURON_ARMY);

	AI_faction->tech_tree->available_techs.clear();
	for (pugi::xml_attribute available_techs = data.child("Enemy").child("TechTree").child("AvailableTechs").attribute("TechType");
	available_techs; available_techs = available_techs.next_attribute())
	{
		AI_faction->tech_tree->available_techs.push_back((TechType)available_techs.as_int());
	}
	AI_faction->tech_tree->available_buildings.clear();
	for (pugi::xml_attribute available_buildings = data.child("Enemy").child("TechTree").child("AvailableBuildings").attribute("BuildingType");
	available_buildings; available_buildings = available_buildings.next_attribute())
	{
		AI_faction->tech_tree->available_buildings.push_back((buildingType)available_buildings.as_int());
	}
	AI_faction->tech_tree->available_units.clear();
	for (pugi::xml_node available_units = data.child("Enemy").child("TechTree").child("AvailableUnits").child("Unit");
	available_units; available_units = available_units.next_sibling("Unit"))
	{
		pair<unitType, buildingType> element = { (unitType)available_units.attribute("UnitType").as_int(), (buildingType)available_units.attribute("BuildingType").as_int() };
		AI_faction->tech_tree->available_units.push_back(element);
	}
	AI_faction->tech_tree->multiplier_list.clear();
	for (pugi::xml_node multipliers = data.child("Enemy").child("Multipliers"); multipliers; multipliers = multipliers.next_sibling("Multipliers"))
	{
		AI_faction->tech_tree->multiplier_list.push_back(multipliers.attribute("Multiplier").as_float());
	}

	App->sceneManager->level1_scene->UpdateResources();

	placingBuilding = data.child("BuildingCreation").attribute("PlacingBuilding").as_bool();
	placing_type = (buildingType)data.child("BuildingCreation").attribute("BuildingType").as_int();

	return true;
}

bool EntityManager::Save(pugi::xml_node & data) const
{


	// -------------------------------------------
	//                    ENTITIES
	// -------------------------------------------

	// ------------------- UNITS -----------------

	// -------------- FRIENDLY UNITS ------------

	for (list<Unit*>::iterator it = App->entityManager->player->units.begin(); it != App->entityManager->player->units.end(); it++) {
		if ((*it)->state != DESTROYED) {

			pugi::xml_node unitNodeInfo = data.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("ID").append_attribute("value") = (*it)->entityID;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			// UNIT ORDERS MAMA ;)
			for (list<Order*>::iterator it2 = (*it)->order_list.begin(); it2 != (*it)->order_list.end(); ++it2)
			{
				pugi::xml_node order = unitNodeInfo.append_child("Order");

				order.append_attribute("OrderType") = (*it2)->order_type;
				order.append_attribute("OrderState") = (*it2)->state;
			}
		}
	}
	// -------------------------------------------
	// -------------- ENEMY UNITS ------------
	for (list<Unit*>::iterator it = App->entityManager->AI_faction->units.begin(); it != App->entityManager->AI_faction->units.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node unitNodeInfo = data.append_child("Unit");
			unitNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = unitNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			unitNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			unitNodeInfo.append_child("Direction").append_attribute("value") = (*it)->currentDirection;
			unitNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			unitNodeInfo.append_child("ID").append_attribute("value") = (*it)->entityID;
			pugi::xml_node destTileNode = unitNodeInfo.append_child("DestinationTile");
			for (list<Order*>::iterator it2 = (*it)->order_list.begin(); it2 != (*it)->order_list.end(); ++it2)
			{
				pugi::xml_node order = unitNodeInfo.append_child("Order");

				order.append_attribute("OrderType") = (*it2)->order_type;
				order.append_attribute("OrderState") = (*it2)->state;
			}
		}
	}

	// -------------------------------------------
	// -------------------- BUILDINGS ------------------

	// ---------------- FRIENDLY BUILDINGS ------------
	for (list<Building*>::iterator it = App->entityManager->player->buildings.begin(); it != App->entityManager->player->buildings.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node buildingNodeInfo = data.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			buildingNodeInfo.append_child("ID").append_attribute("value") = (*it)->entityID;
		}
	}
	// -------------------------------------------
	// ---------------- ENEMY BUILDINGS ------------
	for (list<Building*>::iterator it = App->entityManager->AI_faction->buildings.begin(); it != App->entityManager->AI_faction->buildings.end(); it++) {
		if ((*it)->state != DESTROYED) {
			pugi::xml_node buildingNodeInfo = data.append_child("Building");
			buildingNodeInfo.append_child("Type").append_attribute("value") = (*it)->type;
			pugi::xml_node positionNode = buildingNodeInfo.append_child("Position");
			positionNode.append_attribute("x") = (*it)->entityPosition.x;
			positionNode.append_attribute("y") = (*it)->entityPosition.y;
			buildingNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
			buildingNodeInfo.append_child("State").append_attribute("value") = (*it)->state;
			buildingNodeInfo.append_child("ID").append_attribute("value") = (*it)->entityID;
			App->fog->AddEntity(App->entityManager->AI_faction->Town_center);
		}
	}
	// -------------------------------------------
	// ----------------- RESOURCES ------------------
	for (list<Resource*>::iterator it = App->entityManager->resource_list.begin(); it != App->entityManager->resource_list.end(); it++) {
		pugi::xml_node resourceNodeInfo = data.append_child("Resource");
		resourceNodeInfo.append_child("Type").append_attribute("value") = (*it)->contains;
		pugi::xml_node positionNode = resourceNodeInfo.append_child("Position");
		positionNode.append_attribute("x") = (*it)->entityPosition.x;
		positionNode.append_attribute("y") = (*it)->entityPosition.y;
		resourceNodeInfo.append_child("Life").append_attribute("value") = (*it)->Life;
		pugi::xml_node rect = resourceNodeInfo.append_child("Rect");
		rect.append_attribute("x") = (*it)->blit_rect.x;
		rect.append_attribute("y") = (*it)->blit_rect.y;
		rect.append_attribute("w") = (*it)->blit_rect.w;
		rect.append_attribute("h") = (*it)->blit_rect.h;
	}


	// -------------------------------------------
	// ...........................................
	//                 PLAYERS
	// ...........................................
	// -------------------FRIENDLY----------------


	pugi::xml_node Player = data.append_child("Player");

	Player.append_attribute("wood") = player->resources.wood;
	Player.append_attribute("food") = player->resources.food;
	Player.append_attribute("stone") = player->resources.stone;
	Player.append_attribute("gold") = player->resources.gold;

	Player.append_child("TechTree");

	pugi::xml_node available_techs = Player.child("TechTree").append_child("AvailableTechs");
	for (list<TechType>::iterator it = player->tech_tree->available_techs.begin(); it != player->tech_tree->available_techs.end(); ++it)
	{
		available_techs.append_attribute("TechType") = (*it);
	}
	pugi::xml_node available_buildings = Player.child("TechTree").append_child("AvailableBuildings");
	for (list<buildingType>::iterator it = player->tech_tree->available_buildings.begin(); it != player->tech_tree->available_buildings.end(); ++it)
	{
		available_buildings.append_attribute("BuildingType") = (*it);
	}
	pugi::xml_node available_units = Player.child("TechTree").append_child("AvailableUnits");
	for (list<pair<unitType, buildingType>>::iterator it = player->tech_tree->available_units.begin(); it != player->tech_tree->available_units.end(); ++it)
	{
		pugi::xml_node unit = available_units.append_child("Unit");
		unit.append_attribute("UnitType") = (*it).first;
		unit.append_attribute("BuildingType") = (*it).second;
	}
	pugi::xml_node multipliers = Player.child("TechTree").append_child("Multipliers");
	for (vector<float>::iterator it = player->tech_tree->multiplier_list.begin(); it != player->tech_tree->multiplier_list.begin(); ++it)
	{
		multipliers.append_attribute("Multiplier") = (*it);
	}

	// ---------------------------------------
	// ------------------- AI ----------------


	pugi::xml_node Enemy = data.append_child("Enemy");
	Enemy.append_attribute("wood") = AI_faction->resources.wood;
	Enemy.append_attribute("food") = AI_faction->resources.food;
	Enemy.append_attribute("gold") = AI_faction->resources.gold;
	Enemy.append_attribute("stone") = AI_faction->resources.stone;

	Enemy.append_child("TechTree");
	pugi::xml_node available_techs2 = Enemy.child("TechTree").append_child("AvailableTechs");
	for (list<TechType>::iterator it = AI_faction->tech_tree->available_techs.begin(); it != AI_faction->tech_tree->available_techs.end(); ++it)
	{
		available_techs2.append_attribute("TechType") = (*it);
	}
	pugi::xml_node available_buildings2 = Enemy.child("TechTree").append_child("AvailableBuildings");
	for (list<buildingType>::iterator it = AI_faction->tech_tree->available_buildings.begin(); it != AI_faction->tech_tree->available_buildings.end(); ++it)
	{
		available_buildings2.append_attribute("BuildingsType") = (*it);
	}
	pugi::xml_node available_units2 = Enemy.child("TechTree").append_child("AvailableUnits");
	for (list<pair<unitType, buildingType>>::iterator it = AI_faction->tech_tree->available_units.begin(); it != AI_faction->tech_tree->available_units.end(); ++it)
	{
		pugi::xml_node unit = available_units2.append_child("Unit");
		unit.append_attribute("UnitType") = (*it).first;
		unit.append_attribute("BuildingType") = (*it).second;
	}
	pugi::xml_node multipliers2 = Enemy.child("TechTree").append_child("Multipliers");
	for (vector<float>::iterator it = AI_faction->tech_tree->multiplier_list.begin(); it != AI_faction->tech_tree->multiplier_list.begin(); ++it)
	{
		multipliers2.append_attribute("Multipliers") = (*it);
	}

	// ----------------------------------------------------

	// -----------------CREATING BUILDINGS?----------------
	pugi::xml_node create = data.append_child("BuildingCreation");

	create.append_attribute("PlacingBuilding") = placingBuilding;
	create.append_attribute("BuildingType") = placing_type;

	return true;
}

Resource * EntityManager::ReLoadResource(int posX, int posY, resourceItem type, SDL_Rect rect)
{
	iPoint ret;
	ret.x = posX;
	ret.y = posY;
	Resource* resource = new Resource(ret.x, ret.y, resourcesDB[type], rect);
	resource->entityID = nextID;
	nextID++;
	resource_list.push_back(resource);

	return resource;
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

	if (!gameData.empty())
	{
		ret = true;


		for (unitNodeInfo = gameData.child("Units").child("Unit"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Unit")) {

			Unit* unitTemplate;
			unitType type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();

			if (type == SLAVE_VILLAGER || type == ELF_VILLAGER)
				unitTemplate = (Unit*) new Villager();
			else if (type == LEGOLAS || type == GANDALF || type == BALROG)
				unitTemplate = (Unit*) new Hero();
			else
				unitTemplate = new Unit();

			unitTemplate->name = unitNodeInfo.child("Info").child("Name").attribute("value").as_string();
			unitTemplate->type = (unitType)unitNodeInfo.child("Info").child("ID").attribute("value").as_int();
			unitTemplate->selectionRadius = unitNodeInfo.child("Info").child("SelectionRadius").attribute("value").as_uint();
			unitTemplate->selectionAreaCenterPoint.x = unitNodeInfo.child("Info").child("BasePoint").attribute("x").as_int();
			unitTemplate->selectionAreaCenterPoint.y = unitNodeInfo.child("Info").child("BasePoint").attribute("y").as_int();

			string idleTexturePath = unitNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			string moveTexturePath = unitNodeInfo.child("Textures").child("Move").attribute("value").as_string();
			string attackTexturePath = unitNodeInfo.child("Textures").child("Attack").attribute("value").as_string();
			string dieTexturePath = unitNodeInfo.child("Textures").child("Die").attribute("value").as_string();


			unitTemplate->faction = (Faction)unitNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			unitTemplate->Life = unitTemplate->MaxLife = unitNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			unitTemplate->Attack = unitNodeInfo.child("Stats").child("Attack").attribute("value").as_int();
			unitTemplate->Defense = unitNodeInfo.child("Stats").child("Defense").attribute("value").as_int();
			unitTemplate->unitPiercingDamage = unitNodeInfo.child("Stats").child("PiercingDamage").attribute("value").as_int();
			unitTemplate->unitMovementSpeed = unitNodeInfo.child("Stats").child("MovementSpeed").attribute("value").as_float();
			unitTemplate->cooldown_time = unitNodeInfo.child("Stats").child("Cooldown").attribute("value").as_float();
			unitTemplate->range_value = unitNodeInfo.child("Stats").child("Range").attribute("value").as_float();
			unitTemplate->los_value = unitNodeInfo.child("Stats").child("LineOfSight").attribute("value").as_float();

			unitTemplate->cost.wood = unitNodeInfo.child("Stats").child("Cost").child("woodCost").attribute("value").as_int();
			unitTemplate->cost.stone = unitNodeInfo.child("Stats").child("Cost").child("stoneCost").attribute("value").as_int();
			unitTemplate->cost.food = unitNodeInfo.child("Stats").child("Cost").child("foodCost").attribute("value").as_int();
			unitTemplate->cost.gold = unitNodeInfo.child("Stats").child("Cost").child("goldCost").attribute("value").as_int();

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
				idle.speed = 0.2f;
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
				move.speed = 0.3f;
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
				attack.speed = unitNodeInfo.child("Stats").child("AttackSpeed").attribute("value").as_float();
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
				die.speed = 0.3f;
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

			if (unitTemplate->type == SLAVE_VILLAGER || unitTemplate->type == ELF_VILLAGER) {
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
					chop.speed = 0.3f;
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

			if (unitTemplate->type == LEGOLAS || unitTemplate->type == GANDALF || unitTemplate->type == BALROG) {

				Hero* heroTemplate = (Hero*)unitTemplate;
				heroTemplate->skill->type = (Skill_type)unitNodeInfo.child("Stats").child("SkillType").attribute("value").as_int(1);;
				heroTemplate->IsHero = true;
			}

			unitsDB.insert(pair<int, Unit*>(unitTemplate->type, unitTemplate));
		}

		constructingPhase1 = App->tex->Load(gameData.child("Buildings").child("ConstructingPhase1").attribute("value").as_string());
		constructingPhase2 = App->tex->Load(gameData.child("Buildings").child("ConstructingPhase2").attribute("value").as_string());
		constructingPhase3 = App->tex->Load(gameData.child("Buildings").child("ConstructingPhase3").attribute("value").as_string());

		for (buildingNodeInfo = gameData.child("Buildings").child("Building"); buildingNodeInfo; buildingNodeInfo = buildingNodeInfo.next_sibling("Building")) {

			Building* buildingTemplate = new Building();

			string idleTexturePath = buildingNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			buildingTemplate->entityTexture = App->tex->Load(idleTexturePath.c_str());

			buildingTemplate->name = buildingNodeInfo.child("Info").child("Name").attribute("value").as_string();
			buildingTemplate->faction = (Faction)buildingNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
			buildingTemplate->Life = buildingNodeInfo.child("Stats").child("Life").attribute("value").as_int();
			buildingTemplate->canAttack = buildingNodeInfo.child("Stats").child("CanAttack").attribute("value").as_bool();

			buildingTemplate->cost.wood = buildingNodeInfo.child("Stats").child("Cost").child("woodCost").attribute("value").as_int();
			buildingTemplate->cost.stone = buildingNodeInfo.child("Stats").child("Cost").child("stoneCost").attribute("value").as_int();
			buildingTemplate->cost.food = buildingNodeInfo.child("Stats").child("Cost").child("foodCost").attribute("value").as_int();
			buildingTemplate->cost.gold = buildingNodeInfo.child("Stats").child("Cost").child("goldCost").attribute("value").as_int();

			buildingTemplate->type = (buildingType)buildingNodeInfo.child("Info").child("ID").attribute("value").as_int();
			buildingTemplate->selectionWidth = buildingNodeInfo.child("Info").child("SelectionWidth").attribute("value").as_uint();
			buildingTemplate->selectionAreaCenterPoint.x = buildingNodeInfo.child("Info").child("BasePoint").attribute("x").as_int();
			buildingTemplate->selectionAreaCenterPoint.y = buildingNodeInfo.child("Info").child("BasePoint").attribute("y").as_int();
			buildingTemplate->GetBuildingBoundaries();

			buildingsDB.insert(pair<int, Building*>(buildingTemplate->type, buildingTemplate));
			buildingTemplate->MaxLife = buildingTemplate->Life;
		}

		for (resourceNodeInfo = gameData.child("Resources").child("Resource"); resourceNodeInfo; resourceNodeInfo = resourceNodeInfo.next_sibling("Resource"))
		{

			Resource* resourceTemplate = new Resource();

			string idleTexturePath = resourceNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
			resourceTemplate->entityTexture = App->tex->Load(idleTexturePath.c_str());

			resourceTemplate->Life = resourceNodeInfo.child("Stats").child("Life").attribute("value").as_int();

			for (pugi::xml_node rectsNode = resourceNodeInfo.child("Rects").child("Rect"); rectsNode; rectsNode = rectsNode.next_sibling("Rect")) {
				resourceTemplate->blit_rects.push_back({ rectsNode.attribute("x").as_int(), rectsNode.attribute("y").as_int(), rectsNode.attribute("w").as_int(), rectsNode.attribute("h").as_int() });
			}
			resourceTemplate->name = resourceNodeInfo.child("Info").child("Name").attribute("value").as_string();
			resourceTemplate->res_type = (resourceItem)resourceNodeInfo.child("Info").child("ID").attribute("value").as_int();
			resourceTemplate->contains = (resourceType)resourceNodeInfo.child("Info").child("Type").attribute("value").as_int();
			resourceTemplate->selectionWidth = resourceNodeInfo.child("Info").child("SelectionWidth").attribute("value").as_uint();
			resourceTemplate->selectionAreaCenterPoint.x = resourceNodeInfo.child("Info").child("BasePoint").attribute("x").as_int();
			resourceTemplate->selectionAreaCenterPoint.y = resourceNodeInfo.child("Info").child("BasePoint").attribute("y").as_int();

			resourcesDB.insert(pair<int, Resource*>(resourceTemplate->res_type, resourceTemplate));
		}

		player->tech_tree->LoadTechTree(gameData.child("FPTechs"));
		App->gui->LoadTechInfo();
		AI_faction->tech_tree->LoadTechTree(gameData.child("SATechs"));
	}

	return ret;
}

Unit* EntityManager::CreateUnit(int posX, int posY, unitType type)
{
	Unit* unit;

	if (type == SLAVE_VILLAGER || type == ELF_VILLAGER) {
		unit = (Unit*) new Villager(posX, posY, (Villager*)unitsDB[type]);

		if (unit->faction == player->faction)
			player->villagers.push_back((Villager*)unit);
		else
			AI_faction->villagers.push_back((Villager*)unit);

		if (App->render->CullingCam(unit->entityPosition))
			App->audio->PlayFx(CREATE_VILLAGER_SOUND);
	}
	else {
		if (type == LEGOLAS || type == GANDALF || type == BALROG)
		{
			unit = (Unit*) new Hero(posX, posY, (Hero*)unitsDB[type]);

			if (App->render->CullingCam(unit->entityPosition))
				App->audio->PlayFx(CREATE_HERO_SOUND);
		}

		else
		{
			unit = new Unit(posX, posY, unitsDB[type]);

			if (App->render->CullingCam(unit->entityPosition))
				App->audio->PlayFx(CREATE_UNIT_SOUND);
		}
	}

	unit->entityID = nextID;
	nextID++;

	if (unit->faction == player->faction)
		player->units.push_back(unit);
	else
		AI_faction->units.push_back(unit);

	WorldEntityList.push_back(unit);
	App->fog->AddEntity(unit);

	return unit;
}


Building* EntityManager::CreateBuilding(int posX, int posY,  buildingType type)
{
	Building* building = new Building(posX, posY, buildingsDB[type]);
	building->entityID = nextID;
	nextID++;
	if (building->faction == player->faction)
		player->buildings.push_back(building);
	else
		AI_faction->buildings.push_back(building);

	WorldEntityList.push_back(building);

	App->fog->AddEntity(building);
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
	resource_list.push_back(resource);

	return resource;
}


void EntityManager::DeleteEntity(Entity* entity)
{
	removeEntityList.push_back(entity);
	if(entity->entityType == ENTITY_RESOURCE)
		resource_list.remove((Resource*)entity);
	else
		WorldEntityList.remove(entity);
}


void EntityManager::AddResources(Villager* villager) {

	StoredResources* resources = nullptr;
	if (villager->faction == player->faction)
		resources = &player->resources;
	else
		resources = &AI_faction->resources;

	switch (villager->resource_carried) {
	case WOOD:
		resources->wood += villager->curr_capacity; break;
	case GOLD:
		resources->gold += villager->curr_capacity; break;
	case FOOD:
		resources->food += villager->curr_capacity; break;
	case STONE:
		resources->stone += villager->curr_capacity; break;
	}

	if (villager->faction == player->faction)
		App->sceneManager->level1_scene->UpdateResources();

	villager->curr_capacity = 0;

}

Resource* EntityManager::FindNearestResource(resourceType contains, iPoint pos) {

	Resource* resource = nullptr;

	for (list<Resource*>::iterator it = resource_list.begin(); it != resource_list.end(); it++) {
		if ((*it)->state != DESTROYED && (*it)->contains == contains) {

			if (resource == nullptr)
				resource = (*it);
			else if ((*it)->collider != nullptr && resource->collider != nullptr && pos.DistanceTo((*it)->collider->pos) < pos.DistanceTo(resource->collider->pos))
				resource = (*it);
		}
	}

	return resource;
}


void EntityManager::RallyCall(Entity* entity) {

	list<Unit*>* allied_units = nullptr;
	if (entity == nullptr) return;

	if (entity->faction == player->faction)
		allied_units = &player->units;
	else
		allied_units = &AI_faction->units;

	for (list<Unit*>::iterator it = allied_units->begin(); it != allied_units->end(); it++) {
		if (entity->collider->pos.DistanceTo((*it)->collider->pos) < (*it)->los->r && (*it)->type != SLAVE_VILLAGER && (*it)->type != ELF_VILLAGER)
			(*it)->order_list.push_back(new UnitAttackOrder());
	}
	
}


Building* EntityManager::FindNearestBuilding(Unit* unit) {

	list<Building*>* ally_buildings = nullptr;
	Building* ret = nullptr;

	if (unit->faction == player->faction)
		ally_buildings = &player->buildings;
	else
		ally_buildings = &AI_faction->buildings;

	iPoint aux{ -1,-1 };
	for (list<Building*>::iterator it = ally_buildings->begin(); it != ally_buildings->end(); it++) {
		if ((*it)->state == BEING_BUILT && unit->collider->pos.DistanceTo((*it)->collider->pos) < aux.DistanceTo((*it)->collider->pos)) {
			ret = (*it);
			aux = (*it)->collider->pos;
		}
	}

	return ret;
}

Entity* EntityManager::FindTarget(Entity* entity) {

	list<Unit*>* enemy_units = nullptr;
	Entity* ret = nullptr;

	if (entity->faction == player->faction)
		enemy_units = &AI_faction->units;
	else
		enemy_units = &player->units;

	iPoint aux{ -1,-1 };
	for (list<Unit*>::iterator it = enemy_units->begin(); it != enemy_units->end(); it++) {
		if ((*it)->state != DESTROYED && entity->collider->pos.DistanceTo((*it)->collider->pos) < aux.DistanceTo((*it)->collider->pos)) {
			ret = (*it);
			aux = (*it)->entityPosition;
		}
	}

	list<Building*>* enemy_buildings = nullptr;

	if (entity->faction == player->faction)
		enemy_buildings = &AI_faction->buildings;
	else
		enemy_buildings = &player->buildings;

	for (list<Building*>::iterator it = enemy_buildings->begin(); it != enemy_buildings->end(); it++) {
		if ((*it)->state != DESTROYED && entity->collider->pos.DistanceTo((*it)->collider->pos) < aux.DistanceTo((*it)->collider->pos)) {
			ret = (*it);
			aux = (*it)->collider->pos;
		}
	}


	return ret;
}


void EntityManager::DestroyEntity(Entity* entity)
{
	if (entity != nullptr) {
		list<Entity*>::iterator it = removeEntityList.begin();

		while (it != removeEntityList.end())
		{
			if (*it == entity)
			{
				removeEntityList.remove(*it);
				delete entity;
				return;
			}
			++it;
		}
	}
}

Collider* EntityManager::CheckCursorHover(iPoint cursor_pos) {

	Collider* nearest_col = nullptr;

	if(nearest_col = App->collision->FindCollider(cursor_pos, 5)){

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

		if (Collider* nearest_col = App->collision->FindCollider(mouse, 5)) {
			Unit* unit = nullptr;
			switch (nearest_col->type) {

			case COLLIDER_UNIT:

				unit = (Unit*)nearest_col->entity;
				if (click_timer.ReadSec() < 0.5 && nearest_col->entity == clicked_entity) { // double click

					for (list<Unit*>::iterator it = player->units.begin(); it != player->units.end(); it++) {

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


		for (list<Unit*>::iterator it = player->units.begin(); it != player->units.end(); it++) {
			SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

			if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
				selectedEntityList.push_back((Entity*)(*it));
				selectedCount++;
			}
		}

		if (selectedCount == 0) {

			for (list<Building*>::iterator it = player->buildings.begin(); it != player->buildings.end(); it++) {
				SDL_Point pos = { (*it)->entityPosition.x, (*it)->entityPosition.y };

				if ((bool)SDL_PointInRect(&pos, &multiSelectionRect)) {
					selectedEntityList.push_back((Entity*)(*it));
					selectedCount++;
				}
			}
		}

	}

	if (!selectedEntityList.empty()) {
		selectedListType = selectedEntityList.front()->collider->type;
		if(selectedListType == COLLIDER_UNIT)
			App->audio->PlaySelectSound((Unit*)selectedEntityList.front());
	}

	multiSelectionRect = { 0,0,0,0 };
}

void EntityManager::DrawSelectedList() {

	Unit* unit = nullptr;
	Building* building = nullptr;
	Resource* resource = nullptr;

	for (list<Entity*>::iterator it = selectedEntityList.begin(); it != selectedEntityList.end(); it++) {

		switch (selectedListType) {

		case COLLIDER_UNIT:
			unit = (Unit*)(*it);
			App->render->DrawIsometricCircle(unit->entityPosition.x, unit->entityPosition.y + (*it)->selectionAreaCenterPoint.y, unit->selectionRadius, 255, 255, 255, 255);
			break;
		case COLLIDER_BUILDING:
			building = (Building*)(*it);
			App->render->DrawIsometricRect({ (*it)->entityPosition.x, (*it)->entityPosition.y}, building->selectionWidth);
			break;
		case COLLIDER_RESOURCE:
			resource = (Resource*)(*it);
			App->render->DrawIsometricRect({ (*it)->entityPosition.x, (*it)->entityPosition.y - 15 }, resource->selectionWidth);
			break;
		}
	}
}



