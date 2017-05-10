#include "MenuScene.h"
#include "Render.h"
#include "Application.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"
#include "CutSceneManager.h"

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

	elements[1].position.first = (x / 3) * 2 - (x / 8);
	elements[1].position.second = (y / 2) - (y / 3);

	elements[2].position.first = (x / 3) + (x / 10);
	elements[2].position.second = elements[1].position.second;

	elements[3].position.first = elements[1].position.first + x / 180;
	elements[3].position.second = elements[1].position.second + y / 50;

	elements[4].position.first = elements[3].position.first;
	elements[4].position.second = elements[3].position.second + y / 100 + elements[3].detect_sections.front().h;

	elements[5].position.first = elements[4].position.first;
	elements[5].position.second = elements[4].position.second + y / 100 + elements[4].detect_sections.front().h;

	elements[6].position.first = elements[2].position.first + (x / 100);
	elements[6].position.second = elements[2].position.second + (y / 15);

	elements[7].position.first = elements[6].position.first;
	elements[7].position.second = elements[6].position.second + (y / 20);


	for (uint it = 0; it < elements.size(); ++it) {
		switch (elements[it].type)
		{
		case IMAGE:
			images.push_back((Image*)App->gui->CreateImage(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].rect));
			images.back()->loaded_tex = true;
			break;
		case BUTTON:
			buttons.push_back((Button*)App->gui->CreateButton(elements[it].texture, elements[it].position.first, elements[it].position.second, elements[it].blit_sections, elements[it].detect_sections, elements[it].tier));
			buttons.back()->loaded_tex = true;
			break;
		}
	}

	settings_lbl = (Label*)App->gui->CreateLabel("Settings", images[SETTINGS]->pos.first + x / 100, images[SETTINGS]->pos.second, nullptr);

	if (App->audio->active == true) {
		mute_lbl = (Label*)App->gui->CreateLabel("MUTE", buttons[MUTE]->pos.first + x / 50, buttons[MUTE]->pos.second + y / 150, nullptr);
	}
	else mute_lbl = (Label*)App->gui->CreateLabel("UNMUTE", buttons[MUTE]->pos.first + x / 50, buttons[MUTE]->pos.second + y / 150, nullptr);

	if (!App->win->IsFullScreen()) {
		window_lbl = (Label*)App->gui->CreateLabel("FULLSCREEN", buttons[SCREEN]->pos.first + x / 100, buttons[SCREEN]->pos.second + y / 150, nullptr);
	}
	else window_lbl = (Label*)App->gui->CreateLabel("WINDOWED", buttons[SCREEN]->pos.first + x / 100, buttons[SCREEN]->pos.second + y / 150, nullptr);

	settings_lbl->SetSize(24);
	mute_lbl->SetSize(14);
	window_lbl->SetSize(14);

	ui_menu.in_window.push_back(images[SETTINGS]);
	ui_menu.in_window.push_back(buttons[MUTE]);
	ui_menu.in_window.push_back(buttons[SCREEN]);
	ui_menu.in_window.push_back(settings_lbl);
	ui_menu.in_window.push_back(mute_lbl);
	ui_menu.in_window.push_back(window_lbl);

	ui_menu.WindowOff();
	ui_menu.SetFocus(elements[0].position.first, elements[0].position.second, x, y);


	App->gui->SetPriority();

	/*App->audio->active = false;*/
	//LOAD FX
	fx_button_click = App->audio->LoadFx("audio/fx/fx_button_click.wav");
	App->audio->PlayMusic("audio/music/m_menu.ogg", 0.0f);

	return ret;
}

bool MenuScene::PreUpdate()
{
	return true;
}

bool MenuScene::Update(float dt)
{
	// --------------------------------------------
	//						UI
	//---------------------------------------------
	if (buttons[MUTE]->current == CLICKUP)
	{
		if (App->audio->active == true) {
			App->audio->PlayMusic(nullptr, 0.0f);
			App->audio->active = false;
			mute_lbl->SetString("UNMUTE");
		}
		else {
			App->audio->active = true;
			App->audio->PlayMusic("audio/music/m_menu.ogg");
			mute_lbl->SetString("MUTE");
		}
	}
	if (buttons[SCREEN]->current == CLICKUP)
	{
		if (App->win->IsFullScreen()) {
			App->win->ChangeToWindow();
			window_lbl->SetString("FULLSCREEN");
		}
		else {
			App->win->ChangeToFullScreen();
			window_lbl->SetString("WINDOWED");
		}
	}
	if (ui_menu.IsEnabled()) App->gui->Focus(ui_menu.FocusArea());

	if (buttons[OPTIONS]->current == CLICKUP) {
		if (!ui_menu.IsEnabled()) {
			ui_menu.WindowOn();
		}
		else ui_menu.WindowOff();
	}

	bool cursoron = false;
	for (uint i = 0; i < buttons.size(); ++i) {
		if (buttons[i]->current == HOVER || buttons[i]->current == CLICKIN)
			cursoron = true;
	}

	if (cursoron == true)
		App->gui->cursor->SetCursor(3);
	else App->gui->cursor->SetCursor(0);

	// ---------------------------------------
	return true;
}

bool MenuScene::PostUpdate()
{
	if (buttons[0]->current == CLICKIN)
	{
		App->audio->PlayFx(fx_button_click);
	}
	else if (buttons[0]->current == CLICKUP)
	{
		App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);
		/*App->cutscene->Start();
		App->cutscene->Play("cutscene/first_cutscene.xml", App->sceneManager->level1_scene);*/
	}
	else if (buttons[2]->current == CLICKUP)
	{
		App->quit = true;
	}

	return true;
}

bool MenuScene::CleanUp()
{
	App->gui->DestroyALLUIElements();
	ui_menu.CleanUp();
	elements.clear();
	images.clear();
	buttons.clear();
	return true;
}

