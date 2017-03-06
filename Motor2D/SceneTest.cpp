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


	// To use the cursor, this is needed!
	// This contains all the rects for the cursor. To change the blitted cursor just:
	// Call App->gui->cursor->SetCursor(int id);
	// Where id is a number from 0 to 17, should be an enum later on.

	vector<SDL_Rect> sprites_cursor;
	sprites_cursor.push_back({ 0,   0, 70, 50 });
	sprites_cursor.push_back({ 70,   0, 70, 50});
	sprites_cursor.push_back({ 140, 0, 70, 50 });
	sprites_cursor.push_back({ 210, 0, 70, 50 });
	sprites_cursor.push_back({ 280, 0, 70, 50 });
	sprites_cursor.push_back({ 350, 0, 70, 50 });
	sprites_cursor.push_back({ 420, 0, 70, 50 });
	sprites_cursor.push_back({ 490, 0, 70, 50 });
	sprites_cursor.push_back({ 560, 0, 70, 50 });
	sprites_cursor.push_back({ 0,   50, 70, 50 });
	sprites_cursor.push_back({ 70,  50, 70, 50 });
	sprites_cursor.push_back({ 140, 50, 70, 50 });
	sprites_cursor.push_back({ 210, 50, 70, 50 });
	sprites_cursor.push_back({ 280, 50, 70, 50 });
	sprites_cursor.push_back({ 350, 50, 70, 50 });
	sprites_cursor.push_back({ 420, 50, 70, 50 });
	sprites_cursor.push_back({ 490, 50, 70, 50 });
	sprites_cursor.push_back({ 560, 50, 70, 50 });

	App->gui->cursor = (Cursor*)App->gui->CreateCursor("gui/cursor.png", sprites_cursor);
	return true;
}


bool SceneTest::PreUpdate()
{

	return true;
}


bool SceneTest::Update(float dt)
{
	/*
	_itoa_s(scroll->GetData(), &mierda, 65, 10);
	val->SetText(&mierda);
	*/
	

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