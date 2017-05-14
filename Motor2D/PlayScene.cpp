#include "PlayScene.h"
#include "Application.h"
#include "FogOfWar.h"
#include "Window.h"

PlayScene::PlayScene() : SceneElement("play_scene")
{
}

PlayScene::~PlayScene()
{
}

bool PlayScene::Start()
{
	// Loading UI ====================================================

	uint x, y;
	App->win->GetWindowSize(x, y);

	elements = App->gui->GetElements("LEVEL");

	elements[0].position.first = -STARTING_CAMERA_X + 0;
	elements[0].position.second = -STARTING_CAMERA_Y + 0;

	elements[1].position.first = -STARTING_CAMERA_X + 0;
	elements[1].position.second = -STARTING_CAMERA_Y + y - elements[1].rect.h;

	elements[2].position.first = -STARTING_CAMERA_X + x - elements[2].rect.w;
	elements[2].position.second = -STARTING_CAMERA_Y + y - elements[2].rect.h;

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

	return true;
}

bool PlayScene::PreUpdate()
{
	return true;
}

bool PlayScene::Update(float dt)
{
	return true;
}

bool PlayScene::PostUpdate()
{
	return true;
}

bool PlayScene::CleanUp()
{
	App->gui->DestroyALLUIElements();
	images.clear();
	buttons.clear();
	elements.clear();

	return true;
}
