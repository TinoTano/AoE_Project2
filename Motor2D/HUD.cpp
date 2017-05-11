#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Window.h"
#include "Gui.h"
#include "SceneManager.h"
#include "Window.h"
#include "Orders.h"
#include <stdlib.h>  
#include "TechTree.h"
#include "Hero.h"


//HUD
HUD::HUD()
{
}

void HUD::Start() {
	App->win->GetWindowSize(x, y);
	// 0
	buttons_positions.push_back({ (int)x / 30 - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 1
	buttons_positions.push_back({ (int)x / 15 - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 2
	buttons_positions.push_back({ (int)x / 10 - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 3
	buttons_positions.push_back({ (int)x / 7 - (int)(x / 100) - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 4
	buttons_positions.push_back({ (int)x / 6 - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 5
	buttons_positions.push_back({ (int)x / 5 - CAMERA_OFFSET_X, (int)y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	// 6
	buttons_positions.push_back({ (int)x / 30 - CAMERA_OFFSET_X, (int)y - (int)y / 7 + (int)(y / 100) - CAMERA_OFFSET_Y, 39,40 });
	//7
	buttons_positions.push_back({ (int)x / 15 - CAMERA_OFFSET_X, (int)y - (int)y / 7 + (int)(y / 100) - CAMERA_OFFSET_Y, 39,40 });
	// 8
	buttons_positions.push_back({ (int)x / 10 - CAMERA_OFFSET_X, (int)y - (int)y / 7 + (int)(y / 100) - CAMERA_OFFSET_Y, 39,40 });
	// 9
	buttons_positions.push_back({ (int)x / 7 - (int)(x / 100) - CAMERA_OFFSET_X, (int)y - (int)y / 7 + (int)(y / 100) - CAMERA_OFFSET_Y, 39,40 });
	// 10
	buttons_positions.push_back({ (int)x / 6 - CAMERA_OFFSET_X, (int)y - (int)y / 7 + (int)(y / 100) - CAMERA_OFFSET_Y, 39,40 });


	buttons_positions.push_back({ (int)x / 30 - CAMERA_OFFSET_X, (int)y - (int)y / 8 - CAMERA_OFFSET_Y, 39,40 });
	// 12

	buttons_positions.push_back({ (int)x / 4 - (int)x / 40 - CAMERA_OFFSET_X, (int)y - (int)y / 10 - CAMERA_OFFSET_Y, 39,40 });

	posx = x / 3 - x / 50;
	posy = y - (y / 6);
}


void HUD::ClearBuilding() {
	App->gui->DestroyUIElement(single);
	App->gui->DestroyUIElement(name);
	App->gui->DestroyUIElement(life);

	switch (building_state)
	{
	case BUILDINGMENU:
		HUDClearBuildingMenu();
		break;
	case BUILDINGCREATEUNITS:
		HUDClearCreateUnits();
		break;
	}
}

void HUD::ClearResource()
{
	App->gui->DestroyUIElement(single);
	App->gui->DestroyUIElement(name);
	App->gui->DestroyUIElement(life);
}

void HUD::Update() {
	Sprite bar;
	int percent;
	int barPercent;
	string life_str;
	string cd_str;

	if (App->entityManager->selectedEntityList.size() > 0) {
		if (App->entityManager->selectedEntityList.front()->Life > 0) {
			switch (App->entityManager->selectedListType) {

			case COLLIDER_UNIT:
				if (App->entityManager->selectedEntityList.size() > 1) {

					if (type != MULTIPLESELECTION)
					{
						if (type == SINGLEINFO)
							ClearSingle();
						else if (type == BUILDINGINFO)
							ClearBuilding();
						else if (type == RESOURCEINFO)
							ClearResource();

						type = MULTIPLESELECTION;
						GetSelection();

					}
					else
					{
						int x = 0, y = 0;
						for (list<UnitSprite>::iterator it_sprite = App->gui->SpriteUnits.begin(); it_sprite != App->gui->SpriteUnits.end(); ++it_sprite)
						{
							for (list<Entity*>::iterator it_unit = App->entityManager->selectedEntityList.begin(); it_unit != App->entityManager->selectedEntityList.end(); ++it_unit)
							{
								if (x >= max_width)
								{
									x = 0;
									y += App->gui->SpriteUnits.front().GetRect().h + 5;
								}
								Unit* unit = (Unit*)(*it_unit);
								if (it_sprite._Ptr->_Myval.GetID() == unit->GetType() && it_unit._Ptr->_Myval->Life > 0) {
									if (it_unit._Ptr->_Myval->MaxLife == 0) it_unit._Ptr->_Myval->MaxLife = it_unit._Ptr->_Myval->Life;
									percent = ((it_unit._Ptr->_Myval->MaxLife - it_unit._Ptr->_Myval->Life) * 100) / it_unit._Ptr->_Myval->MaxLife;
									barPercent = (percent * App->gui->SpriteUnits.front().GetRect().w) / 100;
									bar.rect.x = posx + x - App->render->camera.x;
									bar.rect.y = posy + y - 30 - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
									bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
									bar.rect.h = 5;
									bar.r = 255;
									bar.g = 0;
									bar.b = 0;
									App->render->ui_toDraw.push_back(bar);
									bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
									bar.r = 0;
									bar.g = 255;
									App->render->ui_toDraw.push_back(bar);
									x += App->gui->SpriteUnits.front().GetRect().w;
								}
							}
						}

						// CODE TO SELECT ONE UNIT FROM THE PANEL NOT FUNCTIONAL FOR NOW
						/*
						list<Unit*>::iterator it_unit = App->entityManager->selectedUnitList.begin();
						for (list<Image*>::iterator it = multiple.begin(); it != multiple.end(); ++it) {
						if (it._Ptr->_Myval->current == CLICKIN){
						App->entityManager->selectedUnitList.clear();
						App->entityManager->selectedUnitList.push_back(it_unit._Ptr->_Myval);
						type = SINGLEINFO;
						GetSelection();
						ClearMultiple();

						int size = App->entityManager->selectedUnitList.size();
						int a = 9381741;
						}
						if (it_unit != App->entityManager->selectedUnitList.end()) ++it_unit;
						}*/
					}
				}
				else {
					if (type != SINGLEINFO)
					{
						if (type == MULTIPLESELECTION)
							ClearMultiple();
						else if (type == BUILDINGINFO)
							ClearBuilding();
						else if (type == RESOURCEINFO)
							ClearResource();

						type = SINGLEINFO;
						GetSelection();
					}
					else {
						Unit* unit = (Unit*)App->entityManager->selectedEntityList.front();
						if (unit->GetType() != id) {
							ClearSingle();
							GetSelection();
						}
						else if (unit->Life > 0)
						{
							_itoa_s(unit->Defense, armor, 65, 10);
							_itoa_s(unit->Attack, damage, 65, 10);

							damage_val->SetString(damage);
							armor_val->SetString(armor);


							max_life = unit->MaxLife;
							curr_life = unit->Life;

							_itoa_s(curr_life, currlife, 65, 10);
							life_str += currlife;
							life_str += "/";
							_itoa_s(max_life, maxlife, 65, 10);
							life_str += maxlife;

							life->SetString(life_str);

							if (max_life <= 0) max_life = curr_life;
							percent = ((max_life - curr_life) * 100) / max_life;
							barPercent = (percent * App->gui->SpriteUnits.front().GetRect().w) / 100;
							bar.rect.x = posx - App->render->camera.x;
							bar.rect.y = posy - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
							bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
							bar.rect.h = 5;
							bar.r = 255;
							bar.g = 0;
							bar.b = 0;
							App->render->ui_toDraw.push_back(bar);
							bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
							bar.r = 0;
							bar.g = 255;
							App->render->ui_toDraw.push_back(bar);
							if (name->str == "ELF VILLAGER") {
								switch (villager_state) {
								case VILLAGERMENU:
									if (villager_state != VILLAGERMENU)
									{
										HUDVillagerMenu();
									}
									else {
										if (create_building_bt->current == CLICKUP)
										{
											HUDCreateBuildings();
										}
									}
									break;
								case VILLAGERCREATEBUILDINGS:
									if (villager_state != VILLAGERCREATEBUILDINGS)
									{
										HUDCreateBuildings();
									}
									else {
										if (cancel_bt->current == CLICKUP)
											HUDVillagerMenu();
										else if (create_town_center_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, TOWN_CENTER);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_house_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, HOUSE);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_stables_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, STABLES);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_archery_range_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, ARCHERY_RANGE);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_siege_workshop_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, SIEGE_WORKSHOP);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_market_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, MARKET);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_blacksmith_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, BLACKSMITH);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_mill_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, FARM);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_outpost_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, OUTPOST);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_monastery_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, MONASTERY);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										else if (create_castle_bt->current == CLICKUP) {
											if (!App->entityManager->placingBuilding) {
												int x, y;
												App->input->GetMousePosition(x, y);
												App->entityManager->buildingToCreate = App->entityManager->CreateBuilding(x - App->render->camera.x, y - App->render->camera.y, CASTLE);
												App->entityManager->buildingToCreate->collider->type = COLLIDER_CREATING_BUILDING;
												App->entityManager->placingBuilding = true;
												App->entityManager->buildingToCreate->waitingToPlace = true;
												App->entityManager->buildingToCreate->faction = FREE_MEN; //temporal for testing
											}
										}
										// AND SO ON
									}
									break;
								}
							}
							else {
								Unit* unit = (Unit*)App->entityManager->selectedEntityList.front();
								if (unit->IsHero) {
									Hero* hero = (Hero*)unit;

									max_life = hero->skill->cooldown;
									curr_life = (int)hero->skill_timer.ReadSec();
									if (curr_life >= max_life) curr_life = max_life;
									_itoa_s(curr_life, currlife, 65, 10);
									life_str += "     ";
									life_str += currlife;
									life_str += "/";
									_itoa_s(max_life, maxlife, 65, 10);
									life_str += maxlife;
									life_str += "(sec)";
									life->SetString(life_str);
									if (max_life == 0) max_life = curr_life;
									percent = ((max_life - curr_life) * 100) / max_life;
									barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;
									bar.rect.y = bar.rect.y + bar.rect.h;
									bar.rect.h = 5;
									bar.r = 200;
									bar.g = 200;
									bar.b = 200;
									App->render->ui_toDraw.push_back(bar);
									bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
									bar.r = 0;
									bar.b = 255;
									bar.g = 100;
									App->render->ui_toDraw.push_back(bar);


									switch (hero_state) {
									case VILLAGERMENU:
										if (hero_state != HEROMENU)
										{
											HUDHeroMenu();
										}
										else {
											if (hero->skill->ready) {
												skill_bt->enabled = true;
											}
											else skill_bt->enabled = false;
											if (skill_bt->current == CLICKUP)
											{
												hero->skill->Activate(hero);
											}
										}
										break;
									}
								}
							}
						}
					}
					break;
			case COLLIDER_BUILDING:

				if (type != BUILDINGINFO)
				{
					ClearSingle();
					ClearMultiple();
					ClearResource();
					type = BUILDINGINFO;
					StartBuildingInfo();
				}
				else {
					Building* building = (Building*)App->entityManager->selectedEntityList.front();
					if (building->type != id) {
						ClearBuilding();
						StartBuildingInfo();
					}
					for (list<UnitSprite>::iterator it = App->gui->SpriteBuildings.begin(); it != App->gui->SpriteBuildings.end(); ++it)
					{
						if (it._Ptr->_Myval.GetID() == building->type)
						{
							name->SetString(it._Ptr->_Myval.GetName());
						}
					}
					max_life = App->entityManager->selectedEntityList.front()->MaxLife;
					curr_life = App->entityManager->selectedEntityList.front()->Life;
					_itoa_s(curr_life, currlife, 65, 10);
					life_str += currlife;
					life_str += "/";
					_itoa_s(max_life, maxlife, 65, 10);
					life_str += maxlife;
					life->SetString(life_str);
					if (max_life == 0) max_life = curr_life;
					percent = ((max_life - curr_life) * 100) / max_life;
					barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;
					bar.rect.x = posx - App->render->camera.x;
					bar.rect.y = posy - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
					bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
					bar.rect.h = 5;
					bar.r = 255;
					bar.g = 0;
					bar.b = 0;
					App->render->ui_toDraw.push_back(bar);
					bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
					bar.r = 0;
					bar.g = 255;
					App->render->ui_toDraw.push_back(bar);
					if (name->str == "TOWN CENTER") {
						switch (building_state) {
						case BUILDINGMENU:
							if (building_state != BUILDINGMENU)
							{
								HUDBuildingMenu();
							}
							else {
								if (create_unit_bt->current == CLICKUP)
								{
									HUDCreateUnits();
								}
								else if (create_villager_bt->current == CLICKUP) {
									if (App->entityManager->player->resources.Spend(App->entityManager->unitsDB[ELF_VILLAGER]->cost)){
										App->sceneManager->level1_scene->UpdateVillagers(++App->sceneManager->level1_scene->villagers_curr, ++App->sceneManager->level1_scene->villagers_total);
										create_villager_bt->current = FREE;
										App->sceneManager->level1_scene->UpdateResources();
										building->order_list.push_back(new CreateUnitOrder(ELF_VILLAGER));
									}
								}
								else if (create_hero_bt->current == CLICKUP) {
									HUDCreateHero();
								}
							}
							break;
						case BUILDINGCREATEUNITS:
							if (building_state != BUILDINGCREATEUNITS)
							{
								HUDCreateUnits();
							}
							else {
								if (cancel_bt->current == CLICKUP)
									HUDBuildingMenu();
								else if (create_elven_archer_bt->current == CLICKUP)
								{
									if (App->entityManager->player->resources.Spend(App->entityManager->unitsDB[ELVEN_ARCHER]->cost)) {
										building->order_list.push_back(new CreateUnitOrder(ELVEN_ARCHER));
										App->sceneManager->level1_scene->UpdateResources();
										create_elven_archer_bt->current = FREE;
									}
								}
								else if (create_elven_longblade_bt->current == CLICKUP)
								{
									if (App->entityManager->player->resources.Spend(App->entityManager->unitsDB[ELVEN_LONGBLADE]->cost)){
										building->order_list.push_back(new CreateUnitOrder(ELVEN_LONGBLADE));
										App->sceneManager->level1_scene->UpdateResources();
										create_elven_longblade_bt->current = FREE;
									}
								}
								else if (create_elven_cavalry_bt->current == CLICKUP)
								{
									if (App->entityManager->player->resources.Spend(App->entityManager->unitsDB[GONDOR_HERO]->cost)){
										building->order_list.push_back(new CreateUnitOrder(GONDOR_HERO));
										App->sceneManager->level1_scene->UpdateResources();
										create_elven_cavalry_bt->current = FREE;
									}
								}
							}
							break;
						case BUILDINGCREATEHERO:
							if (building_state != BUILDINGCREATEHERO)
							{
								HUDCreateHero();
							}
							if (cancel_bt->current == CLICKUP)
								HUDBuildingMenu();
							else if (create_Legolas_bt->current == CLICKUP)
							{
								Order* new_order = new CreateUnitOrder(LEGOLAS);
								building->order_list.push_back(new_order);
							//	App->sceneManager->level1_scene->UpdateResources(App->sceneManager->level1_scene->wood, App->sceneManager->level1_scene->woodCount -= 350);
							}
							break;
						}
					}
				}
				break;
			case COLLIDER_RESOURCE:
				if (type != RESOURCEINFO)
				{
					ClearSingle();
					ClearMultiple();
					ClearBuilding();
					type = RESOURCEINFO;
					StartResourceInfo();
				}
				else {
					Resource* resource = (Resource*)App->entityManager->selectedEntityList.front();
					if (resource->type != id) {
						ClearResource();
						StartResourceInfo();
					}
					for (list<UnitSprite>::iterator it = App->gui->SpriteResources.begin(); it != App->gui->SpriteResources.end(); ++it)
					{
						if (it._Ptr->_Myval.GetID() == resource->type)
						{
							name->SetString(it._Ptr->_Myval.GetName());
						}
					}

					max_life = App->entityManager->selectedEntityList.front()->MaxLife;
					curr_life = App->entityManager->selectedEntityList.front()->Life;
					_itoa_s(curr_life, currlife, 65, 10);
					life_str += currlife;
					life_str += "/";
					_itoa_s(max_life, maxlife, 65, 10);
					life_str += maxlife;
					life->SetString(life_str);
					if (max_life == 0) max_life = curr_life;
					percent = ((max_life - curr_life) * 100) / max_life;
					barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;
					bar.rect.x = posx - App->render->camera.x;
					bar.rect.y = posy - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
					bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
					bar.rect.h = 5;
					bar.r = 255;
					bar.g = 0;
					bar.b = 0;
					App->render->ui_toDraw.push_back(bar);
					bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
					bar.r = 0;
					bar.g = 255;
					App->render->ui_toDraw.push_back(bar);
				}
				break;
				}
			}
		}
	}
	else {

		if (type != NONE)
		{
			if (type == SINGLEINFO)
				ClearSingle();
			else if (type == MULTIPLESELECTION)
				ClearMultiple();
			else if (type == BUILDINGINFO)
				ClearBuilding();
			else if (type == RESOURCEINFO)
				ClearResource();
			type = NONE;
		}
	}
}


void HUD::HUDVillagerMenu()
{
	switch (villager_state) {
	case VILLAGERCREATEBUILDINGS:
		HUDClearCreateBuildings();
		break;
	}
	// THIS MUST BE DONE HERE
	villager_state = VILLAGERMENU;

	vector<SDL_Rect> blit_sections;

	blit_sections.push_back({ 130, 24, 39, 40 });
	blit_sections.push_back({ 169, 24, 39, 40 });

	create_building_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

}

void HUD::HUDClearVillagerMenu()
{
	App->gui->DestroyUIElement(create_building_bt);
}

void HUD::HUDHeroMenu()
{
	switch (hero_state) {
	}
	hero_state = HEROMENU;

	vector<SDL_Rect> blit_sections;

	blit_sections.push_back({ 210, 64, 39, 40 });
	blit_sections.push_back({ 249, 64, 39, 40 });

	skill_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
}

void HUD::HUDClearHeroMenu()
{
	App->gui->DestroyUIElement(skill_bt);
}

void HUD::HUDCreateBuildings()
{
	switch (villager_state) {
	case VILLAGERMENU:
		HUDClearVillagerMenu();
		break;
	}

	villager_state = VILLAGERCREATEBUILDINGS;

	vector<SDL_Rect> blit_sections;

	blit_sections.push_back({ 0, 0, 33, 32 });
	blit_sections.push_back({ 0, 0, 33, 32 });

	create_town_center_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 33, 0, 33, 32 });
	blit_sections.push_back({ 33, 0, 33, 32 });

	create_house_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 99, 0, 33, 32 });
	blit_sections.push_back({ 99, 0, 33, 32 });

	create_archery_range_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 132, 0, 33, 32 });
	blit_sections.push_back({ 132, 0, 33, 32 });

	create_stables_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[3].x - CAMERA_OFFSET_X, buttons_positions[3].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 165, 0, 33, 32 });
	blit_sections.push_back({ 165, 0, 33, 32 });

	create_siege_workshop_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[4].x - CAMERA_OFFSET_X, buttons_positions[4].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 198, 0, 33, 32 });
	blit_sections.push_back({ 198, 0, 33, 32 });

	create_market_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[5].x - CAMERA_OFFSET_X, buttons_positions[5].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 231, 0, 33, 32 });
	blit_sections.push_back({ 231, 0, 33, 32 });

	create_blacksmith_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[6].x - CAMERA_OFFSET_X, buttons_positions[6].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 264, 0, 33, 32 });
	blit_sections.push_back({ 264, 0, 33, 32 });

	create_mill_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[7].x - CAMERA_OFFSET_X, buttons_positions[7].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 66, 32, 33, 32 });
	blit_sections.push_back({ 66, 32, 33, 32 });

	create_outpost_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[8].x - CAMERA_OFFSET_X, buttons_positions[8].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 99, 32, 33, 32 });
	blit_sections.push_back({ 99, 32, 33, 32 });

	create_monastery_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[9].x - CAMERA_OFFSET_X, buttons_positions[9].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 132, 32, 33, 32 });
	blit_sections.push_back({ 132, 32, 33, 32 });

	create_castle_bt = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[10].x - CAMERA_OFFSET_X, buttons_positions[10].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);


	blit_sections.clear();
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
}

void HUD::HUDClearCreateBuildings()
{
	App->gui->DestroyUIElement(create_town_center_bt);
	App->gui->DestroyUIElement(create_house_bt);
	App->gui->DestroyUIElement(create_archery_range_bt);
	App->gui->DestroyUIElement(create_stables_bt);
	App->gui->DestroyUIElement(create_siege_workshop_bt);
	App->gui->DestroyUIElement(create_market_bt);
	App->gui->DestroyUIElement(create_blacksmith_bt);
	App->gui->DestroyUIElement(create_mill_bt);
	App->gui->DestroyUIElement(create_outpost_bt);
	App->gui->DestroyUIElement(create_monastery_bt);
	App->gui->DestroyUIElement(create_castle_bt);
	App->gui->DestroyUIElement(cancel_bt);
}

void HUD::HUDBuildingMenu()
{
	switch (building_state) {
	case BUILDINGCREATEUNITS:
		HUDClearCreateUnits();
		break;
	case BUILDINGCREATEHERO:
		HUDClearCreateHero();
		break;
	}

	// THIS MUST BE DONE HERE
	building_state = BUILDINGMENU;
	vector<SDL_Rect> blit_sections;

	if (name->str == "TOWN CENTER") {

		blit_sections.push_back({ 130, 64, 39, 40 });
		blit_sections.push_back({ 169, 64, 39, 40 });

		create_unit_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
		blit_sections.clear();

		blit_sections.push_back({ 208, 24, 39, 40 });
		blit_sections.push_back({ 247, 24, 39, 40 });

		create_hero_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
		blit_sections.clear();
		blit_sections.push_back({ 52, 24, 39, 40 });
		blit_sections.push_back({ 91, 24, 39, 40 });

		create_villager_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
		blit_sections.clear();
	}
	if (tech) {
		uint tech_place = TECH_UI_BUTTON;
		uint x = 0;
		for (uint i = 0; i < tech_rects.size(); ++i) {
			blit_sections.push_back(tech_rects[x]);
			tech_rects[x].x += tech_rects[x].w;
			tech_bt.push_back((Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[tech_place].x - CAMERA_OFFSET_X, buttons_positions[tech_place].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2));
			tech_place++;
			x++;
		}
	}
}

void HUD::HUDClearBuildingMenu()
{
	App->gui->DestroyUIElement(create_unit_bt);
	App->gui->DestroyUIElement(create_hero_bt);
	App->gui->DestroyUIElement(create_villager_bt);
	tech = false;
	tech_rects.clear();
	for (list<Button*>::iterator it = tech_bt.begin(); it != tech_bt.end(); ++it) {
	App->gui->DestroyUIElement(it._Ptr->_Myval);
	}
}

void HUD::HUDCreateHero()
{
	switch (building_state) {
	case BUILDINGMENU:
		HUDClearBuildingMenu();
		break;
	case BUILDINGCREATEUNITS:
		HUDClearCreateUnits();
		break;
	}
	building_state = BUILDINGCREATEHERO;

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 0, 0, 33, 32 });
	blit_sections.push_back({ 0, 0, 33, 32 });

	create_Legolas_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 53, 64, 39, 40 });
	blit_sections.push_back({ 92, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
}

void HUD::HUDClearCreateHero()
{
	App->gui->DestroyUIElement(cancel_bt);
	App->gui->DestroyUIElement(create_Legolas_bt);
}


void HUD::HUDCreateUnits()
{
	switch (building_state) {
	case BUILDINGMENU:
		HUDClearBuildingMenu();
		break;
	case BUILDINGCREATEHERO:
		HUDClearCreateHero();
		break;
	}
	// THIS MUST BE DONE HERE
	building_state = BUILDINGCREATEUNITS;

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 99, 0, 33, 32 });
	blit_sections.push_back({ 99, 0, 33, 32 });

	create_elven_archer_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 0, 0, 33, 32 });
	blit_sections.push_back({ 0, 0, 33, 32 });

	create_elven_longblade_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 165, 0, 33, 32 });
	blit_sections.push_back({ 165, 0, 33, 32 });

	create_elven_cavalry_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

}

void HUD::HUDClearCreateUnits()
{
	App->gui->DestroyUIElement(create_elven_archer_bt);
	App->gui->DestroyUIElement(create_elven_longblade_bt);
	App->gui->DestroyUIElement(create_elven_cavalry_bt);
	App->gui->DestroyUIElement(cancel_bt);
}

void HUD::StartResourceInfo()
{
	char currlife[65], maxlife[65];
	string life_str;

	Resource* resource = (Resource*)App->entityManager->selectedEntityList.front();

	for (list<UnitSprite>::iterator it = App->gui->SpriteResources.begin(); it != App->gui->SpriteResources.end(); ++it)
	{
		if (it._Ptr->_Myval.GetID() == resource->type)
		{
			id = resource->type;
			single = (Image*)App->gui->CreateImage("gui/ResourcesMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
			name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 20 - App->render->camera.y, nullptr);
		}
	}

	max_life = App->entityManager->selectedEntityList.front()->Life;
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;

	life = (Label*)App->gui->CreateLabel(life_str, posx + 50 - App->render->camera.x, posy + 35 - App->render->camera.y, nullptr);
}

SDL_Rect HUD::GetTechRect(uint id)
{

	SDL_Rect ret = { 130,144,39,40 };

	switch (id) {
	case 0:
		break;
	case 1:
		ret.x = 208;
		break;
	case 2:
		ret.x = 286;
		break;
	default:
		ret.y = 184;
		for (uint it = 0, c = 1; it < id; it++) {
			ret.x += 39 * 2;
			if (c == 4) {
				ret.x = 130;
				ret.y += 40;
				c = 0;
			}
			c++;
		}
	}
	return ret;
}


void HUD::StartBuildingInfo()
{
	char currlife[65], maxlife[65];
	string life_str;

	Building* building = (Building*)App->entityManager->selectedEntityList.front();

	for (list<UnitSprite>::iterator it = App->gui->SpriteBuildings.begin(); it != App->gui->SpriteBuildings.end(); ++it)
	{
		if (it._Ptr->_Myval.GetID() == building->type)
		{
			id = building->type;
			single = (Image*)App->gui->CreateImage("gui/BuildingMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
			name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 20 - App->render->camera.y, nullptr);
		}
	}

	max_life = App->entityManager->selectedEntityList.front()->MaxLife;
	curr_life = App->entityManager->selectedEntityList.front()->Life;


	_itoa_s(curr_life, currlife, 65, 10);
	life_str += currlife;
	life_str += "/";
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;

	life = (Label*)App->gui->CreateLabel(life_str, posx + 50 - App->render->camera.x, posy + 35 - App->render->camera.y, nullptr);
	
	for (uint i = 0; i < App->entityManager->ally_techtree->all_techs.size(); ++i) {
		for (list<TechType>::iterator it = App->entityManager->ally_techtree->available_techs.begin(); it != App->entityManager->ally_techtree->available_techs.end(); ++it) {
			if (it._Ptr->_Myval == App->entityManager->ally_techtree->all_techs[i]->id && App->entityManager->ally_techtree->all_techs[i]->researched_in && id) {
				tech_rects.push_back(GetTechRect(i));
				tech = true;
			}
		}
	}
		HUDBuildingMenu();
}


void HUD::GetSelection() {
	char armor[65], damage[65], currlife[65], maxlife[65];
	string life_str;
	Unit* unit = nullptr;

	switch (type) {
	case NONE:
		break;
	case SINGLEINFO:
		unit = (Unit*)App->entityManager->selectedEntityList.front();

		for (list<UnitSprite>::iterator it = App->gui->SpriteUnits.begin(); it != App->gui->SpriteUnits.end(); ++it)
		{
			if (it._Ptr->_Myval.GetID() == unit->GetType())
			{
				id = unit->GetType();
				single = (Image*)App->gui->CreateImage("gui/UnitMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
				name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 25 - App->render->camera.y, nullptr);
			}
		}

		_itoa_s(App->entityManager->selectedEntityList.front()->Defense, armor, 65, 10);
		_itoa_s(App->entityManager->selectedEntityList.front()->Attack, damage, 65, 10);

		sword_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", posx - App->render->camera.x, posy + 50 - App->render->camera.y, SDL_Rect{ 0,19, 38, 22 });
		armor_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", posx - App->render->camera.x, posy + 75 - App->render->camera.y, SDL_Rect{ 0,63, 37, 19 });

		damage_val = (Label*)App->gui->CreateLabel(damage, posx + 50 - App->render->camera.x, posy + 50 - App->render->camera.y, nullptr);
		armor_val = (Label*)App->gui->CreateLabel(armor, posx + 50 - App->render->camera.x, posy + 75 - App->render->camera.y, nullptr);

		max_life = App->entityManager->selectedEntityList.front()->MaxLife;
		curr_life = App->entityManager->selectedEntityList.front()->Life;


		_itoa_s(curr_life, currlife, 65, 10);
		life_str += currlife;
		life_str += "/";
		_itoa_s(max_life, maxlife, 65, 10);
		life_str += maxlife;

		life = (Label*)App->gui->CreateLabel(life_str, posx + 50 - App->render->camera.x, posy + 35 - App->render->camera.y, nullptr);
		if (name->str == "ELF VILLAGER") {
			HUDVillagerMenu();
		}
		else if (unit->IsHero) {
			HUDHeroMenu();
		}
		break;
	case MULTIPLESELECTION:
		int x = 0, y = 0;
		max_width = 700;
		for (list<UnitSprite>::iterator it_sprite = App->gui->SpriteUnits.begin(); it_sprite != App->gui->SpriteUnits.end(); ++it_sprite)
		{
			for (list<Entity*>::iterator it_unit = App->entityManager->selectedEntityList.begin(); it_unit != App->entityManager->selectedEntityList.end(); ++it_unit)
			{
				unit = (Unit*)(*it_unit);
				if (x >= max_width)
				{
					x = 0;
					y += App->gui->SpriteUnits.front().GetRect().h + 5;
				}
				if (it_sprite._Ptr->_Myval.GetID() == unit->GetType())
				{
					Image* unit = (Image*)App->gui->CreateImage("gui/UnitMiniatures.png", posx - App->render->camera.x + x, posy - 30 - App->render->camera.y + y, it_sprite._Ptr->_Myval.GetRect());
					x += App->gui->SpriteUnits.front().GetRect().w;
					multiple.push_back(unit);
				}
			}

		}

		break;
	}
}


void HUD::ClearMultiple()
{
	for (list<Image*>::iterator it = multiple.begin(); it != multiple.end(); ++it)
	{
		if (it._Ptr->_Myval != nullptr)
		{
			App->tex->UnLoad(it._Ptr->_Myval->texture);
			App->gui->DestroyUIElement(it._Ptr->_Myval);
		}
	}
}

void HUD::ClearSingle()
{
	if (single != nullptr)
	{
		App->gui->DestroyUIElement(single);
	}
	if (name != nullptr) {
		App->gui->DestroyUIElement(name);
	}
	if (sword_img != nullptr) {
		App->gui->DestroyUIElement(sword_img);
	}
	if (armor_img != nullptr) {
		App->gui->DestroyUIElement(armor_img);
	}
	if (damage_val != nullptr) {
		App->gui->DestroyUIElement(damage_val);
	}
	if (armor_val != nullptr) {
		App->gui->DestroyUIElement(armor_val);
	}
	if (life != nullptr) {
		App->gui->DestroyUIElement(life);
	}

	switch (villager_state)
	{
	case VILLAGERMENU:
		HUDClearVillagerMenu();
		break;
	case VILLAGERCREATEBUILDINGS:
		HUDClearCreateBuildings();
		break;
	}
	switch (hero_state)
	{
	case HEROMENU:
		HUDClearHeroMenu();
	}
}

void HUD::CleanUp()
{
	ClearMultiple();
	ClearSingle();
	type = NONE;
}

bool Gui::LoadHUDData()
{
	bool ret = false;
	pugi::xml_document HUDDataFile;
	pugi::xml_node HUDData;
	pugi::xml_node unitNodeInfo;
	pugi::xml_node buildingNodeInfo;
	pugi::xml_node resourceNodeInfo;

	HUDData = App->LoadHUDDataFile(HUDDataFile);

	if (HUDData.empty() == false)
	{
		for (unitNodeInfo = HUDData.child("Images").child("HUD"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("HUD")) {
			string name(unitNodeInfo.child("Info").child("Name").attribute("value").as_string());
			uint id = unitNodeInfo.child("Info").child("ID").attribute("value").as_uint();
			pair<int, int> position;
			position.first = unitNodeInfo.child("Info").child("Position").attribute("x").as_uint();
			position.second = unitNodeInfo.child("Info").child("Position").attribute("y").as_uint();
			string path(unitNodeInfo.child("Texture").child("Path").attribute("value").as_string());
			SDL_Rect rect;
			rect.x = unitNodeInfo.child("Texture").child("Rect").attribute("x").as_int();
			rect.y = unitNodeInfo.child("Texture").child("Rect").attribute("y").as_int();
			rect.w = unitNodeInfo.child("Texture").child("Rect").attribute("w").as_int();
			rect.h = unitNodeInfo.child("Texture").child("Rect").attribute("h").as_int();
			string scene(unitNodeInfo.child("Scenes").child("Scene").attribute("value").as_string());
			Info curr(name, id, position, path, rect, scene, IMAGE);
			info.push_back(curr);
		}
		for (unitNodeInfo = HUDData.child("Buttons").child("HUD"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("HUD")) {
			string name(unitNodeInfo.child("Info").child("Name").attribute("value").as_string());
			uint id = unitNodeInfo.child("Info").child("ID").attribute("value").as_uint();
			uint tier = unitNodeInfo.child("Info").child("Tier").attribute("value").as_uint();
			pair<int, int> position;
			position.first = unitNodeInfo.child("Info").child("Position").attribute("x").as_uint();
			position.second = unitNodeInfo.child("Info").child("Position").attribute("y").as_uint();
			string path(unitNodeInfo.child("Texture").child("Path").attribute("value").as_string());
			string scene(unitNodeInfo.child("Scenes").child("Scene").attribute("value").as_string());
			vector<SDL_Rect> blit;
			for (pugi::xml_node NodeInfo = unitNodeInfo.child("Blit").child("Section"); NodeInfo; NodeInfo = NodeInfo.next_sibling("Section"))
			{
				SDL_Rect rect;
				rect.x = NodeInfo.attribute("x").as_int();
				rect.y = NodeInfo.attribute("y").as_int();
				rect.w = NodeInfo.attribute("w").as_int();
				rect.h = NodeInfo.attribute("h").as_int();
				blit.push_back(rect);
			}
			vector<SDL_Rect> detect;
			for (pugi::xml_node NodeInfo = unitNodeInfo.child("Detect").child("Section"); NodeInfo; NodeInfo = NodeInfo.next_sibling("Section"))
			{
				SDL_Rect rect;
				rect.x = NodeInfo.attribute("x").as_int();
				rect.y = NodeInfo.attribute("y").as_int();
				rect.w = NodeInfo.attribute("w").as_int();
				rect.h = NodeInfo.attribute("h").as_int();
				detect.push_back(rect);
			}
			Info curr(name, id, position, path, { 0,0,0,0 }, scene, BUTTON);
			curr.tier = (ButtonTier)tier;
			curr.blit_sections = blit;
			curr.detect_sections = detect;
			info.push_back(curr);
		}
		SDL_Rect proportions;
		proportions.w = HUDData.child("Sprites").child("Proportions").attribute("width").as_uint();
		proportions.h = HUDData.child("Sprites").child("Proportions").attribute("height").as_uint();

		for (unitNodeInfo = HUDData.child("Units").child("Unit"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Unit"))
		{
			EntityType type = UNIT;
			string name(unitNodeInfo.child("Name").attribute("value").as_string());

			int id = unitNodeInfo.child("ID").attribute("value").as_int();
			proportions.x = unitNodeInfo.child("Position").attribute("x").as_int();
			proportions.y = unitNodeInfo.child("Position").attribute("y").as_int();

			UnitSprite unit(type, proportions, id, name);
			SpriteUnits.push_back(unit);
		}
		for (unitNodeInfo = HUDData.child("Buildings").child("Building"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Building"))
		{
			EntityType type = BUILDING;
			string name(unitNodeInfo.child("Name").attribute("value").as_string());

			int id = unitNodeInfo.child("ID").attribute("value").as_int();
			proportions.x = unitNodeInfo.child("Position").attribute("x").as_int();
			proportions.y = unitNodeInfo.child("Position").attribute("y").as_int();

			UnitSprite unit(type, proportions, id, name);
			SpriteBuildings.push_back(unit);
		}
		for (unitNodeInfo = HUDData.child("Resources").child("Resource"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Resource"))
		{
			EntityType type = RESOURCE;
			string name(unitNodeInfo.child("Name").attribute("value").as_string());

			int id = unitNodeInfo.child("ID").attribute("value").as_int();
			proportions.x = unitNodeInfo.child("Position").attribute("x").as_int();
			proportions.y = unitNodeInfo.child("Position").attribute("y").as_int();

			UnitSprite unit(type, proportions, id, name);
			SpriteResources.push_back(unit);
		}


	}
	return ret;
}