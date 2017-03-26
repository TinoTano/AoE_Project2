#include "Collision.h"
#include "Application.h"
#include "Input.h"
#include "p2Log.h"
#include "Render.h"

Collision::Collision() : Module()
{
	name = "collision";

	matrix[COLLIDER_FRIENDLY_UNIT][COLLIDER_ENEMY_UNIT] = true;
	matrix[COLLIDER_FRIENDLY_UNIT][COLLIDER_FRIENDLY_BUILDING] = true;
	matrix[COLLIDER_FRIENDLY_UNIT][COLLIDER_ENEMY_BUILDING] = true;
	matrix[COLLIDER_FRIENDLY_UNIT][COLLIDER_RESOURCE] = true;
	matrix[COLLIDER_FRIENDLY_UNIT][COLLIDER_FRIENDLY_UNIT] = true;

	matrix[COLLIDER_ENEMY_UNIT][COLLIDER_FRIENDLY_UNIT] = true;
	matrix[COLLIDER_ENEMY_UNIT][COLLIDER_ENEMY_BUILDING] = true;
	matrix[COLLIDER_ENEMY_UNIT][COLLIDER_FRIENDLY_BUILDING] = true;
	matrix[COLLIDER_ENEMY_UNIT][COLLIDER_RESOURCE] = true;
	matrix[COLLIDER_ENEMY_UNIT][COLLIDER_ENEMY_UNIT] = false;

	matrix[COLLIDER_FRIENDLY_BUILDING][COLLIDER_FRIENDLY_UNIT] = true;
	matrix[COLLIDER_FRIENDLY_BUILDING][COLLIDER_ENEMY_UNIT] = true;
	matrix[COLLIDER_FRIENDLY_BUILDING][COLLIDER_ENEMY_BUILDING] = false;
	matrix[COLLIDER_FRIENDLY_BUILDING][COLLIDER_RESOURCE] = false;
	matrix[COLLIDER_FRIENDLY_BUILDING][COLLIDER_FRIENDLY_BUILDING] = false;

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
	return true;
}

bool Collision::PreUpdate()
{
	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++) {
		if ((*it)->to_delete == true)
		{
			RELEASE(*it);
			it = colliders.erase(it);
		}
	}

	Collider *c1;
	Collider *c2;

	for (list<Collider*>::iterator col = colliders.begin(); col != colliders.end(); col++) 
		(*col)->colliding = false;

	for (list<Collider*>::iterator col1 = colliders.begin(); col1 != colliders.end(); col1++) {
		c1 = (*col1);

		for (list<Collider*>::iterator col2 = next(col1); col2 != colliders.end(); col2++) {
			c2 = (*col2);

			if (c1->CheckCollision(c2) == true) {
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}

bool Collision::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
	}

	if (debug) {
		DebugDraw();
	}

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

	return true;
}

Collider * Collision::AddCollider(iPoint position, int radius, COLLIDER_TYPE type, Module* callback, Entity* entity)
{
	Collider* ret = new Collider(position, radius, type, callback, entity);
	colliders.push_back(ret);

	return ret;
}

void Collision::DeleteCollider(Collider * collider)
{
	collider->to_delete = true;
}

bool Collider::CheckCollision(Collider* c2) const
{
	return (pos.DistanceTo(c2->pos) < (r + c2->r));
}

Unit* Collider::GetUnit() {

	Unit* unit = nullptr;

	if (type == COLLIDER_FRIENDLY_UNIT || type == COLLIDER_ENEMY_UNIT)
		unit = (Unit*)entity;

	return unit;
}

Building* Collider::GetBuilding() {

	Building* building = nullptr;

	if (type == COLLIDER_FRIENDLY_BUILDING || type == COLLIDER_ENEMY_BUILDING)
		building = (Building*)entity;

	return building;
}

// GetResource could be implemented if necessary

void Collision::DebugDraw()
{

	for (list<Collider*>::iterator it = colliders.begin(); it != colliders.end(); it++)
	{
		if ((*it) == nullptr) {
			continue;
		}

		if((*it)->colliding)
			App->render->DrawCircle((*it)->pos.x, (*it)->pos.y, (*it)->r, 255, 0, 0, 255, false);
		else
			App->render->DrawCircle((*it)->pos.x, (*it)->pos.y, (*it)->r, 0, 0, 255, 255, false);
	}
}
