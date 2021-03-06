#include "Application.h"
#include "Map.h"
#include "EntityManager.h"
#include "PathFinding.h"
#include "Orders.h"
#include "p2Log.h"

PathFinding::PathFinding() : Module(), map(NULL), width(0), height(0)
{
	name = "pathfinding";
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos, Collider* collider_to_ignore)
{
	uchar t = GetTileAt(pos);
	iPoint worldpos = App->map->MapToWorld(pos.x, pos.y);

	if (!isGameScene) {
		return (t != INVALID_WALK_CODE && t > 0);
	}
	return (t != INVALID_WALK_CODE && !App->collision->FindCollider(worldpos, 0, collider_to_ignore));
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}


// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
list<PathNode>::iterator PathList::Find(const iPoint& point) {

	for (std::list<PathNode>::iterator it = pathNodeList.begin(); it != pathNodeList.end(); ++it)
	{
		if (it->pos == point)
		{
			return it;
		}
	}
	return pathNodeList.end();
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
list<PathNode>::iterator PathList::GetNodeLowestScore()
{
	list<PathNode>::iterator ret = pathNodeList.end();
	int min = 65535;

	for (std::list<PathNode>::iterator it = pathNodeList.begin(); it != pathNodeList.end(); ++it)
	{
		if (it->Score() < min)
		{
			min = it->Score();
			ret = it;
		}
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, int range) const
{
	iPoint cell;

	for (int i = -range; i <= range; i++) {
		for (int j = -range; j <= range; j++) {

			if (!(i == 0 && j == 0)) {

				cell.create(pos.x + i, pos.y + j);
				if (App->pathfinding->IsWalkable(cell))
					list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

			}
		}
	}
	return list_to_fill.pathNodeList.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	/*g = parent->g + this->pos.DistanceManhattan(parent->pos);
	h = pos.DistanceManhattan(destination);*/

	g = parent->g + parent->pos.DistanceOctile(pos);
	h = pos.DistanceOctile(destination) * 10;

	return g + h;
}


void PathNode::IdentifySuccessors(PathList & list_to_fill, iPoint startNode, iPoint endNode, PathFinding* pathfinder) const
{
	PathList neighbours;
	this->FindWalkableAdjacents(neighbours);

	list<PathNode>::iterator neighbour = neighbours.pathNodeList.begin();

	while (neighbour != neighbours.pathNodeList.end())
	{
		int dx = clamp(neighbour->pos.x - this->pos.x, -1, 1);
		int dy = clamp(neighbour->pos.y - this->pos.y, -1, 1);

		PathNode jump_point(-1, -1, iPoint(-1, -1), this);
		bool succed = pathfinder->Jump(this->pos.x, this->pos.y, dx, dy, startNode, endNode, jump_point);

		if (succed == true)
			list_to_fill.pathNodeList.push_back(jump_point);

		++neighbour;
	}
}


bool PathFinding::Jump(int current_x, int current_y, int dx, int dy, iPoint start, iPoint end, PathNode& new_node)
{
	iPoint next(current_x + dx, current_y + dy);

	if (IsWalkable(next, current_unit->collider) == false)
		return false;
	else if (next.x == end.x && next.y == end.y)
	{
		new_node.pos = next;
		return true;
	}

	if (dx != 0 && dy != 0) // Diagonal Case   
	{
		if (!IsWalkable(iPoint(current_x + dx, current_y), current_unit->collider))
		{
			new_node.pos = next;
			return true;
		}
		else if (!IsWalkable(iPoint(current_x, current_y + dy), current_unit->collider))
		{
			new_node.pos = next;
			return true;
		}

		// Check in horizontal and vertical directions for forced neighbors
		// This is a special case for diagonal direction
		if (Jump(next.x, next.y, dx, 0, start, end, new_node) != NULL || Jump(next.x, next.y, 0, dy, start, end, new_node) != NULL)
		{
			new_node.pos = next;
			return true;
		}
	}
	else
	{
		if (dx != 0) // Horizontal case
		{
			if (!IsWalkable(iPoint(current_x, current_y + 1), current_unit->collider))
			{
				if (IsWalkable(iPoint(current_x + dx, current_y + 1), current_unit->collider))
				{
					new_node.pos = next;
					return true;
				}
			}
			else if (!IsWalkable(iPoint(current_x, current_y - 1), current_unit->collider))
			{
				if (IsWalkable(iPoint(current_x + dx, current_y - 1), current_unit->collider))
				{
					new_node.pos = next;
					return true;
				}
			}
		}
		else // Vertical case
		{
			if (!IsWalkable(iPoint(current_x + 1, current_y), current_unit->collider))
			{
				if (IsWalkable(iPoint(current_x + 1, current_y + dy), current_unit->collider))
				{
					new_node.pos = next;
					return true;
				}
			}
			else if (!IsWalkable(iPoint(current_x - 1, current_y), current_unit->collider))
			{
				if (IsWalkable(iPoint(current_x - 1, current_y + dy), current_unit->collider))
				{
					new_node.pos = next;
					return true;
				}
			}
		}

	}

	// If forced neighbor was not found try next jump point
	return Jump(next.x, next.y, dx, dy, start, end, new_node);
}


void PathFinding::CalculatePath(Path * path)
{

	do
	{

		// Move the lowest score cell from open list to the closed list
		list<PathNode>::iterator lowest = path->open.GetNodeLowestScore();
		path->closed.pathNodeList.push_back(*lowest);
		path->open.pathNodeList.erase(lowest);
		list<PathNode>::iterator node = --path->closed.pathNodeList.end();


		// If destination was added, we are done!
		if (node->pos == path->destination)
		{
			path->finished_path.clear();
			// Backtrack to create the final path
			const PathNode* path_node = &(*node);

			while (path_node)
			{
				path->finished_path.push_back(path_node->pos);
				path_node = path_node->parent;
			}

			iPoint* start = &path->finished_path[0];
			iPoint* end = &path->finished_path[path->finished_path.size() - 1];

			while (start < end)
				SWAP(*start++, *end--);

			for (int i = 0; i < path->finished_path.size(); i++) {
				iPoint current_world = App->map->MapToWorld(path->finished_path[i].x, path->finished_path[i].y);
				path->finished_path[i].x = current_world.x + path->difference.x;
				path->finished_path[i].y = current_world.y + path->difference.y;
			}

			iPoint dest = path->finished_path.back();
			vector<iPoint> tempPath;

			for (int i = 0; i < path->finished_path.size(); i++) {
				if (path->finished_path[i] != dest) {
					if (path->finished_path[i+1].DistanceTo(dest) < path->finished_path[i].DistanceTo(dest)) {
					tempPath.push_back(path->finished_path[i]);
					}
				}
			}
			path->finished_path.clear();
			path->finished_path = tempPath;

			path->completed = true;

			break;
		}

		// Fill a list with all adjacent nodes
		path->adjacent.pathNodeList.clear();
		node->IdentifySuccessors(path->adjacent, path->origin, path->destination, this);


		list<PathNode>::iterator i = path->adjacent.pathNodeList.begin();

		while (i != path->adjacent.pathNodeList.end())
		{
			if (path->closed.Find(i->pos) != path->closed.pathNodeList.end())
			{
				++i;
				continue;
			}

			list<PathNode>::iterator adjacent_in_open = path->open.Find(i->pos);

			if (adjacent_in_open == path->open.pathNodeList.end())
			{
				i->CalculateF(path->destination);
				path->open.pathNodeList.push_back(*i);
			}
			else
			{
				if (adjacent_in_open->g > i->g + 1)
				{
					adjacent_in_open->parent = i->parent;
					adjacent_in_open->CalculateF(path->destination);
				}
			}
			++i;
		}

	} while (path->open.pathNodeList.size() > 0);
}

vector<iPoint> PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	vector<iPoint> ret;
	iPoint mapOrigin = App->map->WorldToMap(origin.x, origin.y);
	iPoint current_origin = mapOrigin;
	iPoint difference = origin - App->map->MapToWorld(mapOrigin.x, mapOrigin.y);

	if (IsWalkable(destination))
	{
		if (!IsWalkable(mapOrigin))
		{
			current_origin = FindNearestWalkable(mapOrigin);

			if (IsWalkable(current_origin))
			{
				LOG("Found new origin walkable");
			}

			if (current_origin.x == -1 && current_origin.y == -1)
			{
				LOG("Path no created: Origin no walkable && no near walkable tiles available");
				return ret;
			}

		}
		Path* path = new Path();

		//paths.insert(pair<uint, Path*>(++current_id, path));

		// Start pushing the origin in the open list
		path->open.pathNodeList.push_back(PathNode(0, 0, current_origin, nullptr));

		path->origin = current_origin;
		path->destination = destination;
		path->difference = difference;

		CalculatePath(path); //Id of the path created
		ret = path->finished_path;
	}

	return ret;
}


iPoint PathFinding::FindNearestAvailable(iPoint origin, int range, iPoint target)
{
	if (target.x == -1)
		target = origin;

	PathList nodes;
	PathNode first_node(0, 0, origin, NULL);

	first_node.FindWalkableAdjacents(nodes, range);

	for (list<PathNode>::iterator it = nodes.pathNodeList.begin(); it != nodes.pathNodeList.end(); it++)
		(*it).CalculateF(target);

	if (nodes.pathNodeList.empty())
		return origin;
	else
		return (*nodes.GetNodeLowestScore()).pos;

}

iPoint PathFinding::FindNearestWalkable(const iPoint & origin)
{
	iPoint ret(origin);

	// dx -> direction x  | dy -> direction y  
	// search_in_radius -> finds the nearest walkable tile in a radius (max radius in FIND_RADIUS) 
	int search_in_radius = 1;
	while (search_in_radius != FIND_RADIUS)
	{
		for (int dx = -search_in_radius; dx < search_in_radius; dx++)
		{
			for (int dy = -search_in_radius; dy < search_in_radius; dy++)
			{
				ret.x = origin.x + dx;
				ret.y = origin.y + dy;
				if (IsWalkable(ret))
					return ret; // Found the nearest walkable tile
			}
		}

		++search_in_radius;
	}


	return ret.create(-1, -1);
}

