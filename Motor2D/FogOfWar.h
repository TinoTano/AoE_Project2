#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include <string>
#include "Entity.h"
#include "Module.h"
#include "Unit.h"

#define FOW_RADIUM 4

enum fow_id
{
	fow_black,
	fow_grey,
	fow_clear,
};

struct in_fog_entity
{
	list<iPoint>	frontier;
	list<iPoint>	current_points;
	iPoint			pos = { 0,0 };
	int				id = -1;
};

class FogOfWar : public Module
{
public:

	FogOfWar();
	~FogOfWar();
	bool Awake(pugi::xml_node& conf);
	bool Start();

	void Update(iPoint prev_pos, iPoint next_pos, uint id);
	bool CleanUp();

	bool AddEntity(Entity* new_entity);

	// Update Methods

	void MoveFrontier(iPoint prev_pos, const char* direction, uint id);
	void MoveArea(in_fog_entity& player, string direction, uint id);

	// Create Areas

	void FillFrontier();
	void GetEntitiesCircleArea(in_fog_entity& new_player);
	void DeletePicks(in_fog_entity& frontier);
	void GetCurrentPointsFromFrontier(in_fog_entity& player);

	// Characters

	void ManageEntities();

	// Utilility

	bool IsVisible(iPoint char_pos, Faction faction);
	bool IsFrontier(iPoint point, in_fog_entity& player);
	uint Get(int x, int y);

	// Atlas

	vector<in_fog_entity>		entities_on_fog;
	list<Entity*>	            entities_not_in_fog;

	SDL_Texture*				texture = nullptr;
	iPoint				        prev_pos = { 0,0 };
	iPoint				        next_pos = { 0,0 };
	uint*						data = nullptr;

private:
	string                      path;
};

#endif