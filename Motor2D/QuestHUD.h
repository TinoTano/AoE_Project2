#include "QuestManager.h"
#include "Gui.h"


class QuestHUD {
public:
	void Start(int, int);

	void Update();

	void CleanUp();

private:
	Quad* quad;
	Label* name, *desc;

};
#pragma once
