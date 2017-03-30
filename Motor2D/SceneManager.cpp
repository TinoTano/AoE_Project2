#include "SceneManager.h"

SceneManager::SceneManager()
{
	name = "sceneManager";
	level1_scene = new Scene(); scenes.push_back(level1_scene);
	menu_scene = new MenuScene(); scenes.push_back(menu_scene);
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Awake(pugi::xml_node &)
{
	return true;
}

bool SceneManager::Start()
{

	bool ret = true;
	
	
	current_scene = level1_scene;

	
	if (current_scene != nullptr && current_scene != level1_scene)
		current_scene->Start();

	return ret;
}

bool SceneManager::PreUpdate()
{
	if (current_scene != nullptr)
		current_scene->PreUpdate();
	return true;
}

bool SceneManager::Update(float dt)
{
	if (current_scene != nullptr)
		current_scene->Update(dt);
	return true;;
}

bool SceneManager::PostUpdate()
{
	if (current_scene != nullptr)
		current_scene->PostUpdate();
	return true;
}

bool SceneManager::CleanUp()
{
	if (current_scene != nullptr)
		current_scene->CleanUp();
	return true;
}

void SceneManager::ChangeScene(SceneElement* last_scene, SceneElement * new_scene)
{
	last_scene->CleanUp();
	new_scene->Start();
	current_scene = new_scene;
}
