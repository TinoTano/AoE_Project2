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

	//Initialize minimap
	void InitMinimap();

	//Draw terrain
	void DrawTerrain(int x, int y, int r, int g, int b);

	//Draw units
	void DrawUnits();

private:

	iPoint minimapPos;
	double minimapRatio;


};


#endif //__MINIMAP_H__