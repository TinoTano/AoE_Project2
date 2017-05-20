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
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	iPoint worldpos = App->map->MapToWorld(pos.x, pos.y);

	return (t != INVALID_WALK_CODE && !App->collision->FindCollider(worldpos));
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

	// Needs optimization for diagonals if sides are not walkable

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
	g = parent->g + 1;
	h = pos.DistanceManhattan(destination);

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

	if (IsWalkable(next) == false)
		return false;
	else if (next.x == end.x && next.y == end.y)
	{
		new_node.pos = next;
		return true;
	}

	if (dx != 0 && dy != 0) // Diagonal Case   
	{
		if (!IsWalkable(iPoint(current_x + dx, current_y)))
		{
			new_node.pos = next;
			return true;
		}
		else if (!IsWalkable(iPoint(current_x, current_y + dy)))
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
			if (!IsWalkable(iPoint(current_x, current_y + 1)))
			{
				if (IsWalkable(iPoint(current_x + dx, current_y + 1)))
				{
					new_node.pos = next;
					return true;
				}
			}
			else if (!IsWalkable(iPoint(current_x, current_y - 1)))
			{
				if (IsWalkable(iPoint(current_x + dx, current_y - 1)))
				{
					new_node.pos = next;
					return true;
				}
			}
		}
		else // Vertical case
		{
			if (!IsWalkable(iPoint(current_x + 1, current_y)))
			{
				if (IsWalkable(iPoint(current_x + 1, current_y + dy)))
				{
					new_node.pos = next;
					return true;
				}
			}
			else if (!IsWalkable(iPoint(current_x - 1, current_y)))
			{
				if (IsWalkable(iPoint(current_x - 1, current_y + dy)))
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

	while (path->open.pathNodeList.size() > 0)
	{
		list<PathNode>::iterator lowest_score_node = path->open.GetNodeLowestScore(); // Get the lowest score node from the open list
		path->closed.pathNodeList.push_back(*lowest_score_node);						  // Adds it to the closed list
		path->open.pathNodeList.erase(lowest_score_node);								  // Delete the lowest_score_node from the open list
		list<PathNode>::iterator next_tile = --path->closed.pathNodeList.end();		  // Next tile will be the newly added node

		if (next_tile->pos == path->destination)
		{
			path->finished_path.clear();
			const PathNode* path_node = &(*next_tile);

			while (path_node) // We backtrack to create the resulting path
			{
				path->finished_path.push_back(path_node->pos); // That path will be stored in finished_path, inside path
				path_node = path_node->parent;
			}

			path->finished_path.reverse();

			break;	// As the path is completed we exit the loop
		}
		path->adjacent.pathNodeList.clear();											// Fill a list with all adjacent nodes
		next_tile->IdentifySuccessors(path->adjacent, path->origin, path->destination, this);

		for (std::list<PathNode>::iterator it = path->adjacent.pathNodeList.begin(); it != path->adjacent.pathNodeList.end();)
		{																			// Iterate for every adjacent node
			if (path->closed.Find(it->pos) != path->closed.pathNodeList.end())
			{
				++it;
				continue;
			}

			list<PathNode>::iterator adjacent_in_open = path->open.Find(it->pos);	// save from the open list to adjacent_in_open
																					// the node that has the position of the current
			if (adjacent_in_open == path->open.pathNodeList.end())						// iterator
			{
				it->CalculateF(path->destination);
				path->open.pathNodeList.push_back(*it);
			}
			else if (adjacent_in_open->g > it->g + 1)
			{
				adjacent_in_open->parent = it->parent;
				adjacent_in_open->CalculateF(path->destination);
			}
			++it;
		}
	}
}



void PathFinding::Repath(Unit* unit) {

	if (!unit->path.empty()) {

		for (list<iPoint>::iterator it = unit->path.begin(); it != unit->path.end(); it++) {
			if (!IsWalkable(*it))
				unit->path.erase(it);
		}

		if (!unit->path.empty()) {
			unit->destinationTileWorld = App->map->MapToWorld(unit->path.back().x, unit->path.back().y);
			unit->order_list.push_front(new MoveToOrder(unit, unit->destinationTileWorld));

		}
	}
	else if (unit->destinationTileWorld.DistanceTo(unit->entityPosition) > 20) 
		unit->order_list.push_front(new MoveToOrder(unit, unit->destinationTileWorld));

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

