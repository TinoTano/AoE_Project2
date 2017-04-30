#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Window.h"
#include "Gui.h"
#include "SceneManager.h"
#include <stdlib.h>  

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

	return ret;
}

// Called before the first frame
bool Gui::Start()
{
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

	


	hud = new HUD();
	App->gui->cursor = (Cursor*)CreateCursor("gui/cursor.png", sprites_cursor);


	LoadHUDData();

	for (uint i = 0; i < info.size(); ++i)
	{
		info[i].texture = App->tex->Load(info[i].path.c_str());
	}

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

	if (App->sceneManager->current_scene->name == "scene")
	{
		hud->Update();
	}
	// CURSOR ALWAYS GOES LAST!!!!!
	cursor->Update();
	return true;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");

	cursor->CleanUp();

	for (uint i = 0; i < info.size(); ++i)
	{
		App->tex->UnLoad(info[i].texture);
	}
	
	if (Elements.empty() != true)
	{
		for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
		{
			App->tex->UnLoad(it._Ptr->_Myval->texture);
			App->gui->DestroyUIElement(it._Ptr->_Myval);
		}
	}
	Elements.clear();
	delete cursor;
	hud->CleanUp();
	delete hud;
	return true;
}


bool Gui::Save(pugi::xml_node &) const
{
	return true;
}

bool Gui::Load(pugi::xml_node &)
{
	return true;
}

void Gui::ScreenMoves(pair<int, int> movement) {

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
vector<Info> Gui::GetElements(string scene)
{
	vector<Info> ret;

	for (uint i = 0; i < info.size(); ++i)
	{
		if (info[i].scene == scene) {
			ret.push_back(info[i]);
		}
	}
	return ret;
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
		tex = App->tex->Load(path);

	ret = new Image(section, x, y, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateImage(char* path, int x, int y)
{
	UIElement* ret = nullptr;
	SDL_Texture* tex;
		tex = App->tex->Load(path);

	ret = new Image(x, y, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateImage(SDL_Texture * argtexture, int x, int y, SDL_Rect section)
{
	UIElement* ret = nullptr;
	ret = new Image(section, x, y,argtexture);
	Elements.push_back(ret);
	return nullptr;
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
	tex = App->tex->Load(path);

	ret = new Button(x, y, blit_sections, detect_sections, Tier, tex);
	Elements.push_back(ret);

	return ret;
}

UIElement * Gui::CreateButton(SDL_Texture * texture, int x, int y, vector<SDL_Rect> blit_sections, vector<SDL_Rect> detect_sections, ButtonTier Tier)
{
	UIElement* ret = new Button(x, y, blit_sections, detect_sections, Tier, texture);
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
	return ret;
}

void Gui::DestroyUIElement(UIElement* element)
{

	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		if (element == it._Ptr->_Myval)
		{
			it._Ptr->_Myval->CleanUp();
			Elements.remove(it._Ptr->_Myval);
			RELEASE((element));
		}
	}

}

void Gui::DestroyALLUIElements() {

	for (list<UIElement*>::iterator it = Elements.begin(); it != Elements.end(); ++it)
	{
		it._Ptr->_Myval->CleanUp();
		Elements.remove(it._Ptr->_Myval);
		RELEASE((it._Ptr->_Myval));
	}
}


// IMAGE
Image::Image(SDL_Rect argsection, int x, int y, SDL_Texture* argtexture) : UIElement(true, x, y, IMAGE, argtexture), section(argsection) {}
Image::Image(int x, int y, SDL_Texture* argtexture) : UIElement(true, x, y, IMAGE, argtexture) {}


void Image::Update()
{
	Draw();
	if (!App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		current = MouseDetect();

	current = MouseDetect();

	if (debug) DebugMode();
}

void Image::Draw()
{
	Sprite img;
	img.pos.x = pos.first;
	img.pos.y = pos.second;
	img.rect = section;
	img.texture = texture;
	App->render->ui_toDraw.push_back(img);
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
void Image::CleanUp()
{
	parent = nullptr;
	//App->tex->UnLoad(texture);
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
	Sprite lbl;

	lbl.pos.x = pos.first;
	lbl.pos.y = pos.second;
	lbl.rect = text_size;
	lbl.texture = texture;
	App->render->ui_toDraw.push_back(lbl);
}

void Label::SetText(char* text) {
	str = text;
	if (texture != nullptr) {
		App->tex->UnLoad(texture);
	}
	texture = App->font->Print(str.c_str(), color, font);
	App->font->CalcSize(str.c_str(), width, height);
}
void Label::SetString(string text) {
	if (str != text)
	{
		str = text.c_str();
		if (texture != nullptr) {
			App->tex->UnLoad(texture);
		}
		texture = App->font->Print(str.c_str(), color, font);
		App->font->CalcSize(str.c_str(), width, height, font);
	}
}

void Label::SetSize(int size) {
	font = App->font->Load(nullptr, size);
	this->size = size;
	if (texture != nullptr) {
		App->tex->UnLoad(texture);
	}
	texture = App->font->Print(str.c_str(), color, font);
	App->font->CalcSize(str.c_str(), width, height, font);
}

void Label::SetColor(SDL_Color color)
{
	if (texture != nullptr) App->tex->UnLoad(texture);
	this->color = color;
	texture = App->font->Print(str.c_str(), color, font);
	App->font->CalcSize(str.c_str(), width, height, font);
}

void Label::CleanUp()
{
	parent = nullptr;
	font = nullptr;
	App->tex->UnLoad(texture);
	str.clear();
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

void Button::CleanUp()
{
	parent = nullptr;
	//App->tex->UnLoad(texture);
	blit_sections.clear();
	detect_sections.clear();
}

void Button::Draw(SDL_Rect section)
{
	Sprite bt;
	bt.pos.x = pos.first;
	bt.pos.y = pos.second;
	bt.rect = section;
	bt.texture = texture;
	App->render->ui_toDraw.push_back(bt);
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

	Sprite inpt;
	inpt.pos.x = pos.first;
	inpt.pos.y = pos.second;
	inpt.rect = text_size;
	inpt.texture = texture;
	App->render->ui_toDraw.push_back(inpt);
}

void InputText::CleanUp()
{
	parent = nullptr;
	App->tex->UnLoad(texture);
	font = nullptr;
	str.clear();
	cpy_str.clear();
	words_lenght.clear();
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
void Quad::CleanUp()
{
	parent = nullptr;


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
	Sprite crsr;
	crsr.pos.x = pos.first - blitoffset.first;
	crsr.pos.y = pos.second - blitoffset.second;
	crsr.rect = sprite_list[id];
	crsr.texture = texture;
	App->render->ui_toDraw.push_back(crsr);

	App->render->Blit(texture, pos.first - blitoffset.first, pos.second - blitoffset.second, &sprite_list[id]);
}
void Cursor::SetCursor(int id) {
	this->id = id;
}

void Cursor::CleanUp()
{
	sprite_list.clear();
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
void WindowUI::CleanUp()
{
	for (list<UIElement*>::iterator it = in_window.begin(); it != in_window.end(); ++it)
	{
		//RELEASE((*it));
		App->gui->DestroyUIElement(it._Ptr->_Myval);
		in_window.remove(it._Ptr->_Myval);
	}

	in_window.clear();
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