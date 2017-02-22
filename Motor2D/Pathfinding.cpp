#include "App.h"
#include "PathFinding.h"
#include "p2Log.h"

PathFinding::PathFinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
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

	last_path.clear();
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
const vector<iPoint>* PathFinding::GetLastPath() const
{
	return &last_path;
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
	uint before = list_to_fill.pathNodeList.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if(_App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(new PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(_App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(new PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(_App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(new PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(_App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(new PathNode(-1, -1, cell, this));

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
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;

	// Nice try :)

	return ret;
}

