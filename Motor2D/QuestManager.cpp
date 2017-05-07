#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "QuestManager.h"
#include "FileSystem.h"


QuestManager::QuestManager() : Module()
{
	name = ("quest");
}

QuestManager::~QuestManager()
{
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
		sleepQuests.erase(it);

	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
		activeQuests.erase(it);

	for (std::list <Quest*>::iterator it = closedQuests.begin(); it != closedQuests.end(); it++)
		closedQuests.erase(it);
}

bool QuestManager::Awake(pugi::xml_node& config)
{
	//Load the path of QuestData file from Config
	LOG("Loading QuestManager data");
	path = config.child("data").attribute("file").as_string();
	return true;
}

bool QuestManager::Start()
{
	bool ret = true;

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
		new_quest->id = quest.attribute("id").as_int();
		new_quest->reward = quest.attribute("reward").as_int();

		new_quest->trigger = createEvent(quest.child("trigger"));

		pugi::xml_node step;
		for (step = quest.child("step"); step; step = step.next_sibling("step"))
		{
			new_quest->steps.push_back(createEvent(step));
		}

		sleepQuests.push_back(new_quest);
	}

	return ret;
}

Event * QuestManager::createEvent(pugi::xml_node &it)
{
	//Event factory method
	int type = it.attribute("type").as_int();

	switch (type)
	{
	case (KILL_EVENT):
		KillEvent* ret = new KillEvent(KILL_EVENT);
		ret->unit_type = (unitType)it.child("kill_enemies").attribute("id").as_int();
		ret->enemies_to_kill = it.child("kill_enemies").attribute("num").as_int();
		return ret;

	// Add more cases
	}
}

// Kill Callbacks ===============================================================================

bool QuestManager::TriggerKillCallback(unitType t)
{
	// Iterates all Triggers of sleep quests.
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
	{
		// Check if It is a CollisionEvent
		if ((*it)->trigger->type == KILL_EVENT)
		{
			//TODO 7: Check if the enemy that died is the one of the quest
			KillEvent* event = (KillEvent*)(*it)->trigger;
			if (event->unit_type == t)
			{
				LOG("Quest Triggered");
				activeQuests.push_back((*it));
				sleepQuests.erase(it);

				return true;
			}
		}
	}
	return false;
}

bool QuestManager::StepKillCallback(unitType t)
{
	// Iterates all Steps of all active quests
	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
	{
		// Check if it is a KillEvent
		if ((*it)->steps[0]->type == KILL_EVENT)
		{
			KillEvent* event = ((KillEvent*)(*it)->steps[0]);

			if (event->unit_type == t && event->enemies_to_kill > 0) event->enemies_to_kill--;
			if (event->unit_type == t && event->enemies_to_kill == 0)
			{
				LOG("Step Completed");

				// Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());

				// Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					LOG("Quest completed");
					closedQuests.push_back((*it));
					activeQuests.erase(it);

					// Reward
				}
				return true;
			}
		}
	}
	return false;
}

//=============================================================================

Quest::~Quest()
{
	//Destroy each event of each quest
	delete trigger;
	for (vector <Event*>::iterator it = steps.begin(); it != steps.end(); it++)
	{
		steps.erase(it);
	}
}




