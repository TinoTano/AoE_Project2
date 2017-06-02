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
		new_quest->reward = (rewardType)quest.attribute("reward").as_uint();
		new_quest->id = quest.attribute("id").as_int();
		new_quest->trigger = createEvent(quest.child("trigger"));
		new_quest->state = quest.attribute("state").as_uint();
		new_quest->step = createEvent(quest.child("step"));

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
	int type = it.attribute("type").as_uint();

	if (type == DESTROY_EVENT)
	{
		DestroyEvent* ret = new DestroyEvent(DESTROY_EVENT);
		ret->building_type = (buildingType)it.child("destroy_building").attribute("id").as_uint();
		return ret;
	}

	else if (type == REACH_EVENT)
	{
		ReachEvent* ret = new ReachEvent(REACH_EVENT);
		ret->building_type = (buildingType)it.child("reach_building").attribute("id").as_uint();
		return ret;
	}

	// Add more cases
}

// Kill Callbacks ===============================================================================

bool QuestManager::TriggerCallback(buildingType t)
{
	// Iterates all quests
	for (std::list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
	{
		// Check if the quest is sleep
		if ((*it)->state == 0)
		{
			// Check if  is a Reach Event
			if ((*it)->trigger->type == REACH_EVENT)
			{
				ReachEvent* event = (ReachEvent*)(*it)->trigger;
				if (event->building_type == t)
				{
					LOG("Quest Triggered");
					(*it)->state = 1;
					App->gui->hud->AlertText("New side quest!", 5);
					App->sceneManager->level1_scene->questHUD.AddActiveQuest((*it)->name, (*it)->description, (*it)->id);

					return true;
				}
			}
		}
	}
	return false;
}

bool QuestManager::StepCallback(buildingType t)
{
	// Iterates all quests
	for (std::list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
	{
		// Check if is active
		if ((*it)->state == 1)
		{
			// Check if it is a KillEvent
			if ((*it)->step->type == DESTROY_EVENT)
			{
				DestroyEvent* event = ((DestroyEvent*)(*it)->step);

				if (event->building_type == t)
				{
					LOG("Quest completed");
					App->gui->hud->AlertText("Quest completed", 5);
					App->sceneManager->level1_scene->questHUD.RemoveQuest((*it)->id);
					(*it)->state = 2;

					// Reward
					if ((*it)->reward == INCREASE_GOLD)
					{
						App->entityManager->player->resources.gold += 300;
						App->sceneManager->level1_scene->UpdateResources();
					}
					else if ((*it)->reward == CREATE_HERO)
					{
						// Create a unit here
					}

					return true;
				}
			}
			if ((*it)->step->type == REACH_EVENT)
			{
				ReachEvent* event = ((ReachEvent*)(*it)->step);

				if (event->building_type == t)
				{
					LOG("Quest completed");
					App->gui->hud->AlertText("Quest completed", 5);
					App->sceneManager->level1_scene->questHUD.RemoveQuest((*it)->id);
					(*it)->state = 2;
				}
			}
		}
	}
	return false;
}

//bool QuestManager::Save(pugi::xml_node & d) const
//{
//	pugi::xml_node dataInfo = d.append_child("Quest");
//
//	for (list <Quest*>::const_iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
//	{
//		dataInfo.append_attribute("state") = (*it)->state;
//	}
//
//	return true;
//}
//
//bool QuestManager::Load(pugi::xml_node & d)
//{
//	pugi::xml_node node = d.child("Quest");
//
//	for (list <Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
//	{
//		(*it)->state = node.attribute("state").as_uint();
//		node = node.next_sibling("Quest");
//	}
//
//	return true;
//}

bool QuestManager::CleanUp()
{
	for (list<Quest*>::iterator it = AllQuests.begin(); it != AllQuests.end(); it++)
	{
		delete((*it)->trigger);
		delete((*it)->step);
		RELEASE((*it));
	}

	AllQuests.clear();

	return true;
}

//=============================================================================

Quest::~Quest()
{
}






