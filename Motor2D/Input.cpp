#include "Application.h"
#include "Input.h"
#include "Window.h"
#include "p2Log.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

Input::Input() : Module()
{
	name = "input";

	keyboard = new KeyState[NUM_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * NUM_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	for (int i = 0; i < __LAST_CONTROLS; i++) {
		if (i == SELECT_ALL_UNITS) {
			controls[i] = SelectedKey(config.child("shortcuts").attribute("select_units").as_string());
		}
		if (i == SELECT_ALL_VILLAGERS) {
			controls[i] = SelectedKey(config.child("shortcuts").attribute("select_villagers").as_string());
		}
		if (i == HERO_SPECIAL_ATTACK) {
			controls[i] = SelectedKey(config.child("shortcuts").attribute("hero_attack").as_string());
		}
		if (i == MOVE_CAMERA) {
			controls[i] = SelectedKey(config.child("shortcuts").attribute("restart_camera").as_string());
		}
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < NUM_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			/*if (keys[i] == 0)
			{
				if (keyboard[i] != KEY_IDLE)
					keyboard[i] = KEY_UP;
				else
					keyboard[i] = KEY_IDLE;
			}*/
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;
		else {
			if (mouse_buttons[i] == KEY_UP)
				mouse_buttons[i] = KEY_IDLE;
		}

	}

	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			windowEvents[WE_QUIT] = true;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvents[WE_HIDE] = true;
				break;

				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				windowEvents[WE_SHOW] = true;
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[event.button.button - 1] = KEY_DOWN;
			//LOG("Mouse button %d down", event.button.button-1);
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[event.button.button - 1] = KEY_UP;
			//LOG("Mouse button %d up", event.button.button-1);
			break;

		case SDL_MOUSEMOTION: {
			int scale = App->win->GetScale();
			mouse_motion_x = event.motion.xrel / scale;
			mouse_motion_y = event.motion.yrel / scale;
			mouse_x = event.motion.x / scale;
			mouse_y = event.motion.y / scale;
			//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
		}
							  break;

		case SDL_TEXTINPUT:
			addtext += event.text.text;
		}
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

// ---------
bool Input::GetWindowEvent(int code)
{
	return windowEvents[code];
}

void Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

void Input::StartInputText(std::string * text, int * cursor)
{
	input_text = text;
	input_cursor = cursor;
	SDL_StartTextInput();
}

void Input::EndInputText()
{
	SDL_StopTextInput();
	input_cursor = nullptr;
	input_text = nullptr;
}

string Input::GetText()
{
	string ret;
	ret += addtext.c_str();
	addtext.clear();
	return ret;
}

uint Input::TextSize()
{
	return addtext.size();
}

SDL_Scancode Input::SelectedKey(std::string s)
{
	if (s == "Q" || s == "q") {
		return SDL_SCANCODE_Q;
	}
	else if (s == "W" || s == "w") {
		return SDL_SCANCODE_W;
	}
	else if (s == "E" || s == "e") {
		return SDL_SCANCODE_E;
	}
	else if (s == "R" || s == "r") {
		return SDL_SCANCODE_R;
	}
	else if (s == "T" || s == "t") {
		return SDL_SCANCODE_T;
	}
	else if (s == "Y" || s == "y") {
		return SDL_SCANCODE_Y;
	}
	else if (s == "U" || s == "u") {
		return SDL_SCANCODE_U;
	}
	else if (s == "I" || s == "i") {
		return SDL_SCANCODE_I;
	}
	else if (s == "O" || s == "o") {
		return SDL_SCANCODE_O;
	}
	else if (s == "P" || s == "p") {
		return SDL_SCANCODE_P;
	}
	else if (s == "A" || s == "a") {
		return SDL_SCANCODE_A;
	}
	else if (s == "S" || s == "s") {
		return SDL_SCANCODE_S;
	}
	else if (s == "D" || s == "d") {
		return SDL_SCANCODE_D;
	}
	else if (s == "F" || s == "f") {
		return SDL_SCANCODE_F;
	}
	else if (s == "G" || s == "g") {
		return SDL_SCANCODE_G;
	}
	else if (s == "H" || s == "h") {
		return SDL_SCANCODE_H;
	}
	else if (s == "J" || s == "j") {
		return SDL_SCANCODE_J;
	}
	else if (s == "K" || s == "k") {
		return SDL_SCANCODE_K;
	}
	else if (s == "L" || s == "l") {
		return SDL_SCANCODE_L;
	}
	else if (s == "Z" || s == "z") {
		return SDL_SCANCODE_Z;
	}
	else if (s == "X" || s == "x") {
		return SDL_SCANCODE_X;
	}
	else if (s == "C" || s == "c") {
		return SDL_SCANCODE_C;
	}
	else if (s == "V" || s == "v") {
		return SDL_SCANCODE_V;
	}
	else if (s == "B" || s == "b") {
		return SDL_SCANCODE_B;
	}
	else if (s == "N" || s == "n") {
		return SDL_SCANCODE_N;
	}
	else if (s == "M" || s == "m") {
		return SDL_SCANCODE_M;
	}
	else if (s == "tab" || s == "TAB") {
		return SDL_SCANCODE_TAB;
	}
	else if (s == "UP" || s == "up") {
		return SDL_SCANCODE_UP;
	}
	else if (s == "DOWN" || s == "down") {
		return SDL_SCANCODE_DOWN;
	}
	else if (s == "Right" || s == "RIGHT") {
		return SDL_SCANCODE_RIGHT;
	}
	else if (s == "LEFT" || s == "left") {
		return SDL_SCANCODE_LEFT;
	}
	else if (s == "1") {
		return SDL_SCANCODE_1;
	}
	else if (s == "2") {
		return SDL_SCANCODE_2;
	}
	else if (s == "3") {
		return SDL_SCANCODE_3;
	}
	else if (s == "4") {
		return SDL_SCANCODE_4;
	}
	else if (s == "5") {
		return SDL_SCANCODE_5;
	}
	else if (s == "6") {
		return SDL_SCANCODE_6;
	}
	else if (s == "7") {
		return SDL_SCANCODE_7;
	}
	else if (s == "8") {
		return SDL_SCANCODE_8;
	}
	else if (s == "9") {
		return SDL_SCANCODE_9;
	}
	else if (s == "SPACE" || s == "space") {
		return SDL_SCANCODE_SPACE;
	}
	else if (s == "RSHIFT" || s == "rshift") {
		return SDL_SCANCODE_RSHIFT;
	}
	else if (s == "LSHIFT" || s == "lshift") {
		return SDL_SCANCODE_LSHIFT;
	}
	else if (s == "RCTRL" || s == "rctrl") {
		return SDL_SCANCODE_RCTRL;
	}
	else if (s == "LCTRL" || s == "lctrl") {
		return SDL_SCANCODE_LCTRL;
	}
	else {
		return SDL_SCANCODE_0;
	}
}