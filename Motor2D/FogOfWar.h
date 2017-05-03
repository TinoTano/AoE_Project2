#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include <string>
#include "Entity.h"
#include "Module.h"

#define FOW_RADIUM 4

enum fow_id
{
	// Dim 

	fow_null,
	dim_middle,
	dim_left,
	dim_right,
	dim_up,
	dim_down,
	dim_bottom_right,
	dim_bottom_left,
	dim_top_right,
	dim_top_left,
	dim_inner_top_left,
	dim_inner_top_right,
	dim_inner_bottom_left,
	dim_inner_bottom_right,

	// Dark dim 

	darkd_middle,
	darkd_left,
	darkd_right,
	darkd_up,
	darkd_down,
	darkd_bottom_right,
	darkd_bottom_left,
	darkd_top_right,
	darkd_top_left,
	darkd_inner_top_left,
	darkd_inner_top_right,
	darkd_inner_bottom_left,
	darkd_inner_bottom_right,

	// Dark clear

	darkc_middle,
	darkc_left,
	darkc_right,
	darkc_up,
	darkc_down,
	darkc_bottom_right,
	darkc_bottom_left,
	darkc_top_right,
	darkc_top_left,
	darkc_inner_top_left,
	darkc_inner_top_right,
	darkc_inner_bottom_left,
	darkc_inner_bottom_right,

	// No mask 

	dim_clear,
};

struct my_unit
{
	list<iPoint>	frontier;
	list<iPoint>	current_points;
	iPoint			pos = { 0,0 };
	int				id = -1;
};

struct enemy_unit
{
	iPoint		pos = { 0, 0 };
	bool		visible = false;
	int			id = -1;
};

class FogOfWar : public Module
{
public:

	FogOfWar();
	bool Awake(pugi::xml_node& conf);
	bool Start();

	void Update(iPoint prev_pos, iPoint next_pos, uint id);

	~FogOfWar();

	bool AddEntity(Entity* new_entity);

	uint RemoveDimJaggies();
	void RemoveDarkJaggies();

	// Update Methods

	void MoveFrontier(iPoint prev_pos, const char* direction, uint id);
	void MoveArea(my_unit& player, string direction, uint id);
	SDL_Rect GetRect(int fow_id);

	// Create Areas

	void FillFrontier();
	void GetEntitiesCircleArea(my_unit& new_player);
	void DeletePicks(my_unit& frontier);
	void GetCurrentPointsFromFrontier(my_unit& player);

	// Characters

	void ManageEntities();

	// Utilility

	bool IsVisible(iPoint char_pos);
	bool IsFrontier(iPoint point, my_unit& player);
	uint Get(int x, int y);

	// Atlas

	SDL_Texture*				texture = nullptr;
	list<enemy_unit>	        simple_char_on_fog_pos;
	iPoint				        prev_pos = { 0,0 };
	iPoint				        next_pos = { 0,0 };
	vector<my_unit>			    players_on_fog;
	uint*						data = nullptr;

private:
	string                      path;
};

#endif