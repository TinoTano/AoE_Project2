#include "Application.h"
#include "Map.h"
#include "EntityManager.h"
#include "PathFinding.h"
#include "p2Log.h"

PathFinding::PathFinding() : Module(), map(NULL), lastPath(DEFAULT_PATH_LENGTH),width(0), height(0)
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

	lastPath.clear();
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
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const list<iPoint>* PathFinding::GetLastPath() const
{
	return &lastPath;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
PathNode* PathList::Find(const iPoint& point) const
{
	for (list<PathNode*>::const_iterator it = pathNodeList.begin(); it != pathNodeList.end(); ++it)
	{
		if ((*it)->pos == point)
		{
			return *it;
		}
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathList::GetNodeLowestScore() const
{
	PathNode* ret = NULL;
	int min = 65535;

	for (list<PathNode*>::const_iterator it = pathNodeList.begin(); it != pathNodeList.end(); ++it)
	{
		if ((*it)->Score() < min)
		{
			min = (*it)->Score();
			ret = *it;
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
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {

			if (!(i == 0 && j == 0)) {

				cell.create(pos.x + i, pos.y + j);
				if (App->pathfinding->IsWalkable(cell) && !App->entityManager->IsOccupied(cell))
					list_to_fill.pathNodeList.push_back(new PathNode(-1, -1, cell, this));

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
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathFinding::CreatePath(iPoint& origin, iPoint& destination, list<iPoint>& path)
{
	lastPath.clear();
	int ret = -1;

	if (!IsWalkable(origin) || origin == destination) {
		return ret;
	}

	if (!IsWalkable(destination) || App->entityManager->IsOccupied(destination)) {
		FindAvailableDestination(destination, origin);
	}

	PathList open;
	PathList close;
	PathNode originTile(0, origin.DistanceTo(destination), origin, nullptr);
	open.pathNodeList.push_back(new PathNode(originTile));
	while (open.pathNodeList.size() > 0)
	{
		PathNode* nextTile = open.GetNodeLowestScore();
		close.pathNodeList.push_back(new PathNode(*nextTile));

		if (close.Find(destination))
		{
			iPoint backtrack(destination);
			while (backtrack != origin)
			{
				lastPath.push_front(backtrack);
				backtrack = close.Find(backtrack)->parent->pos;
			}
			lastPath.push_front(backtrack);

			ret = lastPath.size();
			path = lastPath;
			//clean memory for open and close list
			for (list<PathNode*>::iterator openNode = open.pathNodeList.begin(); openNode != open.pathNodeList.end(); openNode++)
			{
				RELEASE(*openNode);
			}
			for (list<PathNode*>::iterator closeNode = close.pathNodeList.begin(); closeNode != close.pathNodeList.end(); closeNode++)
			{
				RELEASE(*closeNode);
			}

			open.pathNodeList.clear();
			close.pathNodeList.clear();

			break;
		}

		PathList adjacents;
		int walkables = nextTile->FindWalkableAdjacents(adjacents);

		for (list<PathNode*>::iterator curr_adjacent = adjacents.pathNodeList.begin(); curr_adjacent != adjacents.pathNodeList.end(); curr_adjacent++)
		{
			(*curr_adjacent)->parent = nextTile;

			if (close.Find((*curr_adjacent)->pos) != NULL)
				continue;
			if (open.Find((*curr_adjacent)->pos) == NULL)
			{
				(*curr_adjacent)->CalculateF(destination);
				open.pathNodeList.push_back(*curr_adjacent);
			}
			else
			{
				PathNode* oldNode = open.Find((*curr_adjacent)->pos);
				if ((*curr_adjacent)->CalculateF(destination) < oldNode->Score())
				{
					open.pathNodeList.remove(oldNode);
					open.pathNodeList.push_back(*curr_adjacent);
				}
			}
		}

		open.pathNodeList.remove(nextTile);
	}

	return ret;
}


void PathFinding::FindAvailableDestination(iPoint& destination, iPoint& origin)
{
	iPoint newDestination(destination);
	list<iPoint> newDestinationList;
	int distance = 1;

	while (newDestinationList.size() == 0)
	{
		for (int x = -distance; x < distance; x++)
		{
			for (int y = -distance; y < distance; y++)
			{
				newDestination.x = destination.x + x;
				newDestination.y = destination.y + y;
				if (IsWalkable(newDestination) && newDestination != origin && !App->entityManager->IsOccupied(newDestination)) {
					newDestinationList.push_back(newDestination);
				}
			}
		}
		++distance;
	}

	if (newDestinationList.size() == 1) {
		destination = newDestinationList.front();
	}
	else {
		for (list<iPoint>::iterator it = newDestinationList.begin(); it != newDestinationList.end(); it++) {
			if (it._Ptr->_Next != nullptr) {
				if ((*it).DistanceTo(origin) < it._Ptr->_Next->_Myval.DistanceTo(origin)) {
					destination = (*it);
				}
				else {
					destination = it._Ptr->_Myval;
				}
			}
		}
	}
}

iPoint PathFinding::FindNearestAvailable(Unit* unit) const {

	iPoint pos = App->map->WorldToMap(unit->entityPosition.x, unit->entityPosition.y);
	iPoint ret = pos;
	iPoint adj;

	bool found = false;

	int dist = 0;

	while (!found) {

		dist++;

		for (int i = -dist; i < (dist + 1); i++) {
			for (int j = -dist; j < (dist + 1); j++) {

				adj.create(pos.x + i, pos.y + j);

				if (App->pathfinding->IsWalkable(adj) && !App->entityManager->IsOccupied(adj)) {

					if (unit->path.empty()) {

						ret = adj;
						found = true;
					}
					else if (adj.DistanceManhattan(unit->path.front()) < ret.DistanceManhattan(unit->path.front())) {
						ret = adj;
						found = true;
					}
				}
			}
		}

		if (dist > 5)
			return pos;
	}

	return ret;
}


