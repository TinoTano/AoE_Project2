#include "Console.h"
#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Gui.h"
#include "Input.h"

#define CONSOLE_COLOR_BACKGROUND { 30, 30, 30, 200 }
#define CONSOLE_COLOR_INPUT {100, 0, 200, 200}

Console::Console() : Module()
{
	name = "console";
}

Console::~Console()
{
}

bool Console::Awake(pugi::xml_node &)
{
	return true;
}

bool Console::Start()
{
	App->win->GetWindowSize(window_w, window_h);
	SDL_Rect background_rect{ App->render->camera.x, App->render->camera.y, window_w, window_h/3 };

	cbackground = (Quad*)App->gui->CreateQuad(background_rect, SDL_Color CONSOLE_COLOR_BACKGROUND);
	cbackground->priority = 1;
	console_elements.push_back(cbackground);

	SDL_Rect input_rect{ App->render->camera.x, App->render->camera.y + window_h/3, window_w, 20};
	cinput = (Quad*)App->gui->CreateQuad(input_rect, SDL_Color CONSOLE_COLOR_INPUT);
	cinput->priority = 1;
	console_elements.push_back(cinput);

	ctextinput = (InputText*)App->gui->CreateInput(cinput->area.x, cinput->area.y, input_rect, nullptr);
	ctextinput->priority = 1;
	console_elements.push_back(ctextinput);

	return true;
}

bool Console::PreUpdate()
{
	return true;
}

bool Console::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		if (!show) show = true;
		else show = false;
	}

	if (show) {
		for (list<UIElement*>::iterator it = console_elements.begin(); it != console_elements.end(); ++it) {
			it._Ptr->_Myval->enabled = true;
		}
		
		cbackground->area.x = 0 - App->render->camera.x;
		cbackground->area.y = 0 - App->render->camera.y;
		cinput->area.x = 0 - App->render->camera.x;
		cinput->area.y = 0 - App->render->camera.y + window_h/3;
		ctextinput->SetPos(0 - App->render->camera.x, 0 - App->render->camera.y + window_h/3);

		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			ctext.push_back(ctextinput->str);
		}
	}
	else for (list<UIElement*>::iterator it = console_elements.begin(); it != console_elements.end(); ++it)
		it._Ptr->_Myval->enabled = false;

	return true;
}

bool Console::CleanUp()
{
	return true;
}

bool Console::Save(pugi::xml_node &) const
{
	return true;
}
