// ----------------------------------------------------
// Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __MODULE_H__
#define __MODULE_H__

#include "p2Defs.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "SDL\include\SDL.h"

using namespace std;

class App;
struct Collider;

class Module
{
public:

	Module() : active(false)
	{}

	virtual ~Module()
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	virtual void OnCollision(Collider* c1, Collider* c2)
	{}

	void Enable()
	{
		if (active == false)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active == true)
		{
			active = false;
			CleanUp();
		}
	}

public:

	string		name;
	bool		active;

};

#endif // __MODULE_H__