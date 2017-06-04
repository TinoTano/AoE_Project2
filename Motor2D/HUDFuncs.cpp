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
	all_bt.push_back(create_building_bt);
	blit_sections.clear();
}

void HUD::HUDClearVillagerMenu()
{
	App->gui->DestroyUIElement(create_building_bt);
	create_building_bt = nullptr;
	blit_sections.clear();
}

void HUD::HUDHeroMenu(Skill_type skill)
{
	switch (hero_state) {
	}
	hero_state = HEROMENU;


	for (uint i = 0; i < App->gui->skill_bt.size(); ++i)
	{
		if (App->gui->skill_bt[i].type == skill)
		{
			skill_bt_support = App->gui->skill_bt[i].button = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, App->gui->skill_bt[i].blit_sections, buttons_positions, TIER2);
		}
	}
}

void HUD::HUDClearHeroMenu()
{
	for (uint i = 0; i < App->gui->skill_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->skill_bt[i].button);
		App->gui->skill_bt[i].button = nullptr;
	}
	App->gui->DestroyUIElement(skill_bt_support);
	skill_bt_support = nullptr;
}

void HUD::HUDCreateBuildings()
{
	switch (villager_state) {
	case VILLAGERMENU:
		HUDClearVillagerMenu();
		break;
	}

	villager_state = VILLAGERCREATEBUILDINGS;

	vector<buildingType> available_buildings;
	for (list<buildingType>::iterator it = App->entityManager->player->tech_tree->available_buildings.begin(); it != App->entityManager->player->tech_tree->available_buildings.end(); ++it) {
		available_buildings.push_back(*it);
	}
	//TOWN_CENTER, HOUSE, ORC_BARRACKS, ARCHERY_RANGE, STABLES, SIEGE_WORKSHOP, MARKET, BLACKSMITH, MILL, OUTPOST, MONASTERY, CASTLE, SAURON_TOWER, FARM

	uint c = 0;
	for (uint i = 0; i < available_buildings.size(); ++i) {
		for (uint i2 = 0; i2 < App->gui->building_bt.size(); ++i2) {
			if (App->gui->building_bt[i2].type == available_buildings[i])
				if (App->gui->building_bt[i2].button == nullptr) {
					App->gui->building_bt[i2].button = (Button*)App->gui->CreateButton("gui/BuildingMiniatures.png", buttons_positions[c].x - CAMERA_OFFSET_X, buttons_positions[c].y - CAMERA_OFFSET_Y, App->gui->building_bt[i2].blit_sections, buttons_positions, TIER2);
					c++;
					all_bt.push_back(App->gui->building_bt[i2].button);
				}
		}
	}

	blit_sections.clear();
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });
	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
	all_bt.push_back(cancel_bt);
	blit_sections.clear();
}

void HUD::HUDClearCreateBuildings()
{
	for (uint i = 0; i < App->gui->building_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->building_bt[i].button);
		App->gui->building_bt[i].button = nullptr;
	}
	App->gui->DestroyUIElement(cancel_bt);
	cancel_bt = nullptr;
	blit_sections.clear();
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

	bool create_units = false;
	for (list<pair<unitType, buildingType>>::iterator it = App->entityManager->player->tech_tree->available_units.begin(); it != App->entityManager->player->tech_tree->available_units.end(); ++it) {
		if (id == it._Ptr->_Myval.second) {
			create_units = true;
			break;
		}
	}
	if (create_units)
	{
		blit_sections.push_back({ 130, 64, 39, 40 });
		blit_sections.push_back({ 169, 64, 39, 40 });

		create_unit_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
		all_bt.push_back(create_unit_bt);
		blit_sections.clear();
	}

	if (name->str == "TOWN CENTER") {

		blit_sections.push_back({ 52, 24, 39, 40 });
		blit_sections.push_back({ 91, 24, 39, 40 });

		create_villager_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
		all_bt.push_back(create_villager_bt);
		blit_sections.clear();
	}

	vector<TechType> available_techs;

	for (uint i = 0; i < App->entityManager->player->tech_tree->all_techs.size(); ++i)
	{
		for (list<TechType>::iterator it = App->entityManager->player->tech_tree->available_techs.begin(); it != App->entityManager->player->tech_tree->available_techs.end(); ++it)
		{
			if ((*it) == App->entityManager->player->tech_tree->all_techs[i]->id && App->entityManager->player->tech_tree->all_techs[i]->researched_in == id)
			{
				available_techs.push_back((*it));
			}
		}
	}

	uint tech_place = TECH_UI_BUTTON;

	for (uint i = 0; i < available_techs.size(); ++i)
	{
		for (uint i2 = 0; i2 < App->gui->tech_bt.size(); ++i2)
		{
			if (available_techs[i] == App->gui->tech_bt[i2].type)
			{
				if (App->gui->tech_bt[i2].button == nullptr) {
					App->gui->tech_bt[i2].button = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[tech_place].x - CAMERA_OFFSET_X, buttons_positions[tech_place].y - CAMERA_OFFSET_Y, App->gui->tech_bt[i2].blit_sections, buttons_positions, TIER2);
					all_bt.push_back(App->gui->tech_bt[i2].button);
					tech_place++;
				}
			}
		}
	}
}

void HUD::HUDClearBuildingMenu()
{
	App->gui->DestroyUIElement(create_unit_bt);
	create_unit_bt = nullptr;
	App->gui->DestroyUIElement(create_villager_bt);
	create_villager_bt = nullptr;
	App->gui->DestroyUIElement(cancel_bt);
	cancel_bt = nullptr;

	for (uint i = 0; i < App->gui->tech_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->tech_bt[i].button);
		App->gui->tech_bt[i].button = nullptr;
	}
	blit_sections.clear();
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

	App->gui->unit_bt[LEGOLAS].button = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[0].x - CAMERA_OFFSET_X, buttons_positions[0].y - CAMERA_OFFSET_Y, App->gui->unit_bt[LEGOLAS].blit_sections, buttons_positions, TIER2);

	App->gui->unit_bt[GANDALF].button = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[1].x - CAMERA_OFFSET_X, buttons_positions[1].y - CAMERA_OFFSET_Y, App->gui->unit_bt[GANDALF].blit_sections, buttons_positions, TIER2);

	App->gui->unit_bt[BALROG].button = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[2].x - CAMERA_OFFSET_X, buttons_positions[2].y - CAMERA_OFFSET_Y, App->gui->unit_bt[BALROG].blit_sections, buttons_positions, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 53, 64, 39, 40 });
	blit_sections.push_back({ 92, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
	all_bt.push_back(cancel_bt);
	blit_sections.clear();
}

void HUD::HUDClearCreateHero()
{
	App->gui->DestroyUIElement(cancel_bt);
	App->gui->DestroyUIElement(App->gui->unit_bt[LEGOLAS].button);
	App->gui->unit_bt[LEGOLAS].button = nullptr;
	App->gui->DestroyUIElement(App->gui->unit_bt[GANDALF].button);
	App->gui->unit_bt[GANDALF].button = nullptr;
	App->gui->DestroyUIElement(App->gui->unit_bt[BALROG].button);
	App->gui->unit_bt[BALROG].button = nullptr;
	blit_sections.clear();
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

	vector<unitType> available_units;
	for (list<pair<unitType, buildingType>>::iterator it = App->entityManager->player->tech_tree->available_units.begin(); it != App->entityManager->player->tech_tree->available_units.end(); ++it) {
		if (id == it._Ptr->_Myval.second)
			available_units.push_back(it._Ptr->_Myval.first);
	}

	//TOWN_CENTER, HOUSE, ORC_BARRACKS, ARCHERY_RANGE, STABLES, SIEGE_WORKSHOP, MARKET, BLACKSMITH, MILL, OUTPOST, MONASTERY, CASTLE, SAURON_TOWER, FARM


	uint c = 0;
	for (uint i = 0; i < available_units.size(); ++i) {
		for (uint i2 = 0; i2 < App->gui->unit_bt.size(); ++i2) {
			if (App->gui->unit_bt[i2].type == available_units[i] && App->gui->unit_bt[i2].button == nullptr) {
				App->gui->unit_bt[i2].button = (Button*)App->gui->CreateButton("gui/UnitMiniatures.png", buttons_positions[c].x - CAMERA_OFFSET_X, buttons_positions[c].y - CAMERA_OFFSET_Y, buildings_rects[c], buttons_positions, TIER2);
				c++;
				all_bt.push_back(App->gui->unit_bt[i2].button);
			}
		}
	}

	blit_sections.clear();
	blit_sections.push_back({ 52, 64, 39, 40 });
	blit_sections.push_back({ 91, 64, 39, 40 });

	cancel_bt = (Button*)App->gui->CreateButton("gui/game_scene_ui.png", buttons_positions[NUM_UI_BUTTONS].x - CAMERA_OFFSET_X, buttons_positions[NUM_UI_BUTTONS].y - CAMERA_OFFSET_Y, blit_sections, buttons_positions, TIER2);
	all_bt.push_back(cancel_bt);
	blit_sections.clear();
}

void HUD::HUDClearCreateUnits()
{
	for (uint i = 0; i < App->gui->unit_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->unit_bt[i].button);
		App->gui->unit_bt[i].button = nullptr;
	}
	App->gui->DestroyUIElement(cancel_bt);
	blit_sections.clear();
}


void HUD::BlitInfoBuilding(building_button bt)
{
	Sprite quad;
	quad.rect.x = x - x - App->render->camera.x;
	quad.rect.y = y / 2 + (y / 8) + y / 50 - App->render->camera.y;
	quad.rect.w = 400;
	quad.rect.h = 80;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);

	if (info_lbl == nullptr) {
		info_lbl = (Label*)App->gui->CreateLabel(bt.name, quad.rect.x, quad.rect.y, App->font->fonts[TWENTY]);
		info_lbl->SetColor({ 255, 255, 255, 255 });
		desc_lbl = (Label*)App->gui->CreateLabel(bt.desc, quad.rect.x, quad.rect.y + 22, nullptr);
		desc_lbl->SetColor({ 255, 255, 255, 255 });

		cost_lbl = (Label*)App->gui->CreateLabel(bt.cost, quad.rect.x, quad.rect.y + 45, nullptr);
		cost_lbl->SetColor({ 255, 255, 255, 255 });
	}

}

void HUD::BlitInfoUnit(unit_button bt)
{
	Sprite quad;
	quad.rect.x = x - x - App->render->camera.x;
	quad.rect.y = y / 2 + (y / 8) + y / 50 - App->render->camera.y;
	quad.rect.w = 400;
	quad.rect.h = 80;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);
	if (info_lbl == nullptr) {
		info_lbl = (Label*)App->gui->CreateLabel(bt.name, quad.rect.x, quad.rect.y, App->font->fonts[TWENTY]);
		info_lbl->SetColor({ 255, 255, 255, 255 });
		desc_lbl = (Label*)App->gui->CreateLabel(bt.desc, quad.rect.x, quad.rect.y + 22, nullptr);
		desc_lbl->SetColor({ 255, 255, 255, 255 });

		cost_lbl = (Label*)App->gui->CreateLabel(bt.cost, quad.rect.x, quad.rect.y + 45, nullptr);
		cost_lbl->SetColor({ 255, 255, 255, 255 });
	}

}
void HUD::BlitInfoTech(tech_button bt)
{
	Sprite quad;
	quad.rect.x = x - x - App->render->camera.x;
	quad.rect.y = y / 2 + (y / 8) + y / 50 - App->render->camera.y;
	quad.rect.w = 600;
	quad.rect.h = 80;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);
	if (info_lbl == nullptr) {
		info_lbl = (Label*)App->gui->CreateLabel(bt.name, quad.rect.x, quad.rect.y, App->font->fonts[TWENTY]);
		info_lbl->SetColor({ 255, 255, 255, 255 });
		desc_lbl = (Label*)App->gui->CreateLabel(bt.desc, quad.rect.x, quad.rect.y + 22, nullptr);
		desc_lbl->SetColor({ 255, 255, 255, 255 });
		cost_lbl = (Label*)App->gui->CreateLabel(bt.cost, quad.rect.x, quad.rect.y + 45, nullptr);
		cost_lbl->SetColor({ 255, 255, 255, 255 });
	}

}

void HUD::BlitInfoSkill(skill_button bt)
{
	Sprite quad;
	quad.rect.x = x - x - App->render->camera.x;
	quad.rect.y = y / 2 + (y / 8) + y / 50 - App->render->camera.y;
	quad.rect.w = 400;
	quad.rect.h = 80;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);
	if (info_lbl == nullptr) {
		info_lbl = (Label*)App->gui->CreateLabel(bt.name, quad.rect.x, quad.rect.y, App->font->fonts[TWENTY]);
		info_lbl->SetColor({ 255, 255, 255, 255 });
		desc_lbl = (Label*)App->gui->CreateLabel(bt.desc, quad.rect.x, quad.rect.y + 22, nullptr);
		desc_lbl->SetColor({ 255, 255, 255, 255 });
		cost_lbl = (Label*)App->gui->CreateLabel(bt.changes + " Cooldown: " + bt.cooldown + "s", quad.rect.x, quad.rect.y + 45, nullptr);
		cost_lbl->SetColor({ 255, 255, 255, 255 });
	}
}

void HUD::BlitInfoVillager()
{
	Sprite quad;
	quad.rect.x = x - x - App->render->camera.x;
	quad.rect.y = y / 2 + (y / 8) + y / 50 - App->render->camera.y;
	quad.rect.w = 400;
	quad.rect.h = 80;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);
	if (info_lbl == nullptr) {
		info_lbl = (Label*)App->gui->CreateLabel("Villager", quad.rect.x, quad.rect.y, App->font->fonts[TWENTY]);
		info_lbl->SetColor({ 255, 255, 255, 255 });
		desc_lbl = (Label*)App->gui->CreateLabel("Villagers can gather resources and create buildings.", quad.rect.x, quad.rect.y + 22, nullptr);
		desc_lbl->SetColor({ 255, 255, 255, 255 });
		cost_lbl = (Label*)App->gui->CreateLabel("food: 50", quad.rect.x, quad.rect.y + 45, nullptr);
		cost_lbl->SetColor({ 255, 255, 255, 255 });
	}
}
void HUD::StartResourceInfo()
{
	char currlife[65], maxlife[65];
	string life_str;

	Resource* resource = (Resource*)App->entityManager->selectedEntityList.front();

	for (list<UnitSprite>::iterator it = App->gui->SpriteResources.begin(); it != App->gui->SpriteResources.end(); ++it)
	{
		if (it._Ptr->_Myval.GetID() == resource->contains)
		{
			id = resource->contains;
			single = (Image*)App->gui->CreateImage("gui/ResourcesMiniatures.png", posx - App->render->camera.x, posy - App->render->camera.y, it._Ptr->_Myval.GetRect());
			name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), posx - App->render->camera.x, posy - 20 - App->render->camera.y, nullptr);
		}
	}

	max_life = App->entityManager->selectedEntityList.front()->Life;
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;

	life = (Label*)App->gui->CreateLabel(life_str, posx + 50 - App->render->camera.x, posy + 35 - App->render->camera.y, nullptr);
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
			if (it._Ptr->_Myval.GetID() == unit->type)
			{
				id = unit->type;
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
			Hero* hero = (Hero*)unit;
			HUDHeroMenu(hero->skill->type);
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
				if (it_sprite._Ptr->_Myval.GetID() == unit->type)
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
			App->gui->DestroyUIElement(it._Ptr->_Myval);
		}
	}
	multiple.clear();
}

void HUD::ClearBuilding() {

	if (single != nullptr)
	App->gui->DestroyUIElement(single);
	if (name != nullptr)
	App->gui->DestroyUIElement(name);
	if(life != nullptr)
	App->gui->DestroyUIElement(life);

	HUDClearBuildingMenu();

	HUDClearCreateUnits();

	HUDClearCreateHero();
	
	building_state = BUILDINGNULL;
	selected_building = nullptr;

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


		HUDClearVillagerMenu();

		HUDClearCreateBuildings();
	
		HUDClearHeroMenu();

	selected_unit = nullptr;

	single = nullptr;
	name = nullptr;
	armor_val = nullptr;
	damage_val = nullptr;
	life = nullptr;
	sword_img = nullptr;
	armor_img = nullptr;

	blit_sections.clear();
	villager_state = VILLAGERNULL;
	hero_state = HERONULL;
}

void HUD::ClearResource()
{
	if (single != nullptr)
	{
		App->gui->DestroyUIElement(single);
		single = nullptr;
	}
	if (name != nullptr)
	{
		App->gui->DestroyUIElement(name);
		name = nullptr;
	}
	if (life != nullptr)
	{
		App->gui->DestroyUIElement(life);
		life = nullptr;
	}

	selected_resource = nullptr;
}

void HUD::ClearAll()
{
	ClearSingle();
	ClearBuilding();
	ClearMultiple();
	ClearResource();

	for (uint i = 0; i < all_bt.size(); ++i) {
		App->gui->DestroyUIElement(all_bt[i]);
	}
	all_bt.clear();

	for (uint i = 0; i < App->gui->building_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->building_bt[i].button);
		App->gui->building_bt[i].button = nullptr;
	}
	for (uint i = 0; i < App->gui->unit_bt.size(); ++i) {
		App->gui->DestroyUIElement(App->gui->unit_bt[i].button);
		App->gui->unit_bt[i].button = nullptr;
	}

	type = NONE;
	villager_state = VILLAGERNULL;
	hero_state = HERONULL;
	building_state = BUILDINGNULL;
	studying_tech = false;

}

void HUD::DrawResourceBar()
{
	Sprite bar;
	string life_str;
	int percent;
	int barPercent;
	max_life = App->entityManager->selectedEntityList.front()->MaxLife;
	curr_life = App->entityManager->selectedEntityList.front()->Life;
	_itoa_s(curr_life, currlife, 65, 10);
	life_str += currlife;
	life_str += "/";
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;
	life->SetString(life_str);
	if (currlife > 0) {
		if (max_life == 0) max_life = curr_life;
		percent = ((max_life - curr_life) * 100) / max_life;
		barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;
		bar.rect.x = posx - App->render->camera.x;
		bar.rect.y = posy - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
		bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
		bar.rect.h = 5;
		bar.r = 255;
		bar.g = 255;
		bar.b = 255;
		bar.a = 255;
		App->render->ui_toDraw.push_back(bar);
		bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
		bar.r = 0;
		bar.g = 255;
		bar.b = 0;
		App->render->ui_toDraw.push_back(bar);
	}
}

void HUD::DrawBuildingBar()
{
	Sprite bar;
	string life_str;
	int percent, barPercent;
	max_life = App->entityManager->selectedEntityList.front()->MaxLife;
	curr_life = App->entityManager->selectedEntityList.front()->Life;
	_itoa_s(curr_life, currlife, 65, 10);
	life_str += currlife;
	life_str += "/";
	_itoa_s(max_life, maxlife, 65, 10);
	life_str += maxlife;
	life->SetString(life_str);
	if (currlife > 0) {
		if (max_life == 0) max_life = curr_life;
		percent = ((max_life - curr_life) * 100) / max_life;
		barPercent = (percent * App->gui->SpriteBuildings.front().GetRect().w) / 100;
		bar.rect.x = posx - App->render->camera.x;
		bar.rect.y = posy - App->render->camera.y + App->gui->SpriteBuildings.front().GetRect().h;
		bar.rect.w = App->gui->SpriteBuildings.front().GetRect().w;
		bar.rect.h = 5;
		bar.r = 255;
		bar.g = 255;
		bar.b = 255;
		bar.a = 255;
		App->render->ui_toDraw.push_back(bar);
		bar.rect.w = min(App->gui->SpriteBuildings.front().GetRect().w, max(App->gui->SpriteBuildings.front().GetRect().w - barPercent, 0));
		bar.r = 0;
		bar.g = 255;
		bar.b = 0;
		App->render->ui_toDraw.push_back(bar);
	}
}