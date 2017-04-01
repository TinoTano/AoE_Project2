#include "MenuScene.h"
#include "Render.h"
#include "Application.h"
#include "Input.h"
#include "SceneManager.h"
MenuScene::MenuScene() : SceneElement("menu")
{
}

MenuScene::~MenuScene()
{
}

bool MenuScene::Awake(pugi::xml_node & config)
{
	return true;
}

bool MenuScene::Start()
{
	bool ret = true;
	App->render->camera.x = 0;
	App->render->camera.y = 0;
	background = (Image*) App->gui->CreateImage("gui/MenuAtlas.png", 0, 2, { 0,0,1408, 792 });
	menu_bg_img = (Image*)App->gui->CreateImage("gui/MenuAtlas.png", 800, 200, { 10, 798, 412, 291 });

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 462, 811, 391, 64 });
	blit_sections.push_back({ 877, 814, 391, 64 });

	vector<SDL_Rect> detect_sections;
	detect_sections.push_back({ 810, 210, 391, 64 });

	campaign_bt = (Button*)App->gui->CreateButton("gui/MenuAtlas.png", 810, 210, blit_sections, detect_sections, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 462, 881, 391, 64 });
	blit_sections.push_back({ 877, 884, 391, 64 });
	detect_sections.clear();
	detect_sections.push_back({810, 280, 391, 64});

	skirmish_bt = (Button*)App->gui->CreateButton("gui/MenuAtlas.png", 810, 280, blit_sections, detect_sections, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 462, 951, 391, 64 });
	blit_sections.push_back({ 877, 954, 391, 64 });
	detect_sections.clear();
	detect_sections.push_back({ 810, 350, 391, 64 });

	options_bt = (Button*)App->gui->CreateButton("gui/MenuAtlas.png", 810, 350, blit_sections, detect_sections, TIER2);

	blit_sections.clear();
	blit_sections.push_back({ 462, 1021, 391, 64 });
	blit_sections.push_back({ 877, 1023, 391, 64 });
	detect_sections.clear();
	detect_sections.push_back({ 810, 420, 391, 64 });

	quit_bt = (Button*)App->gui->CreateButton("gui/MenuAtlas.png", 810, 420, blit_sections, detect_sections, TIER2);

	return ret;
}

bool MenuScene::PreUpdate()
{
	return true;
}

bool MenuScene::Update(float dt)
{
	

	return true;
}

bool MenuScene::PostUpdate()
{
	if (campaign_bt->current == CLICKIN || skirmish_bt->current == CLICKIN)
	{
		App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);
	}
	
	else if (quit_bt->current == CLICKIN)
	{
		App->quit = true;
	}

	return true;
}

bool MenuScene::CleanUp()
{
	App->gui->DestroyUIElement(background);
	App->gui->DestroyUIElement(menu_bg_img);
	App->gui->DestroyUIElement(campaign_bt);
	App->gui->DestroyUIElement(skirmish_bt);
	App->gui->DestroyUIElement(options_bt);
	App->gui->DestroyUIElement(quit_bt);

	return true;
}
