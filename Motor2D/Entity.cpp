#include "Entity.h"
#include "Render.h"
#include "Collision.h"
#include "Application.h"

#define HPBAR_WIDTH 50

Entity::Entity()
{
	entityTexture = nullptr;
}

Entity::~Entity()
{
}

iPoint Entity::GetPosition() const
{
	return entityPosition;
}

int Entity::GetEntityID() const
{
	return entityID;
}

void Entity::SetActive(bool active)
{
	isActive = active;
}

void Entity::drawLife(iPoint barPos) {

	Sprite bar;
	Sprite bar2;

	int percent = ((MaxLife - Life) * 100) / MaxLife;
	int barPercent = (percent * HPBAR_WIDTH) / 100;

	bar.rect.x = bar2.rect.x = barPos.x;
	bar.rect.y = bar2.rect.y = barPos.y;
	bar.rect.w = HPBAR_WIDTH;
	bar.rect.h = bar2.rect.h = 5;
	bar.priority = entityPosition.y + 10;
	bar.r = 255;
	bar.g = 0;
	bar.b = 0;

	bar2.rect.w = MIN(HPBAR_WIDTH, MAX(HPBAR_WIDTH - barPercent, 0));
	bar2.priority = entityPosition.y + 11;
	bar2.r = 0;
	bar2.g = 255;
	bar2.b = 0;

	App->render->sprites_toDraw.push_back(bar);
	App->render->sprites_toDraw.push_back(bar2);

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


bool Entity::Load(pugi::xml_node &)
{
	return true;
}

bool Entity::Save(pugi::xml_node &) const
{
	return true;
}

