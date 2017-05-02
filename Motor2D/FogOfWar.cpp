#include "Application.h"
#include "FogOfWar.h"
#include "Scene.h"
#include "p2Log.h"
#include "Input.h"
#include "Entity.h"
#include "Textures.h"
#include "FileSystem.h"
#include "Map.h"

FogOfWar::FogOfWar() : Module()
{
	name = "fog";
}

FogOfWar::~FogOfWar()
{
}

bool FogOfWar::AddEntity(Entity* new_entity)
{
	if (new_entity == nullptr)
		return false;

	else if (new_entity->faction == FREE_MEN)
	{
		my_unit new_ally;
		new_ally.pos = App->map->WorldToMap(new_entity->collider->pos.x, new_entity->collider->pos.y);
		GetEntitiesCircleArea(new_ally);
		new_ally.id = new_entity->entityID;
		players_on_fog.push_back(new_ally);
		new_entity->is_on_fow = true;

		FillFrontier();
		//RemoveDimJaggies();
		//RemoveDarkJaggies();
		ManageEntities();
	}

	else
	{
		enemy_unit new_enemy;

		new_enemy.pos = App->map->WorldToMap(new_entity->collider->pos.x, new_entity->collider->pos.y);
		new_enemy.visible = false;
		new_enemy.id = new_entity->entityID;
		simple_char_on_fog_pos.push_back(new_enemy);
	}
	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

bool FogOfWar::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	active = false;
	path = conf.child("texture").attribute("file").as_string("");

	return true;
}

bool FogOfWar::Start()
{
	active = true;

	int size = App->map->data.width*App->map->data.height;
	data = new uint[size];

	memset(data, 0, size * sizeof(uint));
	texture = App->tex->Load(path.c_str());

	return true;
}

void FogOfWar::Update(iPoint prev_pos, iPoint next_pos, uint id)
{
	// We look for the direction that the player is moving
	for (vector<my_unit>::iterator curr = App->fog->players_on_fog.begin(); curr != App->fog->players_on_fog.end(); curr++)
	{
		if (curr->id == id)
		{
			if (prev_pos.x + 1 == next_pos.x)
				MoveFrontier(prev_pos, "right", id);

			else if (prev_pos.x - 1 == next_pos.x)
				MoveFrontier(prev_pos, "left", id);

			if (prev_pos.y + 1 == next_pos.y)
				MoveFrontier(prev_pos, "down", id);

			else if (prev_pos.y - 1 == next_pos.y)
				MoveFrontier(prev_pos, "up", id);
		}
	}

	ManageEntities();
}

void FogOfWar::GetEntitiesCircleArea(my_unit& new_player)
{
	new_player.frontier = App->map->PropagateBFS({ new_player.pos.x, new_player.pos.y }, FOW_RADIUM);

	DeletePicks(new_player);

	for (list<iPoint>::iterator it = new_player.frontier.begin(); it != new_player.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;

}

void FogOfWar::MoveFrontier(iPoint prev_pos, const char* direction, uint id)
{
	string direction_str(direction);

	for (vector<my_unit>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		if (it->id == id)
		{
			MoveArea(*it, direction, id);
		}

		// If one player is pushing another

		for (vector<my_unit>::iterator it2 = players_on_fog.begin(); it2 != players_on_fog.end(); it2++)
		{
			if (it->pos == it2->pos && it->id != it2->id)
			{
				MoveArea(*it2, direction, id);
				break;
			}

		}
	}

}

void FogOfWar::GetCurrentPointsFromFrontier(my_unit& player)
{

	//First we add the frontier 

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		player.current_points.push_back(*it);
	}

	// Now we add the tiles inside 

	bool advance = false;

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if (it->x < player.pos.x && it->y != player.pos.y + (FOW_RADIUM - 1) && it->y != player.pos.y - (FOW_RADIUM - 1))
		{
			for (int x = it->x + 1;; x++)
			{
				if (IsFrontier(iPoint(x, it->y), player))
					break;

				else
					player.current_points.push_back({ x, it->y });
			}
		}
	}


	for (list<iPoint>::iterator it = player.current_points.begin(); it != player.current_points.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;

}



void FogOfWar::FillFrontier()
{
	for (vector<my_unit>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{  
		GetCurrentPointsFromFrontier(*it);	
	}
}

void FogOfWar::MoveArea(my_unit& player_unity, string direction_str, uint id)
{
	for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_middle;

	if (direction_str == "right")
	{
		for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
			(*it).x += 1;

		// Optional improvements ----

		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).x += 1;

		// ---

		player_unity.pos.x += 1;
	}


	else if (direction_str == "left")
	{
		for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
			(*it).x -= 1;

		// Optional improvements ----

		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).x -= 1;

		// ---

		player_unity.pos.x -= 1;
	}


	else if (direction_str == "up")
	{
		for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
			(*it).y -= 1;

		// Optional improvements ----

		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).y -= 1;

		// ---

		player_unity.pos.y -= 1;
	}


	else if (direction_str == "down")
	{
		for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
			(*it).y += 1;

		// Optional improvements ----

		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).y += 1;

		// ---

		player_unity.pos.y += 1;
	}


	for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
	{
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;
	}

	// Redraw the others in case of overlaping

	for (vector<my_unit>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		if (it->id != id)
		{
			for (list<iPoint>::iterator it2 = it->current_points.begin(); it2 != it->current_points.end(); it2++)
			{
				data[(*it2).y * App->map->data.width + (*it2).x] = dim_clear;
			}
		}
	}

}

SDL_Rect FogOfWar::GetRect(int fow_id)
{
	SDL_Rect rect_ret = { 0, 0, 96, 51 };

	int columns = 13;

	if (fow_id > 0 && fow_id <= dim_inner_bottom_right)
	{
		rect_ret.y = 0;
		rect_ret.x = 32 * (fow_id - 1);
	}
	else if (fow_id > dim_inner_bottom_right && fow_id <= darkd_inner_bottom_right)
	{
		fow_id -= columns;
		rect_ret.y = 32;
		rect_ret.x = 32 * (fow_id - 1);
	}
	else if (fow_id > darkd_inner_bottom_right && fow_id <= darkc_inner_bottom_right)
	{
		fow_id -= (columns * 2);
		rect_ret.y = 32;
		rect_ret.x = 32 * (fow_id - 1);
	}

	return rect_ret;
}

void FogOfWar::DeletePicks(my_unit& player)
{
	int count = 0;

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if ((*it) == iPoint(player.pos.x, player.pos.y + FOW_RADIUM))
			(*it) = iPoint((*it).x, (*it).y - 1);

		else if ((*it) == iPoint(player.pos.x, player.pos.y - FOW_RADIUM))
			(*it) = iPoint((*it).x, (*it).y + 1);

		else if ((*it) == iPoint(player.pos.x + FOW_RADIUM, player.pos.y))
			(*it) = iPoint((*it).x - 1, (*it).y);

		else if ((*it) == iPoint(player.pos.x - FOW_RADIUM, player.pos.y))
			(*it) = iPoint((*it).x + 1, (*it).y);
	}

}

void FogOfWar::ManageEntities()
{

	for (list<enemy_unit>::iterator it = simple_char_on_fog_pos.begin(); it != simple_char_on_fog_pos.end(); it++)
	{
		if (IsVisible((*it).pos))
			it->visible = true;

		else
			it->visible = false;
	}

}


bool FogOfWar::IsVisible(iPoint char_pos)
{

	if (Get(char_pos.x, char_pos.y) > dim_middle && Get(char_pos.x, char_pos.y) < darkd_middle)
		return true;

	if (Get(char_pos.x, char_pos.y) >= darkc_middle && Get(char_pos.x, char_pos.y) < darkc_inner_bottom_right)
		return true;

	if (Get(char_pos.x, char_pos.y) == dim_clear)
		return true;

	else
		return false;

}

bool FogOfWar::IsFrontier(iPoint point, my_unit& player)
{

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if (point == *it)
			return true;
	}

	return false;
}
