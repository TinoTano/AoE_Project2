#ifndef __SCENE_ELEMENT__H
#define __SCENE_ELEMENT__H

#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <string>
#include <vector>

using namespace std;

class SceneElement {
public:
	SceneElement(char* _name) { name = _name; };
	virtual ~SceneElement();
	virtual bool Start() { return true; };
	virtual bool PreUpdate() { return true; };
	virtual bool Update(float dt) { return true; };
	virtual bool PostUpdate() { return true; };
	virtual bool CleanUp() { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };
	virtual bool Save(pugi::xml_node&) { return true; };

	string name;
	bool active;
};

#endif // SCENE_ELEMENT
#pragma once
