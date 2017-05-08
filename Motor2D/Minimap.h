#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "Module.h"
#include "p2Point.h"

class Minimap : public Module
{
public:

	Minimap();

	//Destructor
	virtual ~Minimap();

	//Called before render is available
	bool Awake(pugi::xml_node&);

	//Called before quitting
	bool CleanUp();

	//Draw terrain
	void DrawTerrain(int x, int y);

	//Draw units
	void DrawUnits();

};


#endif //__MINIMAP_H__