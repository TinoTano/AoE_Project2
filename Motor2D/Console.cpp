#include "Console.h"
#include "Application.h"
#include "Render.h"
#include "Window.h"
#include "Gui.h"
#include "Input.h"

#define CONSOLE_COLOR_BACKGROUND { 30, 30, 30, 240 }
#define CONSOLE_COLOR_INPUT {100, 0, 200, 200}
#define X_OFFSET 10
#define Y_OFFSET 20
#define SCROLL_OFFSET 80

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

	SDL_Rect input_rect{ App->render->camera.x, App->render->camera.y + window_h/3, window_w, Y_OFFSET};
	cinput = (Quad*)App->gui->CreateQuad(input_rect, SDL_Color CONSOLE_COLOR_INPUT);
	cinput->priority = 1;
	console_elements.push_back(cinput);

	ctextinput = (InputText*)App->gui->CreateInput(cinput->area.x, cinput->area.y, input_rect, nullptr);
	ctextinput->priority = 1;
	console_elements.push_back(ctextinput);

	cscroll = (ScrollBar*)App->gui->CreateScrollBar(window_w - 20, window_h, MODEL2);
	cscroll->priority = 1;
	console_elements.push_back(cscroll);
	SDL_Rect BlitQuare{ App->render->camera.x + X_OFFSET, App->render->camera.y + Y_OFFSET, window_w - SCROLL_OFFSET, window_h/3 - 30};
	SDL_Rect BarSquare{ App->render->camera.x + window_w - SCROLL_OFFSET + X_OFFSET, App->render->camera.y + Y_OFFSET, 20, window_h / 3 - 30};
	cscroll->SetBar(BlitQuare,BarSquare);

	cscroll->ABar.SmallSquare->priority = 1;
	cscroll->BarRect->priority = 1;
	cscroll->BlitSquare->priority = 1;

	console_elements.push_back(cscroll->ABar.SmallSquare);
	console_elements.push_back(cscroll->BarRect);
	console_elements.push_back(cscroll->BlitSquare);
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

		//cscroll->ABar.SmallSquare->area.y = cscroll->BarRect->area.y + cscroll->BarRect->area.h - cscroll->ABar.SmallSquare->area.h;
	}

	if (show) {
		for (list<UIElement*>::iterator it = console_elements.begin(); it != console_elements.end(); ++it) {
			it._Ptr->_Myval->enabled = true;
		}
		cinput->current = CLICKIN;
		MoveEverything();

		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			h += 10;
			cscroll->UpdateThumbSize(h);
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

void Console::MoveEverything() {
	cbackground->area.x = 0 - App->render->camera.x;
	cbackground->area.y = 0 - App->render->camera.y;
	cinput->area.x = 0 - App->render->camera.x;
	cinput->area.y = 0 - App->render->camera.y + window_h / 3;
	ctextinput->area.x = 0 - App->render->camera.x;
	ctextinput->area.y = 0 - App->render->camera.y + window_h / 3;

	ctextinput->SetPos(0 - App->render->camera.x, 0 - App->render->camera.y + window_h / 3);

	cscroll->ABar.BigSquare.x = 0 - App->render->camera.x + X_OFFSET;
	cscroll->ABar.BigSquare.y = 0 - App->render->camera.y + Y_OFFSET;
	
	cscroll->BlitSquare->area.x = 0 - App->render->camera.x + X_OFFSET;
	cscroll->BlitSquare->area.y = 0 - App->render->camera.y + Y_OFFSET;

	cscroll->BarRect->area.x = 0 - App->render->camera.x + window_w - SCROLL_OFFSET + X_OFFSET;
	cscroll->BarRect->area.y = 0 - App->render->camera.y + Y_OFFSET;

	cscroll->ABar.SmallSquare->area.x = 0 - App->render->camera.x + window_w - SCROLL_OFFSET + X_OFFSET;
}

