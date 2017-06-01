#ifndef __QUEST_MANAGER_H__
#define __QUEST_MANAGER_H__

#include "Module.h"
#include <list>
#include <vector>
#include "p2Point.h"
#include "Collision.h"
#include "Building.h"

class Collider;
enum EVENT_TYPE
{
	//Define event types
	DESTROY_EVENT = 0
};

class Event
{
public:
	Event(EVENT_TYPE type) : type(type) {};
	~Event() {};

	EVENT_TYPE type;
};

class DestroyEvent : public Event // Right now it only works for enemy units
{
public:
	DestroyEvent(EVENT_TYPE type) :Event(type) {};

	~DestroyEvent() {};

	buildingType building_type;
	uint buildings_to_kill;
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
	uint state;
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
	bool CleanUp();

	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

	Event* createEvent(pugi::xml_node&);

	/*bool TriggerKillCallback(unitType t);*/
	bool StepKillCallback(buildingType t);

private:
	string path;

	list<Quest*> AllQuests;
};

#endif
