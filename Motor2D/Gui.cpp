#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Window.h"
#include "Gui.h"

Gui::Gui() : Module()
{
	name = "gui";
}

// Destructor
Gui::~Gui()
{
	// Ignores last item of the list CHECK LATER
	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		Elements.erase(it);
	}

}

// Called before render is available
bool Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;
	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool Gui::Start()
{

	atlas = App->tex->Load(atlas_file_name.c_str());

	vector<SDL_Rect> sprites_cursor;
	sprites_cursor.push_back({ 0,   0, 70, 50 });
	sprites_cursor.push_back({ 70,   0, 70, 50 });
	sprites_cursor.push_back({ 140, 0, 70, 50 });
	sprites_cursor.push_back({ 210, 0, 70, 50 });
	sprites_cursor.push_back({ 280, 0, 70, 50 });
	sprites_cursor.push_back({ 350, 0, 70, 50 });
	sprites_cursor.push_back({ 420, 0, 70, 50 });
	sprites_cursor.push_back({ 490, 0, 70, 50 });
	sprites_cursor.push_back({ 560, 0, 70, 50 });
	sprites_cursor.push_back({ 0,   50, 70, 50 });
	sprites_cursor.push_back({ 70,  50, 70, 50 });
	sprites_cursor.push_back({ 140, 50, 70, 50 });
	sprites_cursor.push_back({ 210, 50, 70, 50 });
	sprites_cursor.push_back({ 280, 50, 70, 50 });
	sprites_cursor.push_back({ 350, 50, 70, 50 });
	sprites_cursor.push_back({ 420, 50, 70, 50 });
	sprites_cursor.push_back({ 490, 50, 70, 50 });
	sprites_cursor.push_back({ 560, 50, 70, 50 });

	App->gui->cursor = (Cursor*)CreateCursor("gui/cursor.png", sprites_cursor);

	LoadHUDData();

	return true;
}

// Update all guis
bool Gui::PreUpdate()
{
	return true;
}
bool Gui::Update(float dt)
{
	return true;
}


// Called after all Updates
bool Gui::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
		{
			if (!it._Ptr->_Myval->debug) it._Ptr->_Myval->debug = true;
			else it._Ptr->_Myval->debug = false;
		}
	}

	if (Elements.empty() != true)
	{
		for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
		{
			if (it._Ptr->_Myval->enabled == true)
				it._Ptr->_Myval->Update();
			else if (it._Ptr->_Myval->type == BUTTON && it._Ptr->_Myval->current != FREE) it._Ptr->_Myval->current = FREE;
		}
	}

	cursor->Update();
	hud.Update();

	return true;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");

	if (Elements.empty() != true)
	{
		for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
		{
			RELEASE((*it));
		}
	}

	return true;
}

// const getter for atlas
SDL_Texture* Gui::GetAtlas() const
{
	return atlas;
}

bool Gui::Save(pugi::xml_node &) const
{
	return true;
}

bool Gui::Load(pugi::xml_node &)
{
	return true;
}

void Gui::ScreenMoves(pair<int,int> movement) {

	if (Elements.empty() != true)
	{
		for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
		{
				it._Ptr->_Myval->Movement(movement);
		}
	}
}
void Gui::SetPriority()
{
	list<UIElement*> Priority1;
	list<UIElement*> Priority0;
	int i = 0;
	for (list<UIElement*>::iterator it = Elements.begin(); i < Elements.size(); ++it)
	{
		switch (it._Ptr->_Myval->priority) {
		case 0:
			Priority0.push_back(it._Ptr->_Myval);
			break;
		case 1:
			Priority1.push_back(it._Ptr->_Myval);
			break;
		}
		++i;
	}

	Priority1.merge(Priority0);
	Priority1.unique();
	Elements = Priority1;
}

void Gui::Focus(SDL_Rect rect)
{
	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		if (it._Ptr->_Myval->pos.first < rect.x || it._Ptr->_Myval->pos.first > rect.x + rect.w ||
			it._Ptr->_Myval->pos.second < rect.y || it._Ptr->_Myval->pos.second > rect.y + rect.h)
			it._Ptr->_Myval->focused = false;
	}
}

void Gui::Unfocus()
{
	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		it._Ptr->_Myval->focused = true;
	}
}
// UI ELEMENT
// methods:

void UIElement::SetPos(int x, int y)
{
	pos.first = x;
	pos.second = y;
}

void UIElement::SetParentPos(int x, int y) {
	parent_pos.first = x;
	parent_pos.second = y;
}
void UIElement::Move(int x, int y) {
	pos.first += x;
	pos.second += y;
}

UIElement::UIElement(bool argenabled, int argx, int argy, ElementType argtype, SDL_Texture * argtexture) : enabled(argenabled), pos(argx, argy), type(argtype), texture(argtexture) {
	debug_color = DEBUG_COLOR;
	current = FREE;
}

UIElement * Gui::CreateImage(char* path, int x, int y, SDL_Rect section)
{
	UIElement* ret = nullptr;
	SDL_Texture* tex;
	if (path != nullptr)
		tex = App->tex->Load(path);
	else tex = GetAtlas();

	ret = new Image(section, x, y, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateImage(char* path, int x, int y)
{
	UIElement* ret = nullptr;
	SDL_Texture* tex;
	if (path != nullptr)
		tex = App->tex->Load(path);
	else tex = GetAtlas();

	ret = new Image(x, y, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateLabel(char * text, int x, int y, _TTF_Font * font)
{
	UIElement* ret = new Label(text, x, y, font);
	Elements.push_back(ret);
	return ret;
}

UIElement* Gui::CreateLabel(string text, int x, int y, _TTF_Font* font) {
	UIElement* ret = new Label(text, x, y, font);
	Elements.push_back(ret);
	return ret;
}

UIElement * Gui::CreateLabel(char * text, SDL_Rect area, _TTF_Font * font)
{
	UIElement* ret = new Label(text, area, font);
	Elements.push_back(ret);
	return ret;
}

UIElement * Gui::CreateInput(int x, int y, SDL_Rect detect_section, _TTF_Font * font)
{
	UIElement* ret = new InputText(x, y, detect_section, font);
	Elements.push_back(ret);
	return ret;
}

UIElement * Gui::CreateButton(char* path, int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier Tier)
{
	UIElement* ret = nullptr;
	SDL_Texture* tex;
	if (path != nullptr)
		tex = App->tex->Load(path);
	else tex = GetAtlas();

	ret = new Button(x, y, blit_sections, detect_sections, Tier, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateScrollBar(int x, int y, ScrollBarModel model)
{
	UIElement* ret = nullptr;
	ret = new ScrollBar(x, y, model);
	Elements.push_back(ret);
	return ret;
}

UIElement * Gui::CreateQuad(SDL_Rect size, SDL_Color color)
{
	UIElement* ret = nullptr;
	ret = new Quad(size, color);
	Elements.push_back(ret);
	return ret;
}

UIElement * Gui::CreateCursor(char* path, vector<SDL_Rect> cursor_list)
{
	UIElement* ret = nullptr;
	SDL_Texture * tex = App->tex->Load(path);
	ret = new Cursor(tex, cursor_list);
	Elements.push_back(ret);
	return ret;
}

void Gui::DestroyUIElement(UIElement* element)
{

	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		if (element == it._Ptr->_Myval)
		{
			int a = Elements.size();
			Elements.remove(it._Ptr->_Myval);
			int b = Elements.size();
		//	it._Ptr->_Prev->_Next->_Myval = it._Ptr->_Next->_Myval;
			RELEASE((element));
		}
	}

}

// IMAGE
Image::Image(SDL_Rect argsection, int x, int y, SDL_Texture* argtexture) : UIElement(true, x, y, IMAGE, argtexture), section(argsection) {}
Image::Image(int x, int y, SDL_Texture* argtexture) : UIElement(true, x, y, IMAGE, argtexture) {}


void Image::Update()
{
	Draw();
	if (debug) DebugMode();
}

void Image::Draw()
{
	if (section.w>0 && section.h>0)
		App->render->Blit(texture, pos.first, pos.second, &section);
	else App->render->Blit(texture, pos.first, pos.second);
}

MouseState Image::MouseDetect()
{
	MouseState ret = FREE;
	if (focused)
	{
		pair<int, int> mouse_pos;
		App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
		if (mouse_pos.first >= pos.first && mouse_pos.first <= (pos.first + section.w) && mouse_pos.second >= pos.second && mouse_pos.second <= (pos.second + section.h)) {
			ret = HOVER;
		}
		if (ret == FREE && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			ret = CLICKOUT;
		}
		else {
			if (ret == HOVER && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				ret = CLICKIN;

			}
		}
	}
	return ret;
}

void Image::Movement(pair<int, int> movement) {
	pos.first -= movement.first;
	pos.second -= movement.second;
}

void Image::DebugMode() {
	App->render->DrawQuad({ pos.first, pos.second, section.w, section.h }, debug_color.r, debug_color.g, debug_color.b, debug_color.a, true);
}
// LABEL 

Label::Label(char* text, int x, int y, _TTF_Font* font) : UIElement(true, x, y, LABEL, nullptr), str(text), font(font) {
	color = { 0,0,0,250 };
	texture = App->font->Print(str.c_str(), color);
	App->font->CalcSize(str.c_str(), width, height);
}

Label::Label(string text, int x, int y, _TTF_Font* font) : UIElement(true, x, y, LABEL, nullptr), str(text), font(font) {
	color = { 0,0,0,250 };
	texture = App->font->Print(str.c_str(), color);
	App->font->CalcSize(str.c_str(), width, height);
}

Label::Label(char * text, SDL_Rect area, _TTF_Font* font) : UIElement(true, area.x, area.y, LABEL, nullptr), str(text), font(font) {
	color = { 0,0,0,250 };
	texture = App->font->Print(str.c_str(), color);
	App->font->CalcSize(str.c_str(), width, height);
	
}

void Label::Update()
{
	Draw();
}

void Label::Draw()
{
	SDL_Rect text_size{ 0, 0, width, height };
	App->render->Blit(texture, pos.first, pos.second, &text_size);
}

void Label::SetText(char* text) {
	str = text;
	texture = App->font->Print(str.c_str());
	App->font->CalcSize(str.c_str(), width, height);
}

void Label::SetSize(int size) {
	font = App->font->Load(nullptr, size);
	this->size = size;
	texture = App->font->Print(str.c_str(), color, font);
	App->font->CalcSize(str.c_str(), width, height, font);
}

void Label::Movement(pair<int, int> movement) {
	pos.first -= movement.first;
	pos.second -= movement.second;
}

//BUTTON 

Button::Button(int x, int y, vector<SDL_Rect>blit_sections, vector<SDL_Rect>detect_sections, ButtonTier Tier, SDL_Texture* argtexture) : UIElement(true, x, y, BUTTON, argtexture), button_tier(Tier) {
	this->blit_sections = blit_sections;
	this->detect_sections = detect_sections;

}

void Button::Update()
{
	if (!App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		current = MouseDetect();

	switch (button_tier) {
	case TIER1:
		if (current == HOVER) current = MouseDetect();
		if (current == HOVER) Draw(blit_sections[1]);
		else if (current == CLICKIN)
			Draw(blit_sections[2]);
		else Draw(blit_sections[0]);
		break;
	case TIER2:
		if (current == HOVER) current = MouseDetect();
		if (current == CLICKIN)
			Draw(blit_sections[1]);
		else Draw(blit_sections[0]);
		break;
	}

	if (debug) DebugMode();
}

void Button::Draw(SDL_Rect section)
{
	App->render->Blit(texture, pos.first, pos.second, &section);
}

void Button::Movement(pair<int, int> movement) {
	pos.first -= movement.first;
	pos.second -= movement.second;

	for (int i = 0; i < detect_sections.size(); ++i) {
		detect_sections[i].x -= movement.first;
		detect_sections[i].y = movement.second;
	}
}

MouseState Button::MouseDetect()
{
	MouseState ret = FREE;
	if (focused) {
		for (int it = 0; it < detect_sections.size(); ++it) {
			pair<int, int> mouse_pos;
			App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
			if (mouse_pos.first - App->render->camera.x >= pos.first && mouse_pos.first - App->render->camera.x <= (pos.first + detect_sections[it].w) && mouse_pos.second - App->render->camera.y >= pos.second && mouse_pos.second - App->render->camera.y <= (pos.second + detect_sections[it].h)) {
				ret = HOVER;
				break;
			}
		}
		if (ret == FREE && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			ret = CLICKOUT;
		}
		else {
			if (ret == HOVER && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				ret = CLICKIN;

			}
		}
	}
	return ret;
}
void Button::DebugMode() {
	for (int it = 0; it < detect_sections.size(); ++it) {
		SDL_Rect debug_rect{ pos.first, pos.second, detect_sections[it].w, detect_sections[it].h };
		App->render->DrawQuad(debug_rect, debug_color.r, debug_color.g, debug_color.b, debug_color.a, true);

	}
}


// INPUT

InputText::InputText(int x, int y, SDL_Rect detect_area, _TTF_Font* font) : UIElement(true, x, y, INPUTTEXT, nullptr), font(font), area(detect_area) {
	editable = false;
	str = "write here";
	bar.w = 2;
	bar.h = 32;
	UpdateWordsLength();
	SetBarToEnd();
}

void InputText::Update() {
	current = MouseDetect();

	if (current == CLICKIN)
	{
		editable = true;
		SetBarToEnd();
	}
	else if (current == CLICKOUT)
		editable = false;

	SDL_StartTextInput();
	if (editable) {
		if (str.size() >= MAX_CHAR) App->input->GetText();
		if (App->input->TextSize() > 0) {
			str.insert(bar_pos, App->input->GetText());
			bar_pos++;
		}
		if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
			if (str.length() > 0 && bar_pos > 0) {
				str.erase(--bar_pos, 1);
			}
			UpdateWordsLength();
		}
		UpdateWordsLength();
		MoveCursor();
		DrawBar();
	}
	else SDL_StopTextInput();

	Draw();
	if (debug) DebugMode();
}

void InputText::Draw() {
	texture = App->font->Print(str.c_str());
	App->font->CalcSize(str.c_str(), width, height);
	SDL_Rect text_size{ 0, 0, width, height };
	App->render->Blit(texture, pos.first, pos.second, &text_size);
}

MouseState InputText::MouseDetect() {
	MouseState ret = FREE;
	if (focused)
	{
		pair<int, int> mouse_pos;
		App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
		if (mouse_pos.first - App->render->camera.x >= pos.first && mouse_pos.first - App->render->camera.x <= (pos.first + area.w) && mouse_pos.second - App->render->camera.y >= pos.second && mouse_pos.second - App->render->camera.y <= (pos.second + area.h)) {
			ret = HOVER;
		}
		if (ret == FREE && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			ret = CLICKOUT;
		}
		else {
			if (ret == HOVER && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				ret = CLICKIN;
			}
		}
	}
	return ret;
}

void InputText::MoveCursor()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (bar_pos > 0)
			bar_pos--;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (bar_pos < str.size())
			bar_pos++;
	}

	if (bar_pos != 0)
	{
		int i = 0;
		for (list<int>::iterator it = words_lenght.begin(); it != words_lenght.end(); it++, i++)
		{
			if (i == bar_pos - 1)
			{
				bar_pos_x = *it;
				break;
			}
		}
	}
	else
		bar_pos_x = 0;
}

void InputText::UpdateWordsLength()
{
	words_lenght.clear();

	int acumulated = 0;
	for (uint i = 0; i < str.size(); i++)
	{
		string word; word = str[i];
		int x = 0, y = 0;
		App->font->CalcSize(word.c_str(), x, y, font);
		if ((strcmp(word.c_str(), "f") == 0) || (strcmp(word.c_str(), "j") == 0))
			x--;

		acumulated += x;
		words_lenght.push_back(acumulated);
	}
}

void InputText::DrawBar()
{
	bar.x = pos.first + bar_pos_x;
	bar.y = pos.second;

	App->render->DrawQuad(bar, 255, 255, 255, 100, true);
}

void InputText::SetBarToEnd() {
	bar_pos = words_lenght.size();

	if (bar_pos != 0)
	{
		int i = 0;
		for (list<int>::iterator it = words_lenght.begin(); it != words_lenght.end(); it++, i++)
		{
			if (i = bar_pos - 1)
				bar_pos_x = *it;
		}
	}
}

void InputText::DebugMode() {
	App->render->DrawQuad(area, 0, 255, 255, 255, true);
}
// ScrollBar

ScrollBar::ScrollBar(int x, int y, ScrollBarModel model) : UIElement(true, x, y, SCROLLBAR, nullptr), model(model)
{
	switch (model) {
	case MODEL1:

		int xs, ys;
		xs = 0; ys = 0;

		Bar = (Image*)App->gui->CreateImage("gui/bar.png", x + xs, y + ys);
		Bar->SetParentPos(xs, ys);
		Bar->parent = this;

		xs = 3; ys = 25;

		Thumb = (Image*)App->gui->CreateImage("gui/thumb.png", x + xs, y + ys);
		Thumb->SetParentPos(xs, ys);
		Thumb->parent = this;
		Thumb->section.w = 25;
		Thumb->section.h = 23;

		xs = 2; ys = 2;

		vector<SDL_Rect> sections_b;
		sections_b.push_back({ 0,0, 25, 23 });
		sections_b.push_back({ 0,23, 25, 23 });
		vector<SDL_Rect> sections_d;
		sections_d.push_back({ x + xs, y + ys, 25, 23 });

		Up = (Button*)App->gui->CreateButton("gui/scroll_button1.png", x + xs, y + ys, sections_b, sections_d, TIER2);
		min_y = y + ys + 23;

		xs = 2; ys = 100;

		Down = (Button*)App->gui->CreateButton("gui/scroll_button2.png", x + xs, y + ys, sections_b, sections_d, TIER2);
		max_y = y + ys;
		break;
	}
}

void ScrollBar::Update()
{
	Up->pos.second;
		pair<int, int> motion;
		App->input->GetMouseMotion(motion.first, motion.second);

	switch (model) {
	case MODEL1:

		if (!App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			current = Thumb->MouseDetect();

		if (current == HOVER)
			current = Thumb->MouseDetect();

		else if (current == CLICKIN && Thumb->pos.second >= Up->pos.second && Thumb->pos.second <= Down->pos.second - Thumb->section.h) {
			Thumb->Move(0, motion.second);
			if (Thumb->pos.second < Up->pos.second + 23) Thumb->pos.second = Up->pos.second + 23;
			if (Thumb->pos.second > Down->pos.second - Thumb->section.h) Thumb->pos.second = Down->pos.second - Thumb->section.h;
		}
		if (Up->current == CLICKIN) {
			Thumb->Move(0, -3);
			Thumb->Move(0, motion.second);
			if (Thumb->pos.second < Up->pos.second + 23) Thumb->pos.second = Up->pos.second + 23;
		}
		if (Down->current == CLICKIN) {
			Thumb->Move(0, 3);
			Thumb->Move(0, motion.second);
			if (Thumb->pos.second > Down->pos.second - Thumb->section.h) Thumb->pos.second = Down->pos.second - Thumb->section.h;
		}
		break;
	case MODEL2:
		if (!App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			current = MouseDetect();
		if (current == HOVER)
			current = MouseDetect();
		else if (current == CLICKIN && ABar.SmallSquare->area.y >= BarRect->area.y && ABar.SmallSquare->area.y + ABar.SmallSquare->area.h <= BarRect->area.y + BarRect->area.h) {
			ABar.SmallSquare->area.y += motion.second;
			if (ABar.SmallSquare->area.y <= BarRect->area.y) ABar.SmallSquare->area.y = BarRect->area.y;
			if (ABar.SmallSquare->area.y + ABar.SmallSquare->area.h >= BarRect->area.y + BarRect->area.h) ABar.SmallSquare->area.y = BarRect->area.y + BarRect->area.h - ABar.SmallSquare->area.h;
		}
	}

	if (debug) DebugMode();
}

int ScrollBar::GetData() {
	data = Thumb->pos.second - Bar->pos.second - 25;
	data *= 2;
	if (data  > 100) data = 100;
	return data;
}

MouseState ScrollBar::MouseDetect() {
	MouseState ret = FREE;
	switch (model) {
	case MODEL2:
		pair<int, int> mouse_pos;
		App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
		if (mouse_pos.first - App->render->camera.x >= ABar.SmallSquare->area.x && mouse_pos.first - App->render->camera.x <= (ABar.SmallSquare->area.x + ABar.SmallSquare->area.w) && mouse_pos.second - App->render->camera.y >= ABar.SmallSquare->area.y && mouse_pos.second - App->render->camera.y <= (ABar.SmallSquare->area.y + ABar.SmallSquare->area.h)) {
			ret = HOVER;
		}
		if (ret == FREE && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			ret = CLICKOUT;
		}
		else {
			if (ret == HOVER && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				ret = CLICKIN;
			}
		}
	}
	return ret;
}

void ScrollBar::SetBar(SDL_Rect BlitSquare, SDL_Rect BarIt) {
	this->BlitSquare = (Quad*)App->gui->CreateQuad(BlitSquare, BAR_COLOR);
	BarRect = (Quad*)App->gui->CreateQuad(BarIt, BAR_COLOR);
	ABar.SmallSquare = (Quad*)App->gui->CreateQuad(BarIt, THUMB_COLOR);
	ABar.BigSquare = BlitSquare; //FOR NOW
}

void ScrollBar::UpdateThumbSize(int h) {
	if (h > ABar.BigSquare.h) {
		ABar.BigSquare.h = h;
		ABar.SmallSquare->area.h = BlitSquare->area.h  * BarRect->area.h / ABar.BigSquare.h;
	}
}
void ScrollBar::DebugMode() {
	switch (model) {
	case MODEL2:
		App->render->DrawQuad(ABar.BigSquare, 255, 255, 255, 255, true);
	}
}

// QUAD

Quad::Quad(SDL_Rect area, SDL_Color color) : UIElement(true, area.x, area.y, QUAD, nullptr), color(color), area(area) {
}
void Quad::Update() {
	Draw();

}
void Quad::Draw() {

	App->render->DrawQuad(area, color.r, color.g, color.b, color.a, true);

}
void Quad::SetArea(SDL_Rect area) {
	this->area = area;
	SetPos(area.x, area.y);
}
void Quad::Movement(pair<int, int> movement) {
	area.x -= movement.first;
	area.y -= movement.second;
}

// CURSOR

Cursor::Cursor(SDL_Texture* tex, vector<SDL_Rect> area) : UIElement(true, area[1].x, area[1].y, CURSOR, tex), sprite_list(area) {
	id = 0;
}

void Cursor::Update() {
	if (texture != nullptr) {
		SDL_ShowCursor(false);
		App->input->GetMousePosition(cursor_pos.first, cursor_pos.second);
		pos.first = cursor_pos.first - App->render->camera.x;
		pos.second = cursor_pos.second - App->render->camera.y;
		Draw();
		if (id == 1 || id == 2 || id == 5 || id == 7 || id == 8 || id == 12 || id == 16) blitoffset.second = 20;
	}
}
void Cursor::Draw() {
	
	App->render->Blit(texture, pos.first - blitoffset.first , pos.second - blitoffset.second, &sprite_list[id]);
}
void Cursor::SetCursor(int id) {
	this->id = id;
}

// WINDOW
void WindowUI::WindowOn()
{
	for (list<UIElement*>::iterator it = in_window.begin(); it != in_window.end(); ++it)
	{
		it._Ptr->_Myval->enabled = true;
	}
	enabled = true;
}

void WindowUI::WindowOff()
{
	for (list<UIElement*>::iterator it = in_window.begin(); it != in_window.end(); ++it)
	{
		it._Ptr->_Myval->enabled = false;
	}
	enabled = false;
}

SDL_Rect WindowUI::FocusArea()
{
	SDL_Rect ret;
	ret.x = *x;
	ret.y = *y;
	ret.w = width;
	ret.h = height;
	return ret;
}
void WindowUI::SetFocus(int& x, int& y, int width, int height)
{
	this->x = &x;
	this->y = &y;
	this->width = width;
	this->height = height;
}
bool WindowUI::IsEnabled() {
	return enabled;
}

bool HUD::IsEnabled()
{
	return enabled;
}


//HUD

void HUD::Update() {
	switch (App->entityManager->selectedUnitList.size()) {
	case 0: 
		if (type != NONE)
		{
			ClearSingle();
			type = NONE;
		}
		break;
	case 1:
		if (type != SINGLEINFO)
		{
			type = SINGLEINFO;
			GetSelection();
		}
		else {
			for (list<UnitSprite>::iterator it = App->gui->SpriteRects.begin(); it != App->gui->SpriteRects.end(); ++it)
			{
				if (it._Ptr->_Myval.GetID() == App->entityManager->selectedUnitList.front()->GetType())
				{
					name->str = it._Ptr->_Myval.GetName();
				}
			}
		}
		break;
	default:
		if (type != MULTIPLESELECTION)
		{
			type = MULTIPLESELECTION;
		}
		break;
	}

}

void HUD::ClearSingle()
{
	App->gui->DestroyUIElement(single);
	App->gui->DestroyUIElement(name);
}


void HUD::GetSelection() {

	switch (type) {
	case NONE:
		break;
	case SINGLEINFO:
		for (list<UnitSprite>::iterator it = App->gui->SpriteRects.begin(); it != App->gui->SpriteRects.end(); ++it)
		{
			if (it._Ptr->_Myval.GetID() == App->entityManager->selectedUnitList.front()->GetType())
			{
				single = (Image*)App->gui->CreateImage("gui/EntityMiniatures.png", 310 - App->render->camera.x, 670 - App->render->camera.y, it._Ptr->_Myval.GetRect());
				name = (Label*)App->gui->CreateLabel(it._Ptr->_Myval.GetName(), 310 - App->render->camera.x, 650 - App->render->camera.y, nullptr);
			}
		}
	}
	// X = 500 Y = 650
}

bool Gui::LoadHUDData()
{
	bool ret = false;
	pugi::xml_document HUDDataFile;
	pugi::xml_node HUDData;
	pugi::xml_node unitNodeInfo;
	pugi::xml_node buildingNodeInfo;
	pugi::xml_node resourceNodeInfo;

	HUDData = App->LoadHUDDataFile(HUDDataFile);

	if (HUDData.empty() == false)
	{
		SDL_Rect proportions;
		proportions.w = HUDData.child("Sprites").child("Proportions").attribute("width").as_uint();
		proportions.h = HUDData.child("Sprites").child("Proportions").attribute("height").as_uint();

		for (unitNodeInfo = HUDData.child("Units").child("Unit"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Unit")) {
			EntityType type = UNIT;
			string name(unitNodeInfo.child("Name").attribute("value").as_string());

			int id = unitNodeInfo.child("ID").attribute("value").as_int();
			proportions.x = unitNodeInfo.child("Position").attribute("x").as_int();
			proportions.y = unitNodeInfo.child("Position").attribute("y").as_int();

			UnitSprite unit(type, proportions, id, name);
			SpriteRects.push_back(unit);
		}
	}
		/*
		for (unitNodeInfo = HUDData.child("Buildings").child("Building"); unitNodeInfo; unitNodeInfo = unitNodeInfo.next_sibling("Building")) {
			EntityType type = BUILDING;

			int id = unitNodeInfo.child("ID").attribute("value").as_int();
			proportions.x = unitNodeInfo.child("Position").attribute("x").as_int();
			proportions.y = unitNodeInfo.child("Position").attribute("y").as_int();

			UnitSprite unit(type, proportions, id);
			SpriteRects.push_back(unit);
		}
	}*/
	return ret;
}