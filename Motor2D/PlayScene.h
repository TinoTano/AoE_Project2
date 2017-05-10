#pragma once
#ifndef _PLAYSCENE_H_
#define _PLAYSCENE_H_

#include "SceneElement.h"
#include "Gui.h"

#define STARTING_CAMERA_X 1650
#define STARTING_CAMERA_Y -1900

class PlayScene : public SceneElement
{
public:
	PlayScene();
	~PlayScene();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:
	vector<Info> elements;
	vector<Image*> images;
	vector<Button*> buttons;

public:
	int woodCount = 0;
	int foodCount = 0;
	int goldCount = 0;
	int stoneCount = 0;
	Label* wood = nullptr, *food = nullptr, *gold = nullptr, *stone = nullptr, *villagers = nullptr;
};

#endif