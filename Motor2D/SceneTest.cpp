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

	//Image* test= (Image*)App->gui->CreateImage( "gui/atlas_lol.png", 0,0 , { 0 , 0, 100, 100 } );
	Label* label = (Label*)App->gui->CreateLabel("no crashees pls", 0, 0, nullptr);
	vector<SDL_Rect> blit_sections;
	blit_sections.push_back({ 4, 114, 222, 64 });
	blit_sections.push_back({ 415, 170, 222, 64 });
	blit_sections.push_back({ 646, 170, 222, 64 });

	vector<SDL_Rect> detect_sections;
	detect_sections.push_back({ 200, 200, 222, 64 });
	detect_sections.push_back({ 0, 0, 222, 64 });

	Button* button = (Button*)App->gui->CreateButton("gui/atlas_lol.png", 50, 50, blit_sections, detect_sections, TIER1);

	Image* in_bg = (Image*)App->gui->CreateImage("gui/atlas_lol.png", 500, 30, { 491 , 572, 338, 54 });
	Input* input = (Input*)App->gui->CreateInput(500, 35, { 515, 35, 338, 54 }, nullptr);

	scroll = (ScrollBar*)App->gui->CreateScrollBar(500, 200, MODEL1);

	scroll2 = (ScrollBar*)App->gui->CreateScrollBar(700, 400, MODEL1);

	ScrollBar* scroll3 = (ScrollBar*)App->gui->CreateScrollBar(0, 400, MODEL1);


	_itoa_s(scroll->GetData(), &mierda, 65, 10);
	_itoa_s(scroll2->GetData(), &mierda2, 65, 10);

	val = (Label*)App->gui->CreateLabel(&mierda, 480, 120, nullptr);
	val2 = (Label*)App->gui->CreateLabel(&mierda2, 680, 320, nullptr);

	//Quad* quad = (Quad*)App->gui->CreateQuad({ 0, 0 ,800, 400 }, { 100,100,100, 255 });

	return true;
}


bool SceneTest::PreUpdate()
{

	return true;
}


bool SceneTest::Update(float dt)
{

	_itoa_s(scroll->GetData(), &mierda, 65, 10);
	val->SetText(&mierda);
	_itoa_s(scroll2->GetData(), &mierda2, 65, 10);
	val2->SetText(&mierda2);



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