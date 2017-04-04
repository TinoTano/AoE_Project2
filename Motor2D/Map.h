#ifndef __MAP_H__
#define __MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "Module.h"
#include "p2Point.h"
#include "Resource.h"

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		string name;
		int value;
	};

	~Properties()
	{
		Property* item;
		item = propertyList.front();

		for (list<Property*>::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			RELEASE(*it);
		}

		propertyList.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	list<Property*>	propertyList;
};

// ----------------------------------------------------
struct MapLayer
{
	string		name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	string				name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	int					mapWidth;
	int					mapHeight;
	SDL_Color			background_color;
	MapTypes			type;
	list<TileSet*>		tilesets;
	list<MapLayer*>		layers;
	list<Resource*>		noInteractResources;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
	bool LoadResources(pugi::xml_node& node);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;
	pugi::xml_document	map_file;

private:

	string				folder;
	bool				map_loaded;
	SDL_Surface*		fogSurface;
};

#endif // __MAP_H__