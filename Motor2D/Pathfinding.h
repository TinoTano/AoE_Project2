#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "Unit.h"
#include "p2Point.h"

#include <vector>


#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

struct Path;
struct PathNode;

class PathFinding : public Module
{
public:

	PathFinding();

	// Destructor
	virtual ~PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	list<iPoint>* CreatePath(const iPoint& origin, const iPoint& destination);

	// To request all tiles involved in the last generated path
	const list<iPoint>* GetLastPath() const;

	list<iPoint> GetPath() const;
	list<list<iPoint>>* GetPaths();

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	bool Jump(int current_x, int current_y, int dx, int dy, iPoint start, iPoint end, PathNode& new_node);

	iPoint FindNearestAvailableTarget(const iPoint& tile, const iPoint& target) const;
	iPoint FindNearestAvailable(const iPoint& tile, int max_radius = 5, list<iPoint>* cells_to_ignore = nullptr) const;
	void CalculatePath(Path* path);
	void SharePath(Unit* commander, list<Unit*> followers);
	bool DeletePath(list<iPoint>* path_to_delete);

private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;
	// we store the created path here
	list<iPoint> lastPath;
	list<list<iPoint>> paths;
};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	void IdentifySuccessors(PathList& list_to_fill, iPoint startNode, iPoint endNode, PathFinding* path_finder) const;
	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	list<PathNode>::iterator Find(const iPoint& point);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	list<PathNode>::iterator GetNodeLowestScore();

	// -----------
	// The list itself, note they are pointers!
	list<PathNode> pathNodeList;
};


struct Path {
	Path() { completed = false; }
	PathList open;
	PathList closed;
	PathList adjacent;

	iPoint origin;
	iPoint destination;

	list<iPoint> finished_path;

	bool completed;
};

#endif // __PATHFINDING_H__