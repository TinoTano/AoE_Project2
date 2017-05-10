#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "PugiXml\src\pugixml.hpp"
#include "p2Defs.h"
#include "Brofiler\Brofiler.h"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class FileSystem;
class SceneManager;
class Map;
class PathFinding;
class Fonts;
class ParticleManager;
class EntityManager;
class Collision;
class Gui;
class FogOfWar;
class Minimap;
class QuestManager;
class CutSceneManager;

class Application
{
public:

	// Constructor
	Application(int argc, char* args[]);

	// Destructor
	virtual ~Application();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	void GetSaveGames(list<string>& list_to_fill) const;

	pugi::xml_node LoadGameDataFile(pugi::xml_document&) const;
	pugi::xml_node LoadHUDDataFile(pugi::xml_document&) const;
	pugi::xml_node LoadParticleDataFile(pugi::xml_document &) const;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Modules
	Window*				win = NULL;
	Input*				input = NULL;
	Render*				render = NULL;
	Textures*			tex = NULL;
	Audio*				audio = NULL;
	SceneManager*		sceneManager = NULL;
	FileSystem*			fs = NULL;
	Map*				map = NULL;
	PathFinding*		pathfinding = NULL;
	Fonts*				font = NULL;
	ParticleManager*	particlemanager = NULL;
	EntityManager*		entityManager = NULL;
	Collision*			collision = NULL;
	Gui*				gui = NULL;
	FogOfWar*			fog = NULL;
	Minimap*			minimap = NULL;
	QuestManager*		quest = NULL;
	CutSceneManager*	cutscene = NULL;

	bool				quit = false;

private:

	list<Module*>		modules;
	int					argc;
	char**				args;

	string				title;
	string				organization;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	string				load_game;
	mutable string		save_game;

	PerfTimer			ptimer;
	uint64				frame_count = 0;
	Timer				startup_time;
	Timer				frame_time;
	Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;
};

extern Application* App; 

#endif