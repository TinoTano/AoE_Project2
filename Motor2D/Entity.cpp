#include "Entity.h"

Entity::Entity()
{
	entityTexture = nullptr;
}

Entity::~Entity()
{
}

iPoint Entity::GetPosition() const
{
	return position;
}

int Entity::GetEntityID() const
{
	return entityID;
}

void Entity::SetActive(bool active)
{
	isActive = active;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::Draw()
{
	return true;
}

bool Entity::HandleInput()
{
	return true;
}

