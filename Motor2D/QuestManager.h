#ifndef __QUEST_MANAGER_H__
#define __QUEST_MANAGER_H__

#include "Module.h"
#include <list>
#include <vector>
#include "p2Point.h"
#include "Collision.h"
#include "Unit.h"

class Collider;
enum EVENT_TYPE
{
	//Define event types
	KILL_EVENT = 0
};

class Event
{
public:
	Event(EVENT_TYPE type) : type(type) {};
	~Event() {};

	EVENT_TYPE type;
};

class KillEvent : public Event // Right now it only works for enemy units
{
public:
	KillEvent(EVENT_TYPE type) :Event(type) {};

	~KillEvent() {};

	unitType unit_type;
	uint enemies_to_kill;
};

class Quest
{
public:
	Quest() {};
	~Quest();

public:
	string name;
	string description;
	int reward;
	int id;
	Event* trigger = nullptr;
	vector <Event*> steps;
};

class QuestManager : public Module
{
public:

	QuestManager();
	~QuestManager();

	bool Awake(pugi::xml_node&);
	bool Start();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;


	Event* createEvent(pugi::xml_node&);

	//Callbacks for each event type
	bool TriggerKillCallback(unitType t);
	bool StepKillCallback(unitType t);

	void DestroyQuest(Quest*);
	Quest* AddQuest(string name, string description, int reward, int id, Event* trigger, vector<Event*> steps);

private:
	string path;

	list<Quest*> sleepQuests;
	list<Quest*> closedQuests;
public:
	list<Quest*> activeQuests;
};

#endif
