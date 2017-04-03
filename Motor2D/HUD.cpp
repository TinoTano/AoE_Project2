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
#include <stdlib.h>  

bool HUD::IsEnabled()
{
	return enabled;
}
//HUD
HUD::HUD()
{
	buttons_positions.push_back({ 30 - CAMERA_OFFSET_X, 650 - CAMERA_OFFSET_Y, 39,40 });
	buttons_positions.push_back({ 69 - CAMERA_OFFSET_X, 650 - CAMERA_OFFSET_Y, 39,40 });

	buttons_positions.push_back({ 200 - CAMERA_OFFSET_X, 710 - CAMERA_OFFSET_Y, 39,40 });
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

	char armor[65], damage[65], currlife[65], maxlife[65];

	if (App->entityManager->selectedBuildingtList.size() == 1)
	{
		if (type != BUILDINGINFO)
		{
			ClearSingle();
			ClearMultiple();
			type = BUILDINGINFO;
			StartBuildingInfo();
		}
		else {
			max_life = App->entityManager->selectedBuildingtList.front()->buildingMaxLife;
			curr_life = App->entityManager->selectedBuildingtList.front()->buildingLife;

			string life_str;
			_itoa_s(curr_life, currlife, 65, 10);
			life_str += currlife;
			life_str += "/";
			_itoa_s(max_life, maxlife, 65, 10);
			life_str += maxlife;

			life->str = maxlife;

			int percent = ((max_life - curr_life) * 100) / max_life;
			int barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;

			App->render->DrawQuad({ 310 - App->render->camera.x, 670 - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h, App->gui->SpriteBuildings.front().GetRect().w, 5 }, 255, 0, 0);
			App->render->DrawQuad({ 310 - App->render->camera.x, 670 - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h, min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent , 0)), 5 }, 0, 255, 0);
			switch (building_state) {
			case BUILDINGMENU:
				if (building_state != BUILDINGMENU)
				{
					HUDBuildingMenu();
				}
				else {
					if (create_unit_bt->current == CLICKIN)
						HUDCreateUnits();
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
						App->entityManager->CreateUnit(400, 250, false, ELVEN_ARCHER);
					}
					else if (create_elven_longblade_bt->current == CLICKIN)
					{
						App->entityManager->CreateUnit(400, 350, false, ELVEN_LONGBLADE);
					}
				}
				break;
			}

		}
	}
	else {

		switch (App->entityManager->selectedUnitList.size()) {
		case 0:
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
			break;
		case 1:
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
					if (it._Ptr->_Myval.GetID() == App->entityManager->selectedUnitList.front()->GetType())
					{
						name->str = it._Ptr->_Myval.GetName();
						defense = App->entityManager->selectedUnitList.front()->unitDefense;
						attack = App->entityManager->selectedUnitList.front()->unitDefense;
					}
				}

				_itoa_s(App->entityManager->selectedUnitList.front()->unitDefense, armor, 65, 10);
				_itoa_s(App->entityManager->selectedUnitList.front()->unitAttack, damage, 65, 10);

				damage_val->str = damage;
				armor_val->str = armor;

				max_life = App->entityManager->selectedUnitList.front()->unitMaxLife;
				curr_life = App->entityManager->selectedUnitList.front()->unitLife;

				string life_str;
				_itoa_s(curr_life, currlife, 65, 10);
				life_str += currlife;
				life_str += "/";
				_itoa_s(max_life, maxlife, 65, 10);
				life_str += maxlife;

				life->str = maxlife;

				int percent = ((max_life - curr_life) * 100) / max_life;
				int barPercent = (percent * App->gui->SpriteUnits.front().GetRect().w) / 100;

				App->render->DrawQuad({ 310 - App->render->camera.x, 670 - App->render->camera.y + App->gui->SpriteUnits.front().GetRect().h, App->gui->SpriteUnits.front().GetRect().w, 5 }, 255, 0, 0);
				App->render->DrawQuad({ 310 - App->render->camera.x, 670 - App->render->camera.y + App->gui->SpriteUnits.front().GetRect().h, min(App->gui->SpriteUnits.front().GetRect().w, max(App->gui->SpriteUnits.front().GetRect().w - barPercent , 0)), 5 }, 0, 255, 0);
			}
			break;
		default:
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
					for (list<Unit*>::iterator it_unit = App->entityManager->selectedUnitList.begin(); it_unit != App->entityManager->selectedUnitList.end(); ++it_unit)
					{
						if (x >= max_width)
						{
							x = 0;
							y += App->gui->SpriteUnits.front().GetRect().h + 5;
						}
						if (it_sprite._Ptr->_Myval.GetID() == it_unit._Ptr->_Myval->GetType()) {
							int percent = ((it_unit._Ptr->_Myval->unitMaxLife - it_unit._Ptr->_Myval->unitLife) * 100) / it_unit._Ptr->_Myval->unitMaxLife;
							int barPercent = (percent * App->gui->SpriteUnits.front().GetRect().w) / 100;

							App->render->DrawQuad({ 310 - App->render->camera.x + x, 650 - App->render->camera.y + App->gui->SpriteUnits.front().GetRect().h + y, App->gui->SpriteUnits.front().GetRect().w, 5 }, 255, 0, 0);
							App->render->DrawQuad({ 310 - App->render->camera.x + x, 650 - App->render->camera.y + App->gui->SpriteUnits.front().GetRect().h + y, min(App->gui->SpriteUnits.front().GetRect().w, max(App->gui->SpriteUnits.front().GetRect().w - barPercent , 0)), 5 }, 0, 255, 0);
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
	// THIS MUST BE DONE HERE
	building_state = BUILDINGMENU;
}

void HUD::HUDClearBuildingMenu()
{
	App->gui->DestroyUIElement(create_unit_bt);
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
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);

}

void HUD::HUDClearCreateUnits()
{
	App->gui->DestroyUIElement(create_elven_archer_bt);
	App->gui->DestroyUIElement(create_elven_longblade_bt);
	App->gui->DestroyUIElement(cancel_bt);
}


void HUD::StartBuildingInfo()
{
	char currlife[65], maxlife[65];
	string life_str;

	for (list<UnitSprite>::iterator it = App->gui->SpriteBuildings.begin(); it != App->gui->SpriteBuildings.end(); ++it)
	{
		if (it._Ptr->_Myval.GetID() == App->entityManager->selectedBuildingtList.front()->type)
		{
			single = (Image*)App->gui->CreateImage("gui/BuildingMiniatures.png", 310 - App->render->camera.x, 670 - App->render->camera.y, it._Ptr->_Myval.GetRect());
			name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), 310 - App->render->camera.x, 650 - App->render->camera.y, nullptr);
		}
	}

	max_life = App->entityManager->selectedBuildingtList.front()->buildingMaxLife;
	curr_life = App->entityManager->selectedBuildingtList.front()->buildingLife;


	_itoa_s(curr_life, currlife, 65, 10);
	life_str += currlife;
	life_str += "/";
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;

	life = (Label*)App->gui->CreateLabel(life_str, 350 - App->render->camera.x, 700 - App->render->camera.y, nullptr);

	HUDBuildingMenu();
}


void HUD::GetSelection() {
	char armor[65], damage[65], currlife[65], maxlife[65];
	string life_str;

	switch (type) {
	case NONE:
		break;
	case SINGLEINFO:
		for (list<UnitSprite>::iterator it = App->gui->SpriteUnits.begin(); it != App->gui->SpriteUnits.end(); ++it)
		{
			if (it._Ptr->_Myval.GetID() == App->entityManager->selectedUnitList.front()->GetType())
			{
				single = (Image*)App->gui->CreateImage("gui/UnitMiniatures.png", 310 - App->render->camera.x, 670 - App->render->camera.y, it._Ptr->_Myval.GetRect());
				name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), 310 - App->render->camera.x, 650 - App->render->camera.y, nullptr);
			}
		}

		_itoa_s(App->entityManager->selectedUnitList.front()->unitDefense, armor, 65, 10);
		_itoa_s(App->entityManager->selectedUnitList.front()->unitAttack, damage, 65, 10);

		sword_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", 310 - App->render->camera.x, 720 - App->render->camera.y, SDL_Rect{ 0,19, 38, 22 });
		armor_img = (Image*)App->gui->CreateImage("gui/game_scene_ui.png", 310 - App->render->camera.x, 750 - App->render->camera.y, SDL_Rect{ 0,63, 37, 19 });

		damage_val = (Label*)App->gui->CreateLabel(damage, 350 - App->render->camera.x, 720 - App->render->camera.y, nullptr);
		armor_val = (Label*)App->gui->CreateLabel(armor, 350 - App->render->camera.x, 750 - App->render->camera.y, nullptr);

		max_life = App->entityManager->selectedUnitList.front()->unitMaxLife;
		curr_life = App->entityManager->selectedUnitList.front()->unitLife;


		_itoa_s(curr_life, currlife, 65, 10);
		life_str += currlife;
		life_str += "/";
		_itoa_s(max_life, maxlife, 65, 10);
		life_str += maxlife;

		life = (Label*)App->gui->CreateLabel(life_str, 350 - App->render->camera.x, 700 - App->render->camera.y, nullptr);
		break;
	case MULTIPLESELECTION:
		int x = 0, y = 0;
		max_width = 700;
		for (list<UnitSprite>::iterator it_sprite = App->gui->SpriteUnits.begin(); it_sprite != App->gui->SpriteUnits.end(); ++it_sprite)
		{
			for (list<Unit*>::iterator it_unit = App->entityManager->selectedUnitList.begin(); it_unit != App->entityManager->selectedUnitList.end(); ++it_unit)
			{
				if (x >= max_width)
				{
					x = 0;
					y += App->gui->SpriteUnits.front().GetRect().h + 5;
				}
				if (it_sprite._Ptr->_Myval.GetID() == it_unit._Ptr->_Myval->GetType())
				{
					Image* unit = (Image*)App->gui->CreateImage("gui/UnitMiniatures.png", 310 - App->render->camera.x + x, 650 - App->render->camera.y + y, it_sprite._Ptr->_Myval.GetRect());
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
		App->gui->DestroyUIElement(single);
	if (name != nullptr)
		App->gui->DestroyUIElement(name);
	if (sword_img != nullptr)
		App->gui->DestroyUIElement(sword_img);
	if (armor_img != nullptr)
		App->gui->DestroyUIElement(armor_img);
	if (damage_val != nullptr)
		App->gui->DestroyUIElement(damage_val);
	if (armor_val != nullptr)
		App->gui->DestroyUIElement(armor_val);
	if (life != nullptr)
		App->gui->DestroyUIElement(life);
}

void HUD::CleanUp()
{
	ClearMultiple();
	ClearSingle();
}