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

	return true;
}

// Update all guis
bool Gui::PreUpdate()
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
		}
	}

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
			delete it._Ptr->_Myval;
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
			if (it._Ptr->_Myval->enabled == true)
				it._Ptr->_Myval->Movement(movement);
		}
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

Label::Label(char* text, int x, int y, _TTF_Font* font) : UIElement(true, x, y, LABEL, nullptr), str(text), font(font) {}

Label::Label(char * text, SDL_Rect area, _TTF_Font* font) : UIElement(true, area.x, area.y, LABEL, nullptr), str(text), font(font) {}

void Label::Update()
{
	Draw();
}

void Label::Draw()
{
	texture = App->font->Print(str.c_str());
	App->font->CalcSize(str.c_str(), width, height);
	SDL_Rect text_size{ 0, 0, width, height };
	App->render->Blit(texture, pos.first, pos.second, &text_size);
}

void Label::SetText(char* text) {
	str = text;
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
}

void InputText::Draw() {
	texture = App->font->Print(str.c_str());
	App->font->CalcSize(str.c_str(), width, height);
	SDL_Rect text_size{ 0, 0, width, height };
	App->render->Blit(texture, pos.first, pos.second, &text_size);
}

MouseState InputText::MouseDetect() {
	MouseState ret = FREE;

	pair<int, int> mouse_pos;
	App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
	if (mouse_pos.first >= pos.first && mouse_pos.first <= (pos.first + area.w) && mouse_pos.second >= pos.second && mouse_pos.second <= (pos.second + area.h)) {
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

	}
}

void ScrollBar::Update()
{
	Up->pos.second;
	switch (model) {
	case MODEL1:
		pair<int, int> motion;
		App->input->GetMouseMotion(motion.first, motion.second);

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
	}
}

int ScrollBar::GetData() {
	data = Thumb->pos.second - Bar->pos.second - 25;
	data *= 2;
	if (data  > 100) data = 100;
	return data;
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

// CURSOR

Cursor::Cursor(SDL_Texture* tex, vector<SDL_Rect> area) : UIElement(true, area[1].x, area[1].y, CURSOR, tex), sprite_list(area) {
	id = 1;
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