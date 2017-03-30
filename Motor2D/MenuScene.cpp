#include "MenuScene.h"

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
	return true;
}

bool MenuScene::CleanUp()
{
	return true;
}
