#include "Application.h"
#include "Render.h"
#include "FileSystem.h"
#include "Textures.h"
#include "Map.h"
#include "p2Log.h"
#include <math.h>
#include "EntityManager.h"
#include "Resource.h"
#include "FogOfWar.h"
#include "Minimap.h"

Map::Map() : Module(), map_loaded(false)
{
	name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();

	return ret;
}

void Map::Draw()
{
	if (map_loaded == false)
		return;

	SDL_Rect cam = App->render->culling_cam;

	for (list<MapLayer*>::iterator it = data.layers.begin(); it != data.layers.end(); it++)
	{
		MapLayer* layer = *it;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				int visibility = App->fog->Get(x, y);

				iPoint tileWorld = MapToWorld(x, y);

				/*if (tile_id > 0 && visibility != 0) {*/
					if (tile_id > 0) {
					if (App->render->CullingCam(tileWorld))
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);
						SDL_Rect r = tileset->GetTileRect(tile_id);
						App->render->Blit(tileset->texture, tileWorld.x, tileWorld.y, &r);

					/*	if (visibility == fow_grey)
						{
							r = { 0, 0, 96, 51 };
							App->render->Blit(App->fog->texture, tileWorld.x, tileWorld.y, &r);
						}*/
					}
				}
			}
		}
	}
}


int Properties::Get(const char* value, int default_value) const
{
	for (list<Property*>::const_iterator it = propertyList.begin(); it != propertyList.end(); it++)
	{
		if ((*it)->name == value)
			return (*it)->value;
	}

	return default_value;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	list<TileSet*>::const_iterator it = data.tilesets.begin();
	TileSet* set = *it;

	while (it != data.tilesets.end())
	{
		if (id < (*it)->firstgid)
		{
			set = *(--it);
			break;
		}
		set = *it;
		it++;
	}

	return set;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2);
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}


// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	list<TileSet*>::iterator tileSetIt;
	tileSetIt = data.tilesets.begin();

	while (tileSetIt != data.tilesets.end())
	{
		RELEASE(*tileSetIt);
		tileSetIt++;
	}
	data.tilesets.clear();

	// Remove all layers
	list<MapLayer*>::iterator mapLayerIt;
	mapLayerIt = data.layers.begin();

	while (mapLayerIt != data.layers.end())
	{
		RELEASE(*mapLayerIt);
		mapLayerIt++;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;
	string tmp = folder.c_str() + (string)file_name;

	char* buf;
	int size = App->fs->Load(tmp.c_str(), &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);

	RELEASE(buf);

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.push_back(lay);
	}


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		list<TileSet*>::iterator tile_set_it = data.tilesets.begin();
		while(tile_set_it != data.tilesets.end())
		{
			TileSet* s = *tile_set_it;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			tile_set_it++;
		}

		list<MapLayer*>::iterator item_layer_it = data.layers.begin();
		while(item_layer_it != data.layers.end())
		{
			MapLayer* l = *item_layer_it;
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer_it++;
		}
		data.mapWidth = data.width * data.tile_width;
		data.mapHeight = data.height * data.tile_height;
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.length() > 0)
		{
			string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 4);
			blue = bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = tileset_node.attribute("name").as_string();
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.propertyList.push_back(p);
		}
	}

	return ret;
}

bool Map::LoadResources(pugi::xml_node & node)
{
	bool ret = true;
	data.mapWidth = data.width * data.tile_width;
	data.mapHeight = data.height * data.tile_height;

	pugi::xml_node resourceNode;

	for (resourceNode = node.child("objectgroup"); resourceNode; resourceNode = resourceNode.next_sibling("objectgroup"))
	{
		uint type = 0;
		string name = resourceNode.attribute("name").as_string();
		if (name == "gold") {
			type = GOLD_MINE;
		}
		else if (name == "stone") {
			type = STONE_MINE;
		}
		else if (name == "forest_rocks") {
			type = FOREST_ROCK;
		}
		else if (name == "rocks") {
			type = ROCK_MINE;
		}
		else if (name == "greentrees") {
			type = GREEN_TREE;
		}
		else if (name == "blacktrees") {
			type = BLACK_TREE;
		}
		else if (name == "mount1") {
			type = MOUNT_1;
		}
		else if (name == "mount2") {
			type = MOUNT_2;
		}
		else if (name == "mount3") {
			type = MOUNT_3;
		}
		else if (name == "mount4") {
			type = MOUNT_4;
		}
		else if (name == "mount5") {
			type = MOUNT_5;
		}
		else if (name == "mount6") {
			type = MOUNT_6;
		}

		pugi::xml_node prop;
		for (prop = resourceNode.child("object"); prop; prop = prop.next_sibling("object"))
		{
			Resource* resource = App->entityManager->CreateResource(prop.attribute("x").as_int(), prop.attribute("y").as_int(), (resourceItem)type);
			//	App->fog->AddEntity(resource);
		}
	}
	return ret;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	list<MapLayer*>::const_iterator it;

	for(it = data.layers.begin(); it != data.layers.end(); it++)
	{
		MapLayer* layer = *it;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				
				if(tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

list<iPoint> Map::PropagateBFS(iPoint origin, int field_of_view)
{
	list<iPoint>		frontier;
	vector<iPoint>		visited;

	frontier.push_back(origin);
	visited.push_back(origin);

	int count = 0;

	int current_layer = 0;
	int layer_done = 4;

	while (current_layer < field_of_view)
	{
		iPoint curr = frontier.front();
		bool is_on_list = false;

		if (curr != iPoint(0, 0))
		{
			iPoint neighbors[4];
			neighbors[0].create(curr.x + 1, curr.y);
			neighbors[1].create(curr.x, curr.y + 1);
			neighbors[2].create(curr.x - 1, curr.y);
			neighbors[3].create(curr.x, curr.y - 1);

			frontier.pop_front();

			for (uint i = 0; i < 4; i++)
			{
				for (vector<iPoint>::const_iterator it = visited.cbegin(); it != visited.cend(); it++)
				{
					is_on_list = false;

					if (neighbors[i] == *it)
					{
						is_on_list = true;
						break;
					}
				}

				if (!is_on_list)
				{
					frontier.push_back(neighbors[i]);
					visited.push_back(neighbors[i]);
					count++;
				}
			}
		}

		if (count == layer_done)
		{
			layer_done = layer_done + 4;
			count = 0;
			current_layer++;
		}
	}

	return frontier;

}

