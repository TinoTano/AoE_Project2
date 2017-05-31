#include "Collision.h"
#include "Application.h"
#include "Input.h"
#include "p2Log.h"
#include "Entity.h"
#include "Render.h"
#include "Map.h"
#include "Window.h"
#include "StaticQuadtree.h"

Collision::Collision() : Module()
{
	name = "collision";

	matrix[COLLIDER_UNIT][COLLIDER_UNIT] = true;
	matrix[COLLIDER_UNIT][COLLIDER_BUILDING] = true;
	matrix[COLLIDER_UNIT][COLLIDER_RESOURCE] = true;
	matrix[COLLIDER_UNIT][COLLIDER_RANGE] = false;
	matrix[COLLIDER_UNIT][COLLIDER_LOS] = false;

	matrix[COLLIDER_BUILDING][COLLIDER_UNIT] = false;
	matrix[COLLIDER_BUILDING][COLLIDER_BUILDING] = false;
	matrix[COLLIDER_BUILDING][COLLIDER_RESOURCE] = false;
	matrix[COLLIDER_BUILDING][COLLIDER_RANGE] = false;
	matrix[COLLIDER_BUILDING][COLLIDER_LOS] = false;

	matrix[COLLIDER_RESOURCE][COLLIDER_UNIT] = false;
	matrix[COLLIDER_RESOURCE][COLLIDER_BUILDING] = false;
	matrix[COLLIDER_RESOURCE][COLLIDER_RESOURCE] = false;
	matrix[COLLIDER_RESOURCE][COLLIDER_RANGE] = false;
	matrix[COLLIDER_RESOURCE][COLLIDER_LOS] = false;

	matrix[COLLIDER_RANGE][COLLIDER_UNIT] = false;
	matrix[COLLIDER_RANGE][COLLIDER_BUILDING] = false;
	matrix[COLLIDER_RANGE][COLLIDER_RESOURCE] = false;
	matrix[COLLIDER_RANGE][COLLIDER_RANGE] = false;
	matrix[COLLIDER_RANGE][COLLIDER_LOS] = false;

	matrix[COLLIDER_LOS][COLLIDER_UNIT] = true;
	matrix[COLLIDER_LOS][COLLIDER_BUILDING] = true;
	matrix[COLLIDER_LOS][COLLIDER_RESOURCE] = false;
	matrix[COLLIDER_LOS][COLLIDER_RANGE] = false;
	matrix[COLLIDER_LOS][COLLIDER_LOS] = false;


}


Collision::~Collision()
{
}

bool Collision::Awake(pugi::xml_node &)
{
	return true;
}

bool Collision::Start()
{
	uint w, h;
	App->win->GetWindowSize(w, h);
	quadTree = new StaticQuadTree();
	return true;
}

bool Collision::PreUpdate()
{
	for (list<Collider*>::iterator it = colliders_to_delete.begin(); it != colliders_to_delete.end(); it++) {

		quadTree->Remove(*it);
		colliders_to_delete.erase(it);
		RELEASE(*it);
	}

	Collider *c1 =	nullptr;
	Collider *c2 =	nullptr;
	
	for (list<Collider*>::iterator col1 = colliders.begin(); col1 != colliders.end(); col1++) {
		if ((*col1)->type == COLLIDER_UNIT || (*col1)->type == COLLIDER_LOS) {
			c1 = (*col1);

			potential_collisions.clear();
			quadTree->Retrieve(potential_collisions, c1);

			for (list<Collider*>::iterator col2 = potential_collisions.begin(); col2 != potential_collisions.end(); col2++) {
				c2 = (*col2);

				if (c1->CheckCollision(c2) == true && matrix[c1->type][c2->type] && c1->callback && c1 != c2) 
					c1->callback->OnCollision(*c1, *c2);
			}
		}
	}

	return true;
}

bool Collision::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) 
		debug = !debug;
	
	if (debug) 
		DebugDraw();
	
	return true;
}

bool Collision::CleanUp()
{
	LOG("Freeing colliders");
	list<Collider*>::reverse_iterator it = colliders.rbegin();

	while (it != colliders.rend())
	{
		RELEASE(*it);
		++it;
	}

	colliders.clear();
	quadTree->ClearTree();

	return true;
}

bool Collision::Load(pugi::xml_node &)
{
	/*for (list<Collider*>::const_iterator it = colliders.begin(); it != colliders.end(); ++it) {
	DeleteCollider((*it));
	}

	colliders.clear();

	for (pugi::xml_node collider = data.append_child("Collider"); collider; collider = collider.next_sibling("Collider"))
	{
	iPoint pos;
	pos.x = collider.child("position").attribute("x").as_int();
	pos.y = collider.child("position").attribute("y").as_int();

	int radius = collider.attribute("radius").as_int();
	int quadtree = collider.attribute("quadtree").as_int();
	bool colliding = collider.attribute("collision").as_bool();
	COLLIDER_TYPE type = (COLLIDER_TYPE)collider.attribute("type").as_uint();

	Module* callback = nullptr;
	for (list<Module*>::iterator it = App->modules.begin(); it != App->modules.end(); ++it)
	{
	if ((*it)->name == collider.attribute("callback").as_string())
	{
	callback = (*it);
	}
	}
	Entity* entity = nullptr;
	for (list<Entity*>::iterator it = App->entityManager->WorldEntityList.begin(); it != App->entityManager->WorldEntityList.end(); ++it)
	{
	if ((*it)->entityID == collider.attribute("entity").as_uint())
	{
	entity = (*it);
	}
	}

	Collider* curr = AddCollider(pos, radius, type, callback, entity);
	curr->quadtree_node = quadtree;
	curr->colliding = colliding;
	}*/

	return true;
}

bool Collision::Save(pugi::xml_node & data) const
{

	//for (list<Collider*>::const_iterator it = colliders.begin(); it != colliders.end(); ++it){

	//	pugi::xml_node collider = data.append_child("Collider");

	//	pugi::xml_node pos = collider.append_child("position");
	//	pos.append_attribute("x") = (*it)->pos.x;
	//	pos.append_attribute("y") = (*it)->pos.y;

	//	collider.append_attribute("radius") = (*it)->r;
	//	collider.append_attribute("quadtree") = (*it)->quadtree_node;
	//	collider.append_attribute("collision") = (*it)->colliding;
	//	collider.append_attribute("type") = (*it)->type;
	//	collider.append_attribute("callback") = (*it)->callback->name.c_str();
	//	collider.append_attribute("entity") = (*it)->entity->entityID;
	//}

	return true;
}

Collider * Collision::AddCollider(iPoint position, int radius, COLLIDER_TYPE type, Module* callback, Entity* entity)
{
	Collider* ret = new Collider(position, radius, type, callback, entity);
	colliders.push_back(ret);
	quadTree->Insert(ret);
	
	return ret;
}

void Collision::DeleteCollider(Collider * collider)
{
	colliders.remove(collider);
	colliders_to_delete.push_back(collider);
}

bool Collider::CheckCollision(Collider* c2) const
{
	int radius = r + c2->r + 3;
	int deltaX = pos.x - c2->pos.x;
	int deltaY = pos.y - c2->pos.y;


	return (abs(deltaX) < radius && abs(deltaY) < radius*sin(0.54f));

}

Unit* Collider::GetUnit() {

	Unit* unit = nullptr;

	if (entity->collider->type == COLLIDER_UNIT)
		unit = (Unit*)entity;

	return unit;
}

Building* Collider::GetBuilding() {

	Building* building = nullptr;

	if (entity->collider->type == COLLIDER_BUILDING)
		building = (Building*)entity;

	return building;
}

Resource* Collider::GetResource() {

	Resource* resource = nullptr;

	if (entity->collider->type == COLLIDER_RESOURCE)
		resource = (Resource*)entity;

	return resource;
}


void Collision::DebugDraw()
{

	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++)
	{
		if (App->render->CullingCam((*it)->pos)) {
			if ((*it)->colliding)
				App->render->DrawIsometricCircle((*it)->pos.x, (*it)->pos.y, (*it)->r, 255, 0, 0, 255);
			else
				App->render->DrawIsometricCircle((*it)->pos.x, (*it)->pos.y, (*it)->r, 0, 0, 255, 255);
		}
	}
}

Collider* Collision::FindCollider(iPoint worldPos, int radius) {

	Collider* col = nullptr;
	iPoint MapPos = App->map->WorldToMap(worldPos.x, worldPos.y);
	int quadtree_node = ((trunc((float)(MapPos.y / NODES_FOR_ROW)) * NODES_FOR_ROW) + (trunc((float)(MapPos.x / NODES_FOR_ROW))));

	if (colliders.size() > 0 && (quadtree_node >= 0 && quadtree_node < 100)) {
		col = colliders.front();
		for (list<Collider*>::iterator it = quadTree->nodes.at(quadtree_node).begin(); it != quadTree->nodes.at(quadtree_node).end(); it++) {
			if ((*it)->type == COLLIDER_RANGE || (*it)->type == COLLIDER_LOS)
				continue;

			if ((*it)->pos.DistanceTo(worldPos) < col->pos.DistanceTo(worldPos))
				col = (*it);
		}

		if (col->pos.DistanceTo(worldPos) > (col->r + radius) || col->entity->state == DESTROYED)
			col = nullptr;
	}

	return col;
}
