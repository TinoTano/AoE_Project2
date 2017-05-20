#include "MenuScene.h"
#include "Render.h"
#include "Application.h"
#include "Input.h"
#include "SceneManager.h"
#include "Window.h"
#include "Audio.h"
#include "CutSceneManager.h"
#include "Video.h"
#include "Fonts.h"

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

	elements[3].position.first = (x / 4) + (x / 20);
	elements[3].position.second = elements[1].position.second;

	elements[4].position.first = elements[3].position.first + (x / 150);
	elements[4].position.second = elements[1].position.second + (y / 13);

	elements[5].position.first = elements[3].position.first + (x / 150) + elements[4].rect.w + (x/40);
	elements[5].position.second = elements[4].position.second;

	elements[6].position.first = elements[1].position.first + x / 180;
	elements[6].position.second = elements[1].position.second + y / 50;

	elements[7].position.first = elements[6].position.first;
	elements[7].position.second = elements[6].position.second + y / 100 + elements[6].detect_sections.front().h;

	elements[8].position.first = elements[7].position.first;
	elements[8].position.second = elements[7].position.second + y / 100 + elements[7].detect_sections.front().h;

	elements[9].position.first = elements[2].position.first + (x / 100);
	elements[9].position.second = elements[2].position.second + (y / 15);

	elements[10].position.first = elements[9].position.first;
	elements[10].position.second = elements[9].position.second + (y / 20);

	elements[11].position.first = elements[3].position.first + (x / 30);
	elements[11].position.second = elements[3].position.second + (y / 5) + (y / 30);

	elements[12].position.first = elements[11].position.first;
	elements[12].position.second = elements[11].position.second + (y / 15);



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

	new_game_lbl = (Label*)App->gui->CreateLabel("New Game", buttons[NEWGAME]->pos.first + x / 20, buttons[NEWGAME]->pos.second, App->font->fonts[EIGHTEEN]);

	load_game_lbl = (Label*)App->gui->CreateLabel("Load Game", buttons[LOADGAME]->pos.first + x / 20, buttons[LOADGAME]->pos.second, App->font->fonts[EIGHTEEN]);

	freepeople_lbl = (Label*)App->gui->CreateLabel("FREE PEOPLE", images[FREEPEOPLE]->pos.first + x / 40, images[FREEPEOPLE]->pos.second + images[FREEPEOPLE]->section.h + y / 500, nullptr);
	sauronarmy_lbl = (Label*)App->gui->CreateLabel("SAURON ARMY", images[SAURONARMY]->pos.first + x / 40, images[SAURONARMY]->pos.second + images[SAURONARMY]->section.h + y / 500, nullptr);
	map_lbl = (Label*)App->gui->CreateLabel("Map: Riverdale", images[BACKGROUND_SKIRMISH]->pos.first + x / 15, images[BACKGROUND_SKIRMISH]->pos.second + y / 500, App->font->fonts[FOURTEEN]);
	
	skirmish_menu.in_window.push_back(images[BACKGROUND_SKIRMISH]);
	skirmish_menu.in_window.push_back(images[FREEPEOPLE]);
	skirmish_menu.in_window.push_back(images[SAURONARMY]);
	skirmish_menu.in_window.push_back(buttons[NEWGAME]);
	skirmish_menu.in_window.push_back(buttons[LOADGAME]);
	skirmish_menu.in_window.push_back(new_game_lbl);
	skirmish_menu.in_window.push_back(load_game_lbl);
	skirmish_menu.in_window.push_back(map_lbl);
	skirmish_menu.in_window.push_back(freepeople_lbl);
	skirmish_menu.in_window.push_back(sauronarmy_lbl);

	skirmish_menu.WindowOff();
	skirmish_menu.SetFocus(images[BACKGROUND_SKIRMISH]->pos.first, images[BACKGROUND_SKIRMISH]->pos.second, x, y);

	settings_lbl = (Label*)App->gui->CreateLabel("Settings", images[SETTINGS]->pos.first + x / 100, images[SETTINGS]->pos.second, App->font->fonts[TWENTYSIX]);

	if (App->audio->active == true) {
		mute_lbl = (Label*)App->gui->CreateLabel("MUTE", buttons[MUTE]->pos.first + x / 50, buttons[MUTE]->pos.second + y / 150, App->font->fonts[FOURTEEN]);
	}
	else mute_lbl = (Label*)App->gui->CreateLabel("UNMUTE", buttons[MUTE]->pos.first + x / 50, buttons[MUTE]->pos.second + y / 150, App->font->fonts[FOURTEEN]);

	if (!App->win->IsFullScreen()) {
		window_lbl = (Label*)App->gui->CreateLabel("FULLSCREEN", buttons[SCREEN]->pos.first + x / 100, buttons[SCREEN]->pos.second + y / 150, App->font->fonts[FOURTEEN]);
	}
	else window_lbl = (Label*)App->gui->CreateLabel("WINDOWED", buttons[SCREEN]->pos.first + x / 100, buttons[SCREEN]->pos.second + y / 150, App->font->fonts[FOURTEEN]);


	ui_menu.in_window.push_back(images[SETTINGS]);
	ui_menu.in_window.push_back(buttons[MUTE]);
	ui_menu.in_window.push_back(buttons[SCREEN]);
	ui_menu.in_window.push_back(settings_lbl);
	ui_menu.in_window.push_back(mute_lbl);
	ui_menu.in_window.push_back(window_lbl);

	ui_menu.WindowOff();
	ui_menu.SetFocus(elements[0].position.first, elements[0].position.second, x, y);

	App->gui->SetPriority();

	//LOAD FX
	App->audio->PlayMusic("audio/music/m_menu.ogg", 0.0f);

	return ret;
}

bool MenuScene::PreUpdate()
{
	return true;
}

bool MenuScene::Update(float dt)
{
	//if (App->video->want_to_play == false)
	//{
	//	uint w, h;
	//	App->win->GetWindowSize(w, h);
	//	App->video->PlayVideo("VideoLogoNemesis.ogv");
	//}

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
		else {
			ui_menu.WindowOff();
		}
	}

	if (images[FREEPEOPLE]->current == CLICKUP) {
		team = 0;
	}
	else if (images[SAURONARMY]->current == CLICKUP) {
		team = 1;
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
		App->audio->PlayFx(BUTTON_SOUND);
	}
	else if (buttons[SKIRMISH]->current == CLICKUP)
	{
		
		/*if (!skirmish_menu.IsEnabled()) {
			skirmish_menu.WindowOn();
		}
		else skirmish_menu.WindowOff();*/

		/*App->sceneManager->ChangeScene(this, App->sceneManager->level1_scene);*/
		App->cutscene->Start();
		App->cutscene->Play("cutscene/first_cutscene.xml", App->sceneManager->level1_scene);

		team = -1;
	}
	else if (buttons[2]->current == CLICKUP)
	{
		App->quit = true;
	}
	else if (buttons[NEWGAME]->current == CLICKUP)
	{
	}
	if (final_team != team)
	{
		final_team = team;
		if (final_team == 0)
		{
			App->gui->DestroyUIElement(ring);
			ring = (Image*)App->gui->CreateImage("gui/ring.png", images[FREEPEOPLE]->pos.first, images[FREEPEOPLE]->pos.second, { 0,0, 125, 70 });
		}
		else if (final_team == 1)
		{
			App->gui->DestroyUIElement(ring);
			ring = (Image*)App->gui->CreateImage("gui/ring.png", images[SAURONARMY]->pos.first, images[SAURONARMY]->pos.second, {0,0, 125, 70});
		}
		else if (final_team == -1)
			App->gui->DestroyUIElement(ring);
	}
	return true;
}

bool MenuScene::CleanUp()
{
	ui_menu.CleanUp();
	skirmish_menu.CleanUp();
	App->gui->DestroyALLUIElements();
	elements.clear();
	images.clear();
	buttons.clear();
	return true;
}

