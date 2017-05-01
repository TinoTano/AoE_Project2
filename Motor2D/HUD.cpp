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


//HUD
HUD::HUD()
{
}

void HUD::Start() {
	App->win->GetWindowSize(x, y);

	buttons_positions.push_back({ (int)x / 40 - CAMERA_OFFSET_X, (int) y - (int)y / 5 - CAMERA_OFFSET_Y, 39,40 });
	buttons_positions.push_back({ (int)x / 15 - CAMERA_OFFSET_X, (int) y - (int) y / 5 - CAMERA_OFFSET_Y, 39,40 });
	buttons_positions.push_back({ (int) x / 10 - CAMERA_OFFSET_X, (int) y -  (int) y / 5 - CAMERA_OFFSET_Y, 39,40 });

	buttons_positions.push_back({ (int) x / 8 - CAMERA_OFFSET_X, (int) y - (int) y / 8 - CAMERA_OFFSET_Y, 39,40 });

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

void HUD::Update() {
	Sprite bar;
	int percent;
	int barPercent;
	string life_str;

	if (App->entityManager->selectedEntityList.size() > 0) {
		switch (App->entityManager->selectedListType) {

		case COLLIDER_UNIT:
			if (App->entityManager->selectedEntityList.size() > 1) {


				if (type != MULTIPLESELECTION)
				{
					if (type == SINGLEINFO)
						ClearSingle();
					else if (type == BUILDINGINFO)
						ClearBuilding();

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
							if (it_sprite._Ptr->_Myval.GetID() == unit->GetType()) {

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
				break;

			}
			else {

				if (type != SINGLEINFO)
				{
					if (type == MULTIPLESELECTION)
						ClearMultiple();
					else if (type == BUILDINGINFO)
						ClearBuilding();

					type = SINGLEINFO;
					GetSelection();
				}
				else {
					for (list<UnitSprite>::iterator it = App->gui->SpriteUnits.begin(); it != App->gui->SpriteUnits.end(); ++it)
					{
						/*if (it._Ptr->_Myval.GetID() == App->entityManager->selectedUnitList.front()->GetType())
						{
						single->section = it->GetRect();
						name->SetString(it->GetName());
						defense = App->entityManager->selectedUnitList.front()->Defense;
						attack = App->entityManager->selectedUnitList.front()->Defense;
						}*/
					}

					_itoa_s(App->entityManager->selectedEntityList.front()->Defense, armor, 65, 10);
					_itoa_s(App->entityManager->selectedEntityList.front()->Attack, damage, 65, 10);

					damage_val->SetString(damage);
					armor_val->SetString(armor);

					max_life = App->entityManager->selectedEntityList.front()->MaxLife;
					curr_life = App->entityManager->selectedEntityList.front()->Life;

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
				}
			}
			break;

		case COLLIDER_BUILDING:

			if (type != BUILDINGINFO)
			{
				ClearSingle();
				ClearMultiple();
				type = BUILDINGINFO;
				StartBuildingInfo();
			}
			else {
				Building* building = (Building*)App->entityManager->selectedEntityList.front();

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
							if (create_unit_bt->current == CLICKIN)
							{
								HUDCreateUnits();
							}
							else if (create_villager_bt->current == CLICKIN) {
								if (App->sceneManager->level1_scene->woodCount > 50) {
									App->sceneManager->level1_scene->UpdateVillagers(++App->sceneManager->level1_scene->villagers_curr, ++App->sceneManager->level1_scene->villagers_total);
									create_villager_bt->current = FREE;
									App->sceneManager->level1_scene->UpdateResources(App->sceneManager->level1_scene->wood, App->sceneManager->level1_scene->woodCount -= 50);
									Order* new_order = new CreateUnitOrder(ELVEN_ARCHER);
									building->order_list.push_front(new_order);
								}
							}
						}

						break;
					case BUILDINGCREATEUNITS:
						if (building_state != BUILDINGCREATEUNITS)
						{
							HUDCreateUnits();
						}
						else {
							if (cancel_bt->current == CLICKIN)
								HUDBuildingMenu();
							else if (create_elven_archer_bt->current == CLICKIN)
							{
								if (App->sceneManager->level1_scene->woodCount > 70) {
									Order* new_order = new CreateUnitOrder(ELVEN_ARCHER);
									building->order_list.push_front(new_order);
									App->sceneManager->level1_scene->UpdateResources(App->sceneManager->level1_scene->wood, App->sceneManager->level1_scene->woodCount -= 70);
									create_elven_archer_bt->current = FREE;
								}
							}
							else if (create_elven_longblade_bt->current == CLICKIN)
							{
								if (App->sceneManager->level1_scene->woodCount > 70) {
									Order* new_order = new CreateUnitOrder(ELVEN_LONGBLADE);
									building->order_list.push_front(new_order);
									App->sceneManager->level1_scene->UpdateResources(App->sceneManager->level1_scene->wood, App->sceneManager->level1_scene->woodCount -= 70);
									create_elven_longblade_bt->current = FREE;
								}
							}
							else if (create_elven_cavalry_bt->current == CLICKIN)
							{
								if (App->sceneManager->level1_scene->woodCount > 350) {
									Order* new_order = new CreateUnitOrder(GONDOR_HERO);
									building->order_list.push_front(new_order);
									App->sceneManager->level1_scene->UpdateResources(App->sceneManager->level1_scene->wood, App->sceneManager->level1_scene->woodCount -= 350);
									create_elven_cavalry_bt->current = FREE;
								}
							}
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
			break;

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

			type = NONE;
		}
	}
}


void HUD::HUDBuildingMenu()
{
	switch (building_state) {
	case BUILDINGCREATEUNITS:
		HUDClearCreateUnits();
		break;
	}

	// THIS MUST BE DONE HERE
	building_state = BUILDINGMENU;

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 130, 64, 39, 40 });
	blit_sections.push_back({ 169, 64, 39, 40 });

	create_unit_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
	blit_sections.clear();


	blit_sections.push_back({ 52, 24, 39, 40 });
	blit_sections.push_back({ 91, 24, 39, 40 });

	create_villager_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
	// THIS MUST BE DONE HERE
	building_state = BUILDINGMENU;
}

void HUD::HUDClearBuildingMenu()
{
	App->gui->DestroyUIElement(create_unit_bt);
	App->gui->DestroyUIElement(create_villager_bt);
}


void HUD::HUDCreateUnits()
{
	switch (building_state) {
	case BUILDINGMENU:
		HUDClearBuildingMenu();
		break;
	}
	// THIS MUST BE DONE HERE
	building_state = BUILDINGCREATEUNITS;

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 0, 0, 33, 32 });
	blit_sections.push_back({ 0, 0, 33, 32 });

	create_elven_archer_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 33, 0, 33, 32 });
	blit_sections.push_back({ 33, 0, 33, 32 });

	create_elven_longblade_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 66, 0, 33, 32 });
	blit_sections.push_back({ 66, 0, 33, 32 });

	create_elven_cavalry_bt = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[3].x - CAMERA_OFFSET_X, buttons_positions[3].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);



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
			single = (Image*)App->gui->CreateImage("gui/ResourcesMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
			name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 20 - App->render->camera.y, nullptr);
		}
	}

	max_life = App->entityManager->selectedEntityList.front()->Life;
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;

	life = (Label*)App->gui->CreateLabel(life_str, 350 - App->render->camera.x, 700 - App->render->camera.y, nullptr);
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

	if (name->str == "TOWN CENTER")
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
				// PENE
				single = (Image*)App->gui->CreateImage("gui/UnitMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
				name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 25 - App->render->camera.y, nullptr);
			}
		}

		_itoa_s(App->entityManager->selectedEntityList.front()->Defense, armor, 65, 10);
		_itoa_s(App->entityManager->selectedEntityList.front()->Attack, damage, 65, 10);

		sword_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", posx - App->render->camera.x, posy + 50 - App->render->camera.y, SDL_Rect{ 0,19, 38, 22 });
		armor_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", posx- App->render->camera.x, posy + 75 - App->render->camera.y, SDL_Rect{ 0,63, 37, 19 });

		damage_val = (Label*)App->gui->CreateLabel(damage, posx + 50 - App->render->camera.x, posy + 50 - App->render->camera.y, nullptr);
		armor_val = (Label*)App->gui->CreateLabel(armor, posx + 50 - App->render->camera.x, posy + 75 - App->render->camera.y, nullptr);

		max_life = App->entityManager->selectedEntityList.front()->MaxLife;
		curr_life = App->entityManager->selectedEntityList.front()->Life;


		_itoa_s(curr_life, currlife, 65, 10);
		life_str += currlife;
		life_str += "/";
		_itoa_s(max_life, maxlife, 65, 10);
		life_str += maxlife;

		life = (Label*)App->gui->CreateLabel(life_str, posx + 50 - App->render->camera.x, posy + 35- App->render->camera.y, nullptr);

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
	// X = 500 Y = 650
}


void HUD::ClearMultiple()
{
	for (list<Image*>::iterator it = multiple.begin(); it != multiple.end(); ++it)
	{
		if (it._Ptr->_Myval != nullptr)
			App->gui->DestroyUIElement(it._Ptr->_Myval);
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