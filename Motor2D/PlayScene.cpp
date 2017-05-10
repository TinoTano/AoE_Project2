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

	// Labels
	wood = (Label*)App->gui->CreateLabel(to_string(woodCount), -STARTING_CAMERA_X + 50, -STARTING_CAMERA_Y + 5, nullptr);
	wood->SetColor({ 255, 255, 255 ,255 });
	food = (Label*)App->gui->CreateLabel(to_string(foodCount), -STARTING_CAMERA_X + 150, -STARTING_CAMERA_Y + 5, nullptr);
	food->SetColor({ 255, 255, 255 ,255 });
	gold = (Label*)App->gui->CreateLabel(to_string(goldCount), -STARTING_CAMERA_X + 280, -STARTING_CAMERA_Y + 5, nullptr);
	gold->SetColor({ 255, 255, 255 ,255 });
	stone = (Label*)App->gui->CreateLabel(to_string(stoneCount), -STARTING_CAMERA_X + 360, -STARTING_CAMERA_Y + 5, nullptr);
	stone->SetColor({ 255, 255, 255 ,255 });
	villagers = (Label*)App->gui->CreateLabel("0/0", -STARTING_CAMERA_X + 480, -STARTING_CAMERA_Y + 5, nullptr);
	villagers->SetColor({ 255, 255, 255 ,255 });

	// Priority
	App->gui->SetPriority();

	// Resources
	App->map->LoadResources(App->map->map_file.child("map"));

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
	elements.clear();
	images.clear();
	buttons.clear();

	App->fog->CleanUp();
	return true;
}
