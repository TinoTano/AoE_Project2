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
	int id;
	int reward;
	Event* trigger;
	vector <Event*> steps;
};

class QuestManager : public Module
{
public:

	QuestManager();
	~QuestManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	Event* createEvent(pugi::xml_node&);

	//Callbacks for each event type
	bool TriggerKillCallback(unitType t);
	bool StepKillCallback(unitType t);

private:
	string path;

	list<Quest*> sleepQuests;
	list<Quest*> activeQuests;
	list<Quest*> closedQuests;

};

#endif
