#include "CutSceneManager.h"
#include "Textures.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Audio.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Gui.h"
#include "Window.h"
#include "Scene.h"
#include "p2Log.h"
#include "SceneManager.h"
#include "Application.h"
#include "FileSystem.h"
#include "Fonts.h"
#include "Unit.h"
#include "FogOfWar.h"
#include "Orders.h"
#include "Fonts.h"

#define MASK_W  128
#define MASK_H  72
#define MASK_SIZE 15.0f

CutSceneManager::CutSceneManager()
{
	name = "cutscene";
}

CutSceneManager::~CutSceneManager()
{
}

bool CutSceneManager::Awake(pugi::xml_node & config)
{
	active = false;
	return true;
}

bool CutSceneManager::Start()
{
	active = true;

	App->render->camera.x = STARTING_CAMERA_X;
	App->render->camera.y = STARTING_CAMERA_Y;

	App->render->culling_cam.x = -App->render->camera.x - 300;
	App->render->culling_cam.y = -App->render->camera.y - 300;

	return true;
}

bool CutSceneManager::Update(float dt)
{
	//Remove from active actions the ones already finished.
	for (std::list<CutsceneAction*>::iterator act = active_actions.begin(); act != active_actions.end();)
	{
		if ((*act)->start + (*act)->duration < scene_timer.ReadSec())
		{
			RELEASE(*act);
			act = active_actions.erase(act);
		}
		else
			++act;
	}

	//Add to active actions the ones that need to start
	while (!remaining_actions.empty() && remaining_actions.top()->start < scene_timer.ReadSec())
	{
		active_actions.push_back(remaining_actions.top());
		remaining_actions.pop();
	}

	//Do active actions
	PerformActions(dt);

	//Update elements that don't have a module
	UpdateElements(dt);

	//Chenge between 2 cutscenes
	if (change_scene)
		ChangeScene();

	//If cutscene finished, change scene
	if (!change_scene && remaining_actions.empty() && active_actions.empty() && !finished)
	{
		finished = true;
		if (App->sceneManager->current_scene == App->sceneManager->play_scene)
		{
			CleanUp();
			App->sceneManager->ChangeScene(App->sceneManager->current_scene, App->sceneManager->level1_scene);
		}
	}

	return true;
}

bool CutSceneManager::CleanUp()
{
	ClearScene();
	App->fog->CleanUp();
	return true;
}

int CutSceneManager::GetNextID() const
{
	return elements.size();
}

void CutSceneManager::Play(const char * path, Scene* next_scene)
{
	if (App->sceneManager->current_scene == App->sceneManager->menu_scene) App->sceneManager->ChangeScene(App->sceneManager->menu_scene, App->sceneManager->play_scene);
	else App->sceneManager->ChangeScene(App->sceneManager->level1_scene, App->sceneManager->play_scene);

	Load(path);
	scene_timer.Start();
	finished = false;
	this->next_scene = next_scene;
}

bool CutSceneManager::HasFinished() const
{
	return finished;
}

void CutSceneManager::Load(const char * path)
{
	pugi::xml_document doc;
	char* buff;
	int size = App->fs->Load(path, &buff);
	pugi::xml_parse_result result = doc.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load questData xml file. Pugi error: %s", result.description());
	}

	pugi::xml_node elements = doc.child("file").child("elements");

	for (pugi::xml_node group = elements.child("type"); group; group = group.next_sibling("type"))
	{
		string type = group.attribute("group").as_string();

		if (type == "map")
		{
			for (pugi::xml_node map = group.child("map"); map; map = map.next_sibling("map"))
			{
				LoadMap(map);
				App->fog->Start();
			}
		}
		else if (type == "image")
		{
			for (pugi::xml_node image = group.child("image"); image; image = image.next_sibling("image"))
			{
				LoadCSImage(image);
			}
		}
		else if (type == "unit")
		{
			for (pugi::xml_node unit = group.child("unit"); unit; unit = unit.next_sibling("unit"))
			{
				LoadUnit(unit);
			}
		}
		else if (type == "building")
		{
			for (pugi::xml_node building = group.child("building"); building; building = building.next_sibling("building"))
			{
				LoadBuilding(building);
			}
		}
		else if (type == "music")
		{
			for (pugi::xml_node music = group.child("music"); music; music = music.next_sibling("music"))
			{
				LoadMusic(music);
			}
		}
		else if (type == "sound_effect")
		{
			for (pugi::xml_node fx = group.child("sound_effect"); fx; fx = fx.next_sibling("sound_effect"))
			{
				LoadSoundEffect(fx);
			}
		}
		else if (type == "text")
		{
			for (pugi::xml_node txt = group.child("text"); txt; txt = txt.next_sibling("text"))
			{
				LoadText(txt);
			}
		}
	}

	pugi::xml_node scene = doc.child("file").child("scene");

	for (pugi::xml_node act = scene.child("act"); act; act = act.next_sibling("act"))
	{
		string type = act.attribute("type").as_string();

		if (type == "move")
		{
			LoadMove(act);
		}
		else if (type == "modify")
		{
			LoadModify(act);
		}
		else
		{
			LoadAction(act);
		}
	}

}

elements_groups CutSceneManager::GetElementGroup(const char * ele) const
{
	for (std::list<CutsceneElement*>::const_iterator e = elements.begin(); e != elements.end(); ++e)
	{
		if ((*e)->name == ele)
			return (*e)->group;
	}

	return e_g_null;
}

CutsceneElement * CutSceneManager::GetElement(const char * ele) const
{
	for (std::list<CutsceneElement*>::const_iterator e = elements.begin(); e != elements.end(); ++e)
	{
		if ((*e)->name == ele)
			return *e;
	}

	return nullptr;
}

void CutSceneManager::PerformActions(float dt)
{
	for (std::list<CutsceneAction*>::iterator act = active_actions.begin(); act != active_actions.end(); ++act)
	{
		if ((*act)->element_name == "camera")
		{
			if ((*act)->action == a_move)
			{
				CutsceneMove* move = static_cast<CutsceneMove*>(*act);
				MoveCamera(move);
			}
			else if ((*act)->action == a_modify)
			{
				PerformModify(nullptr, *act);
			}
		}
		else
		{
			CutsceneElement* element = GetElement((*act)->element_name.c_str());

			switch ((*act)->action)
			{
			case a_move:
			{
				CutsceneMove* move = static_cast<CutsceneMove*>(*act);

				PerformMove(element, move);

				break;
			}
			case a_action:
				PerformAction(element);
				break;
			case a_play:
				PerformPlay(element);
				break;
			case a_stop:
				PerformStop(element);
				break;
			case a_modify:
				PerformModify(element, *act);
				break;
			case a_enable:
				PerformEnable(element);
				break;
			case a_disable:
				PerformDisable(element);
				break;
			default:
				break;
			}
		}
	}
}

void CutSceneManager::ClearScene()
{
	for (std::list<CutsceneElement*>::iterator it = elements.begin(); it != elements.end();)
	{
		//if ((*it)->group == e_g_map)
		//	App->map->CleanUp();

		if ((*it)->group == e_g_unit)
		{
			CutsceneUnit* unit = static_cast<CutsceneUnit*>(*it);
			if (unit->GetUnit() != nullptr) {
				unit->GetUnit()->Destroy();
				App->entityManager->DeleteEntity(unit->GetUnit());
			}
		}
		if ((*it)->group == e_g_building)
		{
			CutsceneBuilding* building = static_cast<CutsceneBuilding*>(*it);
			if (building->GetBuilding() != nullptr) {
				building->GetBuilding()->Destroy();
				App->entityManager->DeleteEntity(building->GetBuilding());
			}
		}

		RELEASE(*it);
		it = elements.erase(it);
	}

	while (!remaining_actions.empty())
	{
		CutsceneAction* a = remaining_actions.top();
		RELEASE(a);
		remaining_actions.pop();
	}

	for (std::list<CutsceneAction*>::iterator it = active_actions.begin(); it != active_actions.end();)
	{
		RELEASE(*it);
		it = active_actions.erase(it);
	}
}

void CutSceneManager::LoadMap(pugi::xml_node & node)
{
	if (node.attribute("preload").as_bool() == false)
	{
		if (App->map->Load(node.attribute("path").as_string()))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
				App->pathfinding->SetMap(w, h, data);

			RELEASE_ARRAY(data);
		}
	}

	CutsceneMap* m = new CutsceneMap(e_g_map, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool());

	elements.push_back(m);
}

void CutSceneManager::LoadCSImage(pugi::xml_node & node)
{
	CutsceneImage* i = new CutsceneImage(e_g_image, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(),
	{ node.attribute("pos_x").as_int() - (node.attribute("rect_w").as_int() / 2), node.attribute("pos_y").as_int() - (node.attribute("rect_h").as_int() / 2) },
	{ node.attribute("rect_x").as_int(),node.attribute("rect_y").as_int(),node.attribute("rect_w").as_int(),node.attribute("rect_h").as_int() }, node.attribute("layer").as_int());

	elements.push_back(i);

}

void CutSceneManager::LoadUnit(pugi::xml_node & node)
{
	CutsceneUnit* e = new CutsceneUnit(e_g_unit, node.attribute("path").as_string(), node.attribute("name").as_string(), (unitType)node.attribute("type").as_uint(), node.attribute("active").as_bool(), { node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int() });

	elements.push_back(e);
}

void CutSceneManager::LoadBuilding(pugi::xml_node & node)
{
	CutsceneBuilding* b = new CutsceneBuilding(e_g_building, node.attribute("path").as_string(), node.attribute("name").as_string(), (buildingType)node.attribute("type").as_uint(), node.attribute("active").as_bool(), { node.attribute("pos_x").as_int(),node.attribute("pos_y").as_int() });

	elements.push_back(b);
}


void CutSceneManager::LoadMusic(pugi::xml_node & node)
{
	CutsceneMusic* m = new CutsceneMusic(e_g_music, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool());

	elements.push_back(m);
}

void CutSceneManager::LoadSoundEffect(pugi::xml_node & node)
{
	CutsceneSoundEffect* fx = new CutsceneSoundEffect(e_g_sound_effect, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), node.attribute("loops").as_int());

	if (node.attribute("active").as_bool() == true)
	{
		App->audio->PlayFx(fx->GetID(), fx->GetLoops());
	}

	elements.push_back(fx);
}

void CutSceneManager::LoadText(pugi::xml_node & node)
{
	CutsceneText* t = new CutsceneText(e_g_text, node.attribute("path").as_string(), node.attribute("name").as_string(), node.attribute("active").as_bool(), { -STARTING_CAMERA_X + node.attribute("pos_x").as_int(), -STARTING_CAMERA_Y + node.attribute("pos_y").as_int() });
	t->label = (Label*)App->gui->CreateLabel(node.attribute("txt").as_string(), -STARTING_CAMERA_X + node.attribute("pos_x").as_int(), -STARTING_CAMERA_Y + node.attribute("pos_y").as_int(), App->font->fonts[TWENTY]);

	elements.push_back(t);
}

void CutSceneManager::UpdateElements(float dt)
{
	//Just maps and images need to be updated. Entities, music, sound_efects and text have modules that take care of them (entities, audio, gui)
	for (std::list<CutsceneElement*>::iterator ele = elements.begin(); ele != elements.end(); ++ele)
	{
		if ((*ele)->group == e_g_map)
		{
			if ((*ele)->active == true)
			{
				App->map->Draw();
			}

		}
		else if ((*ele)->group == e_g_image)
		{
			if ((*ele)->active == true)
			{
				CutsceneImage* image = dynamic_cast<CutsceneImage*>(*ele);
				App->render->Blit(image->GetTexture(), image->GetPos().x, image->GetPos().y, &image->GetRect());
			}
		}
	}
}

void CutSceneManager::LoadAction(pugi::xml_node & node)
{
	string type = node.attribute("type").as_string();

	actions a = a_null;

	if (type == "action")
		a = a_action;
	else if (type == "play")
		a = a_play;
	else if (type == "stop")
		a = a_stop;
	else if (type == "enable")
		a = a_enable;
	else if (type == "disable")
		a = a_disable;

	CutsceneAction* action = new CutsceneAction(a, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

	remaining_actions.push(action);
}

void CutSceneManager::LoadMove(pugi::xml_node & node)
{
	CutsceneMove* m = new CutsceneMove(a_move, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

	pugi::xml_node move = node.child("move");

	// set destination
	m->dest = { move.attribute("x").as_int(),move.attribute("y").as_int() };


	// set reference type (local, map, global). if ref is not valit global is assigned as default
	reference_type ref = r_t_null;
	string ref_str = move.attribute("ref").as_string();

	if (ref_str == "local")
		ref = r_t_local;
	else if (ref_str == "map")
		ref = r_t_map;
	else
		ref = r_t_global;

	m->reference = ref;

	remaining_actions.push(m);
}

void CutSceneManager::LoadModify(pugi::xml_node & node)
{
	string ele = node.attribute("element").as_string();
	pugi::xml_node modify = node.child("modify");

	if (ele == "camera")
	{
		CutsceneModifyCamera* mc = new CutsceneModifyCamera(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());
		mc->position = { modify.attribute("x").as_int(),modify.attribute("y").as_int() };

		remaining_actions.push(mc);
	}
	else
	{
		switch (GetElementGroup(ele.c_str()))
		{
		case e_g_unit:
		{
			CutsceneModifyUnit* me = new CutsceneModifyUnit(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

			unit_actions e_action = e_a_null;
			string e_action_str = modify.attribute("action").as_string();

			if (e_action_str == "kill")
				e_action = e_a_kill;
			else if (e_action_str == "spawn")
				e_action = e_a_spawn;
			else
				e_action = e_a_change_pos;

			me->unit_action = e_action;

			me->pos = { modify.attribute("x").as_int(), modify.attribute("y").as_int() };

			remaining_actions.push(me);

			break;
		}
		case e_g_image:
		{
			CutsceneModifyImage* mi = new CutsceneModifyImage(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

			mi->var = modify.attribute("var").as_string();

			mi->rect = { modify.attribute("x").as_int(),  modify.attribute("y").as_int() , modify.attribute("w").as_int() , modify.attribute("h").as_int() };

			mi->path = modify.attribute("path").as_string();

			remaining_actions.push(mi);

			break;
		}
		case e_g_text:
		{
			CutsceneModifyText* mt = new CutsceneModifyText(a_modify, node.attribute("element").as_string(), node.attribute("start").as_int(), node.attribute("duration").as_int());

			mt->txt = modify.attribute("txt").as_string();

			remaining_actions.push(mt);

			break;
		}
		default:
			break;
		}
	}
}

void CutSceneManager::PerformMove(CutsceneElement * ele, CutsceneMove * move)
{
	float rel_time = (scene_timer.ReadSec() - (float)move->start) / (float)move->duration;

	switch (ele->group)
	{
	case e_g_image:
		break;
	case e_g_unit:
	{
		CutsceneUnit* unit = static_cast<CutsceneUnit*>(ele);

		// Here put only move if the entity is a unit
		unit->GetUnit()->order_list.push_front(new MoveToOrder(unit->GetUnit(), move->dest));
	}
	case e_g_text:
		break;
	default:
		break;
	}
}

void CutSceneManager::PerformAction(CutsceneElement * ele)
{
}

void CutSceneManager::PerformPlay(CutsceneElement * ele)
{
	//Just music and sound effect can be played
	if (ele->group == e_g_music)
	{
		CutsceneMusic* music = static_cast<CutsceneMusic*>(ele);

		music->Play();
	}
	else if (ele->group == e_g_sound_effect)
	{
		CutsceneSoundEffect* fx = static_cast<CutsceneSoundEffect*>(ele);

		fx->Play();
	}

}

void CutSceneManager::PerformStop(CutsceneElement * ele)
{
}

void CutSceneManager::PerformModify(CutsceneElement * ele, CutsceneAction * act)
{
	if (act->element_name == "camera")
	{
		CutsceneModifyCamera* modify = static_cast<CutsceneModifyCamera*>(act);
		App->render->camera.x = -modify->position.x;
		App->render->camera.y = -modify->position.y;
	}
	else
	{
		//Just entity, image and text can be modified
		if (ele->group == e_g_unit)
		{
			CutsceneUnit* e = static_cast<CutsceneUnit*>(ele);
			CutsceneModifyUnit* modify = static_cast<CutsceneModifyUnit*>(act);

			switch (modify->unit_action)
			{
			case e_a_kill:
				if (e->GetUnit() != nullptr)
				{
					App->entityManager->DeleteEntity((Unit*)e->GetUnit());
					e->SetNull();
				}
				break;
			case e_a_spawn:
				if (e->GetUnit() == nullptr)
				{
					e->SetUnit(App->entityManager->CreateUnit(modify->pos.x, modify->pos.y, modify->type));
				}
				break;
			case e_a_change_pos:
				e->GetUnit()->entityPosition = modify->pos; //should be a teleport to the position
				break;
			default:
				break;
			}
		}
		else if (ele->group == e_g_image)
		{
			CutsceneImage* i = static_cast<CutsceneImage*>(ele);
			CutsceneModifyImage* modify = static_cast<CutsceneModifyImage*>(act);

			if (modify->var == "tex")
				i->ChangeTex(modify->path.c_str());
			else if (modify->var == "rect")
				i->ChangeRect(modify->rect);
			else if (modify->var == "both")
			{
				i->ChangeTex(modify->path.c_str());
				i->ChangeRect(modify->rect);
			}

		}
		else if (ele->group == e_g_text)
		{
			CutsceneText* t = static_cast<CutsceneText*>(ele);
			CutsceneModifyText* modify = static_cast<CutsceneModifyText*>(act);

			t->label->SetText((char*)modify->txt.c_str());
		}
	}
}

void CutSceneManager::PerformEnable(CutsceneElement * ele)
{
	switch (ele->group)
	{
	case e_g_map:
		ele->active = true;
		break;
	case e_g_image:
		ele->active = true;
		break;
	case e_g_unit:
	{
		CutsceneUnit* e = static_cast<CutsceneUnit*>(ele);

		e->active = true;
		e->GetUnit()->isActive = true;

		break;
	}
	case e_g_text:
	{
		CutsceneText* t = static_cast<CutsceneText*>(ele);

		t->active = true;
		t->GetText()->enabled = true;

		break;
	}
	default:
		break;
	}
}

void CutSceneManager::PerformDisable(CutsceneElement * ele)
{
	switch (ele->group)
	{
	case e_g_map:
		ele->active = false;
		break;
	case e_g_image:
		ele->active = false;
		break;
	case e_g_unit:
	{
		CutsceneUnit* e = static_cast<CutsceneUnit*>(ele);

		e->active = false;
		e->GetUnit()->isActive = false;

		break;
	}
	case e_g_text:
	{
		CutsceneText* t = static_cast<CutsceneText*>(ele);

		t->active = false;
		t->GetText()->enabled = false;

		break;
	}
	default:
		break;
	}
}

void CutSceneManager::MoveCamera(CutsceneMove * move)
{
	float rel_time = (scene_timer.ReadSec() - (float)move->start) / (float)move->duration;

	if (move->first_time)
	{
		move->initial_pos = { App->render->camera.x, App->render->camera.y };
		move->first_time = false;
	}

	int delta_x = -move->dest.x - move->initial_pos.x;
	int delta_y = -move->dest.y - move->initial_pos.y;

	// Here goes move camera
}

//-----------------------
// Element
//-----------------------

CutsceneElement::CutsceneElement(elements_groups group, const char * path, const char* name, bool active) : group(group), path(path), active(active), name(name)
{
	id = App->cutscene->GetNextID();
}
//----------------------

//-----------------------
// Map
//-----------------------

CutsceneMap::CutsceneMap(elements_groups group, const char * path, const char* name, bool active) : CutsceneElement(group, path, name, active)
{
}
//----------------------

//-----------------------
// Image
//-----------------------

CutsceneImage::CutsceneImage(elements_groups group, const char * path, const char* name, bool active, iPoint pos, SDL_Rect rect, int layer) : CutsceneElement(group, path, name, active), pos(pos), layer(layer)
{
	texture = App->tex->Load(path);
	this->rect = rect;
}

SDL_Texture * CutsceneImage::GetTexture() const
{
	return texture;
}

SDL_Rect CutsceneImage::GetRect() const
{
	return rect;
}

iPoint CutsceneImage::GetPos() const
{
	return pos;
}
int CutsceneImage::GetLayer() const
{
	return layer;
}
void CutsceneImage::Move(float x, float y)
{
	pos.x = x;
	pos.y = y;
}
void CutsceneImage::ChangeTex(const char * path)
{
	texture = App->tex->Load(path);
}
void CutsceneImage::ChangeRect(SDL_Rect r)
{
	rect = r;
}
//--------------------

//-----------------------
// Unit
//-----------------------

CutsceneUnit::CutsceneUnit(elements_groups group, const char * path, const char* name, unitType type, bool active, iPoint pos) : CutsceneElement(group, path, name, active)
{
	unit = App->entityManager->CreateUnit(pos.x, pos.y, type);
	unit->isActive = active;
}

CutsceneUnit::~CutsceneUnit()
{
}

Unit * CutsceneUnit::GetUnit() const
{
	return unit;
}
void CutsceneUnit::SetNull()
{
	unit = nullptr;
}
void CutsceneUnit::SetUnit(Unit * e)
{
	unit = e;
}
//----------------------

//-----------------------
// Building
//-----------------------

CutsceneBuilding::CutsceneBuilding(elements_groups group, const char * path, const char* name, buildingType type, bool active, iPoint pos) : CutsceneElement(group, path, name, active)
{
	building = App->entityManager->CreateBuilding(pos.x, pos.y, type);
	App->fog->AddEntity(building);
	building->isActive = active;
}

CutsceneBuilding::~CutsceneBuilding()
{
}

Building * CutsceneBuilding::GetBuilding() const
{
	return building;
}
void CutsceneBuilding::SetNull()
{
	building = nullptr;
}
void CutsceneBuilding::SetBuilding(Building * b)
{
	building = b;
}
//----------------------

//-----------------------
// Music
//-----------------------

CutsceneMusic::CutsceneMusic(elements_groups group, const char * path, const char* name, bool active) : CutsceneElement(group, path, name, active)
{
	if (active)
	{
		App->audio->PlayMusic(path);
		playing = true;
	}
}

CutsceneMusic::~CutsceneMusic()
{
}

bool CutsceneMusic::IsPlaying() const
{
	return playing;
}
void CutsceneMusic::Play()
{
	App->audio->PlayMusic(path.c_str(), 0);
	playing = true;
}
//------------------------

//-----------------------
// Sound Effect
//-----------------------

CutsceneSoundEffect::CutsceneSoundEffect(elements_groups group, const char * path, const char* name, bool active, int loops) : CutsceneElement(group, path, name, active), loops(loops)
{
	id = App->audio->LoadFx(path);
}

CutsceneSoundEffect::~CutsceneSoundEffect()
{
}
int CutsceneSoundEffect::GetID() const
{
	return id;
}
int CutsceneSoundEffect::GetLoops() const
{
	return loops;
}
void CutsceneSoundEffect::Play()
{
	App->audio->PlayFx(id, loops);
}
//----------------

//-----------------------
// Text
//-----------------------

CutsceneText::CutsceneText(elements_groups group, const char * path, const char* name, bool active, iPoint pos) : CutsceneElement(group, path, name, active)
{
	if (!active)
		label->enabled = false;
}

CutsceneText::~CutsceneText()
{
	App->gui->DestroyUIElement(label);
}

void CutsceneText::SetText(char * txt)
{
	label->SetText(txt);
}
Label* CutsceneText::GetText() const
{
	return label;
}
void CutsceneText::Move(float x, float y)
{
	label->SetPos((int)x, (int)y);
}
//---------------------

//---------------------
// Actions
//---------------------

CutsceneAction::CutsceneAction(actions action, const char * name, int start_time, int duration) : action(action), element_name(name), start(start_time), duration(duration)
{
}

CutsceneMove::CutsceneMove(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneMove::~CutsceneMove()
{
}

CutsceneModifyUnit::CutsceneModifyUnit(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneModifyImage::CutsceneModifyImage(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneModifyText::CutsceneModifyText(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutsceneChangeScene::CutsceneChangeScene(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}

CutSceneAction_Cmp::CutSceneAction_Cmp()
{
}

CutsceneModifyCamera::CutsceneModifyCamera(actions action, const char * name, int start_time, int duration) : CutsceneAction(action, name, start_time, duration)
{
}
