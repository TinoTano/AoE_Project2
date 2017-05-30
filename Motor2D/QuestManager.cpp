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
		new_quest->name = quest.attribute("name").as_string();
		new_quest->description = quest.attribute("description").as_string();
		new_quest->reward = quest.attribute("reward").as_int();
		new_quest->id = quest.attribute("id").as_int();

		new_quest->trigger = createEvent(quest.child("trigger"));

		pugi::xml_node step;
		for (step = quest.child("step"); step; step = step.next_sibling("step"))
		{
			new_quest->steps.push_back(createEvent(step));
		}

		uint state = quest.attribute("state").as_uint();

		if (state == 0) sleepQuests.push_back(new_quest);
		else if (state == 1)
		{
			activeQuests.push_back(new_quest);
			App->sceneManager->level1_scene->questHUD.AddActiveQuest(new_quest->name, new_quest->description, new_quest->id);
		}
		else closedQuests.push_back(new_quest);
	}

	return ret;
}

bool QuestManager::Load(pugi::xml_node & data)
{
	for (list<Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.begin(); ++it)
	{
		/*DestroyQuest(*it);*/
	}
	sleepQuests.clear();

	for (pugi::xml_node sleepQuests = data.child("SleepQuest"); sleepQuests; sleepQuests = sleepQuests.next_sibling("SleepQuests")) {
		string Name(sleepQuests.attribute("Name").as_string());
		string Desc(sleepQuests.attribute("Desc").as_string());
		int reward = sleepQuests.attribute("Reward").as_int();
		int id = sleepQuests.attribute("id").as_int();
		EVENT_TYPE TriggerType = (EVENT_TYPE)sleepQuests.child("TriggerEvent").attribute("Type").as_int();
		vector<Event*> StepEvents;

		for (pugi::xml_node step_events = sleepQuests.child("StepEvent"); step_events; step_events = step_events.next_sibling("StepEvent"))
		{
			KillEvent* event = new KillEvent((EVENT_TYPE)step_events.attribute("EventType").as_int());
			event->enemies_to_kill = step_events.attribute("Enemies").as_int();
			event->unit_type = (unitType)step_events.attribute("Unit").as_int();
			StepEvents.push_back(event);
		}
		Event* trigger = new Event(TriggerType);
		this->sleepQuests.push_back(AddQuest(name, Desc, reward, id, trigger, StepEvents));
	}


	for (list<Quest*>::iterator it = closedQuests.begin(); it != closedQuests.begin(); ++it)
	{
		/*DestroyQuest(*it);*/
	}
	closedQuests.clear();

	for (pugi::xml_node closedQuests = data.child("SleepQuest"); closedQuests; closedQuests = closedQuests.next_sibling("closedQuests")) {
		string Name(closedQuests.attribute("Name").as_string());
		string Desc(closedQuests.attribute("Desc").as_string());
		int reward = closedQuests.attribute("Reward").as_int();
		int id = closedQuests.attribute("id").as_int();
		EVENT_TYPE TriggerType = (EVENT_TYPE)closedQuests.child("TriggerEvent").attribute("Type").as_int();
		vector<Event*> StepEvents;

		for (pugi::xml_node step_events = closedQuests.child("StepEvent"); step_events; step_events = step_events.next_sibling("StepEvent"))
		{
			KillEvent* event = new KillEvent((EVENT_TYPE)step_events.attribute("EventType").as_int());
			event->enemies_to_kill = step_events.attribute("Enemies").as_int();
			event->unit_type = (unitType)step_events.attribute("Unit").as_int();
			StepEvents.push_back(event);
		}
		Event* trigger = new Event(TriggerType);
		this->closedQuests.push_back(AddQuest(name, Desc, reward, id, trigger, StepEvents));
	}

	for (list<Quest*>::iterator it = activeQuests.begin(); it != activeQuests.begin(); ++it)
	{
		/*DestroyQuest(*it);*/
	}
	activeQuests.clear();

	for (pugi::xml_node activeQuests = data.child("SleepQuest"); activeQuests; activeQuests = activeQuests.next_sibling("activeQuests")) {
		string Name(activeQuests.attribute("Name").as_string());
		string Desc(activeQuests.attribute("Desc").as_string());
		int reward = activeQuests.attribute("Reward").as_int();
		int id = activeQuests.attribute("id").as_int();
		EVENT_TYPE TriggerType = (EVENT_TYPE)activeQuests.child("TriggerEvent").attribute("Type").as_int();
		vector<Event*> StepEvents;

		for (pugi::xml_node step_events = activeQuests.child("StepEvent"); step_events; step_events = step_events.next_sibling("StepEvent"))
		{
			KillEvent* event = new KillEvent((EVENT_TYPE)step_events.attribute("EventType").as_int());
			event->enemies_to_kill = step_events.attribute("Enemies").as_int();
			event->unit_type = (unitType)step_events.attribute("Unit").as_int();
			StepEvents.push_back(event);
		}

		Event* trigger = new Event(TriggerType);
		this->activeQuests.push_back(AddQuest(name, Desc, reward, id, trigger, StepEvents));
	}

	return true;
}

bool QuestManager::Save(pugi::xml_node & data) const
{
	for (list<Quest*>::const_iterator it = sleepQuests.begin(); it != sleepQuests.begin(); ++it) {
		pugi::xml_node sleepQuests = data.append_child("SleepQuest");
		sleepQuests.append_attribute("Name") = (*it)->name.c_str();
		sleepQuests.append_attribute("Desc") = (*it)->description.c_str();
		sleepQuests.append_attribute("Reward") = (*it)->reward;
		sleepQuests.append_attribute("ID") = (*it)->id;
		pugi::xml_node Trigger = sleepQuests.append_child("TriggerEvent");
		KillEvent* trigger = (KillEvent*)(*it)->trigger;
		Trigger.append_attribute("Type") = (*it)->trigger->type;

		for (uint i = 0; i < (*it)->steps.size(); ++i)
		{
			pugi::xml_node Step = sleepQuests.append_child("StepEvent");
			KillEvent* event = (KillEvent*)(*it)->steps[i];
			Step.append_attribute("Unit") = event->unit_type;
			Step.append_attribute("Enemies") = event->enemies_to_kill;
			Step.append_attribute("EventType") = event->type;
		}
	}

	for (list<Quest*>::const_iterator it = closedQuests.begin(); it != closedQuests.begin(); ++it) {
		pugi::xml_node closedQuests = data.append_child("ClosedQuest");
		closedQuests.append_attribute("Name") = (*it)->name.c_str();
		closedQuests.append_attribute("Desc") = (*it)->description.c_str();
		closedQuests.append_attribute("Reward") = (*it)->reward;
		closedQuests.append_attribute("ID") = (*it)->id;
		pugi::xml_node Trigger = closedQuests.append_child("TriggerEvent");
		KillEvent* trigger = (KillEvent*)(*it)->trigger;
		Trigger.append_attribute("Type") = (*it)->trigger->type;

		for (uint i = 0; i < (*it)->steps.size(); ++i)
		{
			pugi::xml_node Step = closedQuests.append_child("StepEvent");
			KillEvent* event = (KillEvent*)(*it)->steps[i];
			Step.append_attribute("Unit") = event->unit_type;
			Step.append_attribute("Enemies") = event->enemies_to_kill;
			Step.append_attribute("EventType") = event->type;
		}
	}

	for (list<Quest*>::const_iterator it = activeQuests.begin(); it != activeQuests.begin(); ++it) {
		pugi::xml_node activeQuests = data.append_child("ActiveQuest");
		activeQuests.append_attribute("Name") = (*it)->name.c_str();
		activeQuests.append_attribute("Desc") = (*it)->description.c_str();
		activeQuests.append_attribute("Reward") = (*it)->reward;
		activeQuests.append_attribute("ID") = (*it)->id;
		pugi::xml_node Trigger = activeQuests.append_child("TriggerEvent");
		KillEvent* trigger = (KillEvent*)(*it)->trigger;
		Trigger.append_attribute("Type") = (*it)->trigger->type;

		for (uint i = 0; i < (*it)->steps.size(); ++i)
		{
			pugi::xml_node Step = activeQuests.append_child("StepEvent");
			KillEvent* event = (KillEvent*)(*it)->steps[i];
			Step.append_attribute("Unit") = event->unit_type;
			Step.append_attribute("Enemies") = event->enemies_to_kill;
			Step.append_attribute("EventType") = event->type;
		}
	}

	return true;
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
				App->sceneManager->level1_scene->questHUD.AddActiveQuest((*it)->name, (*it)->description, (*it)->id);
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

			if ((t == TROLL_MAULER || t == VENOMOUS_SPIDER) && event->enemies_to_kill > 0) event->enemies_to_kill--;
			if ((t == TROLL_MAULER || t == VENOMOUS_SPIDER) && event->enemies_to_kill == 0)
			{
				LOG("Step Completed");

				// Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());

				// Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					App->sceneManager->level1_scene->questHUD.RemoveQuest((*it)->id);

					LOG("Quest completed");
					closedQuests.push_back((*it));
					activeQuests.erase(it);

					// Reward
					App->entityManager->player->resources.gold += (*it)->reward;
					App->sceneManager->level1_scene->UpdateResources();
				}
				return true;
			}
		}
	}
	return false;
}

Quest * QuestManager::AddQuest(string name, string description, int reward, int id, Event * trigger, vector<Event*> steps)
{
	Quest* quest = new Quest();
	quest->name = name;
	quest->description = description;
	quest->reward = reward;
	quest->id = id;
	quest->trigger = trigger;
	quest->steps = steps;
	return quest;
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




