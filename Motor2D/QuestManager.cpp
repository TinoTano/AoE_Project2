#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "QuestManager.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include "QuestHUD.h"
#include "Gui.h"


QuestManager::QuestManager() : Module()
{
	name = ("quest");
}

QuestManager::~QuestManager()
{
	for (std::list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
		AllQuests.erase(it);
}

bool QuestManager::Awake(pugi::xml_node& config)
{
	active = false;
	//Load the path of QuestData file from Config
	LOG("Loading QuestManager data");
	path = config.child("data").attribute("file").as_string();
	return true;
}

bool QuestManager::Start()
{
	bool ret = true;
	active = true;
	//Load QuestData File
	pugi::xml_document	questDataFile;
	char* buff;
	int size = App->fs->Load(path.c_str(), &buff);
	pugi::xml_parse_result result = questDataFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load questData xml file. Pugi error: %s", result.description());
		ret = false;
	}

	else for (pugi::xml_node quest = questDataFile.child("quests").first_child(); quest; quest = quest.next_sibling("quest"))
	{
		//Load quest data from XML
		Quest* new_quest = new Quest();
		new_quest->name = quest.attribute("name").as_string();
		new_quest->description = quest.attribute("description").as_string();
		new_quest->reward = quest.attribute("reward").as_int();
		new_quest->id = quest.attribute("id").as_int();
		//new_quest->trigger = createEvent(quest.child("trigger"));
		new_quest->state = quest.attribute("state").as_uint();

		pugi::xml_node step;
		for (step = quest.child("step"); step; step = step.next_sibling("step"))
		{
			new_quest->steps.push_back(createEvent(step));
		}

		// We first add the quest to the all quest list
		AllQuests.push_back(new_quest);

		// If it's already active we add the gui
		if (new_quest->state == 1)
			App->sceneManager->level1_scene->questHUD.AddActiveQuest(new_quest->name, new_quest->description, new_quest->id);
	}

	return ret;
}

Event* QuestManager::createEvent(pugi::xml_node &it)
{
	//Event factory method
	int type = it.attribute("type").as_int();

	switch (type)
	{
	case (DESTROY_EVENT):
		DestroyEvent* ret = new DestroyEvent(DESTROY_EVENT);
		ret->building_type = (buildingType)it.child("destroy_building").attribute("id").as_int();
		ret->buildings_to_kill = it.child("destroy_entity").attribute("num").as_int();
		return ret;

		// Add more cases
	}
}

// Kill Callbacks ===============================================================================

//bool QuestManager::TriggerKillCallback(unitType t)
//{
//	// Iterates all quests
//	for (std::list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
//	{
//		// Check if the quest is sleep
//		if ((*it)->state == 0)
//		{
//			// Check if  is a CollisionEvent
//			if ((*it)->trigger->type == KILL_EVENT)
//			{
//				//TODO 7: Check if the enemy that died is the one of the quest
//				KillEvent* event = (KillEvent*)(*it)->trigger;
//				if (event->unit_type == t)
//				{
//					LOG("Quest Triggered");
//					(*it)->state = 1;
//					App->sceneManager->level1_scene->questHUD.AddActiveQuest((*it)->name, (*it)->description, (*it)->id);
//
//					return true;
//				}
//			}
//		}
//	}
//	return false;
//}

bool QuestManager::StepKillCallback(buildingType t)
{
	// Iterates all quests
	for (std::list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
	{
		// Check if is active
		if ((*it)->state == 1)
		{
			// Check if it is a KillEvent
			if ((*it)->steps[0]->type == DESTROY_EVENT)
			{
				DestroyEvent* event = ((DestroyEvent*)(*it)->steps[0]);

				if ((t == TROLL_MAULER || t == VENOMOUS_SPIDER) && event->buildings_to_kill > 0) event->buildings_to_kill--;
				if ((t == TROLL_MAULER || t == VENOMOUS_SPIDER) && event->buildings_to_kill == 0)
				{
					LOG("Step Completed");

					// Erase the first step of the steps vector
					(*it)->steps.erase((*it)->steps.begin());

					// Close the quest if there's no more steps and add reward
					if ((*it)->steps.size() == 0)
					{
						App->sceneManager->level1_scene->questHUD.RemoveQuest((*it)->id);

						LOG("Quest completed");
						(*it)->state = 2;

						// Reward
						App->entityManager->player->resources.gold += (*it)->reward;
						App->sceneManager->level1_scene->UpdateResources();
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool QuestManager::CleanUp()
{
	for (list<Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
	{
		for (vector<Event*>::iterator it2 = (*it)->steps.begin(); it2 != (*it)->steps.end(); it2++)
		{
			RELEASE((*it2));
		}

		RELEASE((*it));
	}
	return true;
}

//=============================================================================

Quest::~Quest()
{
}




