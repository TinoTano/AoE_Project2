#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "Gui.h"
#include "SceneTest.h"
#include <stdlib.h>  


SceneTest::SceneTest() : Module()
{
	name = "scene";
}


SceneTest::~SceneTest()
{}


bool SceneTest::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}


bool SceneTest::Start()
{

	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 0, 0, 220, 30 });
	blit_sections.push_back({ 0, 30, 220, 30 });

	vector<SDL_Rect> detect_sections;
	detect_sections.push_back({ 0, 0, 220, 30 });
	//detect_sections.push_back({ 0, 0, 222, 64 });

	button = (Button*)App->gui->CreateButton("gui/button.png", 50, 50, blit_sections, detect_sections, TIER2);

	top = (Image*)App->gui->CreateImage("gui/ingame_layer.png", 0, 0, { 0,0,1920, 30 });
	bottom = (Image*)App->gui->CreateImage("gui/ingame_layer.png", 0, 550, { 0, 40,1920, 200 });

	//Input* input = (Input*)App->gui->CreateInput(500, 35, { 515, 35, 338, 54 }, nullptr);

	
	return true;
}


bool SceneTest::PreUpdate()
{

	return true;
}


bool SceneTest::Update(float dt)
{

	if (button->current == HOVER || button->current == CLICKIN) App->gui->cursor->SetCursor(3);
	else App->gui->cursor->SetCursor(0);

	return true;
}

bool SceneTest::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}


bool SceneTest::CleanUp()
{
	LOG("Freeing scene");

	return true;
}