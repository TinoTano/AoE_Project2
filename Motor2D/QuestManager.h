#ifndef __QUEST_MANAGER_H__
#define __QUEST_MANAGER_H__

#include "Module.h"
#include <list>
#include <vector>
#include "p2Point.h"
#include "Collision.h"
#include "Building.h"

class Collider;

enum rewardType { NOTHING, INCREASE_GOLD, CREATE_HERO };
enum eventType { DESTROY_EVENT, REACH_EVENT };

class Event
{
public:
	Event(eventType type) : type(type) {};
	~Event() {};

	eventType type;
};

class ReachEvent : public Event
{
public:
	ReachEvent(eventType type) :Event(type) {};
	~ReachEvent() {};

	buildingType building_type;
};

class DestroyEvent : public Event 
{
public:
	DestroyEvent(eventType type) : Event(type) {};
	~DestroyEvent() {};

	buildingType building_type;
};

class Quest
{
public:
	Quest() {};
	~Quest();

public:
	string name;
	string description;
	rewardType reward;
	int id;
	uint state;
	Event* trigger = nullptr;
	Event* step = nullptr;
};

class QuestManager : public Module
{
public:

	QuestManager();
	~QuestManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	Event* createEvent(pugi::xml_node&);

	bool TriggerCallback(Building* t);
	bool StepCallback(Building* t);

	list<Quest*> AllQuests;

private:
	string path;
};

#endif
