#include "SceneManager.h"

SceneManager::SceneManager()
{
	name = "sceneManager";
	level1_scene = new Scene(); scenes.push_back(level1_scene);
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

	/*
	if (current_scene != nullptr)
		current_scene->Start();
*/
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
