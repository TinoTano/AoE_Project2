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
	delete this;
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
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);
	}

	return true;
}

bool MenuScene::CleanUp()
{
	App->gui->DestroyUIElement(background);
	return true;
}
