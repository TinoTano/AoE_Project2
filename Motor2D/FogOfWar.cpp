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
		in_fog_entity new_ally;
		new_ally.pos = App->map->WorldToMap(new_entity->collider->pos.x, new_entity->collider->pos.y);
		GetEntitiesCircleArea(new_ally);
		new_ally.id = new_entity->entityID;
		entities_on_fog.push_back(new_ally);

		FillFrontier();
	}

	else
		entities_not_in_fog.push_back(new_entity);

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
	for (vector<in_fog_entity>::iterator curr = App->fog->entities_on_fog.begin(); curr != App->fog->entities_on_fog.end(); curr++)
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

void FogOfWar::GetEntitiesCircleArea(in_fog_entity& new_player)
{
	new_player.frontier = App->map->PropagateBFS({ new_player.pos.x, new_player.pos.y }, FOW_RADIUM);

	DeletePicks(new_player);

	for (list<iPoint>::iterator it = new_player.frontier.begin(); it != new_player.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = fow_clear;

}

void FogOfWar::MoveFrontier(iPoint prev_pos, const char* direction, uint id)
{
	string direction_str(direction);

	for (vector<in_fog_entity>::iterator it = entities_on_fog.begin(); it != entities_on_fog.end(); it++)
	{
		if (it->id == id)
		{
			MoveArea(*it, direction, id);
		}
	}
}

void FogOfWar::GetCurrentPointsFromFrontier(in_fog_entity& player)
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
		data[(*it).y * App->map->data.width + (*it).x] = fow_clear;
}



void FogOfWar::FillFrontier()
{
	for (vector<in_fog_entity>::iterator it = entities_on_fog.begin(); it != entities_on_fog.end(); it++)
	{  
		GetCurrentPointsFromFrontier(*it);	
	}
}

void FogOfWar::MoveArea(in_fog_entity& player_unity, string direction_str, uint id)
{
	for (list<iPoint>::iterator it = player_unity.current_points.begin(); it != player_unity.current_points.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = fow_grey;

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
		data[(*it).y * App->map->data.width + (*it).x] = fow_clear;
	}

	// Redraw the others in case of overlaping

	for (vector<in_fog_entity>::iterator it = entities_on_fog.begin(); it != entities_on_fog.end(); it++)
	{
		if (it->id != id)
		{
			for (list<iPoint>::iterator it2 = it->current_points.begin(); it2 != it->current_points.end(); it2++)
			{
				data[(*it2).y * App->map->data.width + (*it2).x] = fow_clear;
			}
		}
	}

}

void FogOfWar::DeletePicks(in_fog_entity& player)
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
	for (list<Entity*>::iterator it = entities_not_in_fog.begin(); it != entities_not_in_fog.end(); it++)
	{
		if (IsVisible(App->map->WorldToMap((*it)->entityPosition.x, (*it)->entityPosition.y), (*it)->faction))
			(*it)->isActive = true;
		else
			(*it)->isActive = false;
	}
}


bool FogOfWar::IsVisible(iPoint char_pos, Faction faction)
{
	bool ret = false;

	if (faction == NATURE && Get(char_pos.x, char_pos.y) != fow_black) ret = true;
	if (faction == SAURON_ARMY && Get(char_pos.x, char_pos.y) == fow_clear) ret = true;
	if (faction == FREE_MEN) ret = true;

	return ret;
}

bool FogOfWar::IsFrontier(iPoint point, in_fog_entity& player)
{
	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if (point == *it)
			return true;
	}

	return false;
}

bool FogOfWar::CleanUp()
{
	entities_on_fog.clear();
	entities_not_in_fog.clear();

	App->tex->UnLoad(texture);
	return true;
}

