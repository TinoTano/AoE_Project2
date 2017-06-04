#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include <string>
#include "Entity.h"
#include "Module.h"
#include "Unit.h"

enum fow_id
{
	fow_black,
	fow_grey,
	fow_grey_rup, fow_grey_rdown, fow_grey_rleft, fow_grey_rright , // Filled with black edge

	fow_clear,
	fow_clear_rup, fow_clear_rdown, fow_clear_rleft, fow_clear_rright,
	fow_black_rup, fow_black_rdown, fow_black_rleft, fow_black_rright, // Grey rounded with black edge
	fow_clear_rectupright, fow_clear_rectupleft, fow_clear_rectdownright, fow_clear_rectdownleft,
	fow_clear_sup, fow_clear_sdown, fow_clear_sleft, fow_clear_sright
};

struct in_fog_entity
{
	list<iPoint>	frontier;
	list<iPoint>	current_points;
	iPoint			pos = { 0,0 };
	int				id = -1;
	uint            radium = 0;
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

	void MoveFrontier(iPoint prev_pos, const char* direction, uint id, int amount);
	void MoveArea(in_fog_entity& player, string direction, uint id, int amount);

	// Create Areas

	void FillFrontier();
	void GetEntitiesCircleArea(in_fog_entity& new_player);
	void DeletePicks(in_fog_entity& frontier);
	void GetCurrentPointsFromFrontier(in_fog_entity& player);
	void SoftEdges();

	// Characters

	void ManageEntities();

	// Utilility

	bool IsVisible(iPoint char_pos, Faction faction);
	bool IsFrontier(iPoint point, in_fog_entity& player);
	uint Get(int x, int y);
	bool Save(pugi::xml_node & data) const;
	bool Load(pugi::xml_node & data);
	void DeleteEntityFog(uint id);

	// Atlas

	vector<in_fog_entity>		entities_on_fog;
	list<Entity*>	            entities_not_in_fog;

	SDL_Texture*				texture_grey = nullptr;
	SDL_Texture*				texture_black = nullptr;
	iPoint				        prev_pos = { 0,0 };
	iPoint				        next_pos = { 0,0 };
	uint*						data = nullptr;

private:
	string                      path_texture1;
	string                      path_texture2;
};

#endif