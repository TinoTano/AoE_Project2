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

	matrix[COLLIDER_RANGE][COLLIDER_UNIT] = true;
	matrix[COLLIDER_RANGE][COLLIDER_BUILDING] = true;
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

		for (list<Collision_data*>::iterator it2 = collision_list.begin(); it2 != collision_list.end(); it2++) {
			if ((*it) == (*it2)->c1 || (*it) == (*it2)->c2)
				(*it2)->state = SOLVED;
		}

		quadTree->Remove(*it);
		colliders_to_delete.erase(it);
		RELEASE(*it);
	}

	Collider *c1 =	nullptr;
	Collider *c2 =	nullptr;
	
	for (list<Collider*>::iterator col1 = colliders.begin(); col1 != colliders.end(); col1++) {
		if ((*col1)->type == COLLIDER_UNIT || (*col1)->type == COLLIDER_RANGE || (*col1)->type == COLLIDER_LOS) {
			c1 = (*col1);

			potential_collisions.clear();
			quadTree->Retrieve(potential_collisions, c1);

			for (list<Collider*>::iterator col2 = potential_collisions.begin(); col2 != potential_collisions.end(); col2++) {
				c2 = (*col2);

				if (c1->CheckCollision(c2) == true && matrix[c1->type][c2->type] && c1->callback && c1 != c2) {
					if (!FindCollision(c1, c2)) {
						Collision_data* collision = new Collision_data(c1, c2);   
						collision_list.push_back(collision);
					}
				}
			}
		}
	}

	for (list<Collision_data*>::iterator collisions = collision_list.begin(); collisions != collision_list.end(); collisions++) {

		if ((*collisions)->state == UNSOLVED) 
			(*collisions)->c1->callback->OnCollision(*(*collisions));

		if ((*collisions)->state == SOLVING) {
			if ((*collisions)->c1->CheckCollision((*collisions)->c2) == false)
				(*collisions)->state = SOLVED;
		}

		if ((*collisions)->state == SOLVED) {
			(*collisions)->c1->colliding = (*collisions)->c2->colliding = false;
			RELEASE(*collisions);
			collisions = collision_list.erase(collisions);
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

	list<Collision_data*>::reverse_iterator it2 = collision_list.rbegin();

	while (it2 != collision_list.rend())
	{
		RELEASE(*it2);
		++it2;
	}

	collision_list.clear();
	quadTree->ClearTree();

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

	if (type == COLLIDER_UNIT) 
		unit = (Unit*)entity;

	return unit;
}

Building* Collider::GetBuilding() {

	Building* building = nullptr;

	if (type == COLLIDER_BUILDING)
		building = (Building*)entity;

	return building;
}

Resource* Collider::GetResource() {

	Resource* resource = nullptr;

	if (type == COLLIDER_RESOURCE)
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

bool Collision::FindCollision(Collider* col1, Collider* col2) {

	for (list<Collision_data*>::iterator it = collision_list.begin(); it != collision_list.end(); it++) {
		if (((*it)->c1 == col1 && (*it)->c2 == col2) || ((*it)->c2 == col1 && (*it)->c1 == col2))
			return true;
	}

	return false;
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
