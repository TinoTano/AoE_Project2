#include "MenuScene.h"
#include "Render.h"
#include "Application.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"

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

	uint x, y;
	App->win->GetWindowSize(x, y);
	elements = App->gui->GetElements("MENUSCENE");

	elements[1].position.first =  (x / 3) * 2 - (x/8);
	elements[1].position.second = (y / 2) - (y/3);

	elements[2].position.first = elements[1].position.first + x / 100;
	elements[2].position.second = elements[1].position.second + y / 50;

	elements[3].position.first = elements[2].position.first;
	elements[3].position.second = elements[2].position.second + y / 100 + elements[2].detect_sections.front().h;

	elements[4].position.first = elements[3].position.first;
	elements[4].position.second = elements[3].position.second + y / 100 + elements[3].detect_sections.front().h;



	for (uint it = 0; it < elements.size(); ++it) {
		switch (elements[it].type)
		{
		case IMAGE:
			images.push_back((Image*)App->gui->CreateImage(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].rect));
			break;
		case BUTTON:
			buttons.push_back((Button*)App->gui->CreateButton(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].blit_sections, elements[it].detect_sections, elements[it].tier));
			break;
		}
	}

	//LOAD FX
	fx_button_click = App->audio->LoadFx("audio/fx/fx_button_click.wav");
	//App->audio->PlayMusic("audio/music/m_menu.ogg");

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
	if (buttons[0]->current == CLICKIN)
	{
		App->audio->PlayFx(fx_button_click);
		App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);
	}
	else if (buttons[2]->current == CLICKIN)
	{
		App->quit = true;
	}

	return true;
}

bool MenuScene::CleanUp()
{
	App->gui->DestroyALLUIElements();
	elements.clear();
	images.clear();
	buttons.clear();
	return true;
}
