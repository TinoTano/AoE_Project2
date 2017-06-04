#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"

#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum Controls {
	SELECT_ALL_UNITS,
	SELECT_ALL_VILLAGERS,
	MOVE_CAMERA,
	HERO_SPECIAL_ATTACK,
	__LAST_CONTROLS
};

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{

public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	void StartInputText(std::string* text, int* cursor);
	void EndInputText();

	string	GetText();
	uint	TextSize();

	SDL_Scancode SelectedKey(std::string s);
	int controls[__LAST_CONTROLS];

private:
	bool		windowEvents[WE_COUNT];
	KeyState*	keyboard;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x = 0;
	int			mouse_motion_y = 0;
	int			mouse_x = 0;
	int			mouse_y = 0;

public:
	//text input management
	string*		input_text = nullptr;
	int*		input_cursor = nullptr;
	string		addtext;
};

#endif // __INPUT_H__