#include "SceneManager.h"

SceneManager::SceneManager()
{
	name = "sceneManager";
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
	return true;
}

bool SceneManager::PreUpdate()
{
	return true;
}

bool SceneManager::Update(float dt)
{

	return true;;
}

bool SceneManager::PostUpdate()
{
	return true;
}

bool SceneManager::CleanUp()
{
	return true;
}
