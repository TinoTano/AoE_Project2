#include "QuestHUD.h"
#include "Application.h"
#include "Gui.h"
#include "Window.h"


void QuestHUD::Start()
{
	uint x, y;
	App->win->GetWindowSize(x, y);
	winx = x; winy = y;

	objective_lbl = (Label*)App->gui->CreateLabel("Objectives", x - App->render->camera.x - 300, y / 2 - App->render->camera.y, nullptr);
	objective_lbl->SetColor({ 255, 255 ,255 ,255 });
	objective_lbl->SetSize(16);

	desc_lbl = (Label*)App->gui->CreateLabel("Destroy Sauron's tower to win.", x - App->render->camera.x - 300, y / 2 - App->render->camera.y + 22, nullptr);
	desc_lbl->SetColor({ 255, 255 ,255 ,255 });

	questx = x - App->render->camera.x - 300;

	quest_lbl = (Label*)App->gui->CreateLabel("Side Quests", x - App->render->camera.x - 300, y / 2 - App->render->camera.y + 40, nullptr);
	quest_lbl->SetColor({ 255, 255 ,255 ,255 });
	quest_lbl->SetSize(16);

	questy = y / 2 - App->render->camera.y + 58;
}

void QuestHUD::Update()
{

	Sprite quad;
	quad.rect.x = winx - App->render->camera.x - 300;
	quad.rect.y = winy / 2 - App->render->camera.y;
	quad.rect.w = 300;
	quad.rect.h = 175;
	quad.a = 180;
	App->render->ui_toDraw.push_back(quad);

	if (vec_quest.size() == 0 && no_quest_lbl == nullptr) {
		no_quest_lbl = (Label*)App->gui->CreateLabel("No quests to complete", questx,questy, nullptr);
		no_quest_lbl->SetColor({ 255,255,255,255 });
	}
	else if (vec_quest.size() > 0 && no_quest_lbl != nullptr) {
		App->gui->DestroyUIElement(no_quest_lbl);
		no_quest_lbl = nullptr;
	}
	else if (vec_quest.size() > 0) {
		if (vec_quest.front()->name_lbl == nullptr) {
			vec_quest.front()->name += ":";
			vec_quest.front()->name_lbl = (Label*)App->gui->CreateLabel(vec_quest.front()->name , questx, questy, nullptr);
			vec_quest.front()->name_lbl->SetColor({255, 255, 255, 255});
			int size = 14;
			vec_quest.front()->name_lbl->SetSize(size);
			vec_quest.front()->desc_lbl = (Label*)App->gui->CreateLabel(vec_quest.front()->desc, questx, questy + size, nullptr);
			vec_quest.front()->desc_lbl->SetColor({ 255, 255, 255, 255 });
		}
	}

}

void QuestHUD::CleanUp()
{
	App->gui->DestroyUIElement(objective_lbl);
	objective_lbl = nullptr;
	App->gui->DestroyUIElement(desc_lbl);
	desc_lbl = nullptr;
	App->gui->DestroyUIElement(quest_lbl);
	quest_lbl = nullptr;
	App->gui->DestroyUIElement(no_quest_lbl);
	no_quest_lbl = nullptr;

	for (list<QuestsShown*>::iterator it = vec_quest.begin(); it != vec_quest.end(); ++it) {
		RELEASE((it._Ptr->_Myval));
	}
}

void QuestHUD::AddActiveQuest(string argname, string argdesc, int id)
{
	QuestsShown* quest;
	quest = new QuestsShown();
	quest->AddQuest(argname, argdesc, id);

	vec_quest.push_back(quest);
}

void QuestHUD::RemoveQuest(int argid)
{
	QuestsShown* c;
	for (list<QuestsShown*>::iterator it = vec_quest.begin(); it != vec_quest.end(); ++it){
		if (argid == it._Ptr->_Myval->id) {
			c = it._Ptr->_Myval;
			it._Ptr->_Myval->CleanUpQuest();
		}
	}
	vec_quest.remove(c);
}

void QuestsShown::AddQuest(string argname, string argdesc, int argid) {
	name = argname; desc = argdesc; id = argid;
}

void QuestsShown::CleanUpQuest() {
	App->gui->DestroyUIElement(name_lbl);
	name_lbl = nullptr;
	App->gui->DestroyUIElement(desc_lbl);
	desc_lbl = nullptr;
}