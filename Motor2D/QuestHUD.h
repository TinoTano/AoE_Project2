#include "QuestManager.h"
#include "Gui.h"


class QuestsShown {
	public:
	Label* name_lbl, *desc_lbl;
		string name, desc;
	int id;
public:
	void AddQuest(string argname, string argdesc, int argid);
	void CleanUpQuest();
};


class QuestHUD {
public:
	void Start();

	void Update();

	void CleanUp();
	void AddActiveQuest(string, string, int);
	void RemoveQuest(int);
private:
	int winx, winy;
	int questx, questy;
	list<QuestsShown*> vec_quest;
	Label* objective_lbl = nullptr, *desc_lbl = nullptr, *quest_lbl = nullptr, *no_quest_lbl = nullptr;
};
#pragma once
