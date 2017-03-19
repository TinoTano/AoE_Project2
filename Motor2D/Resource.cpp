#include "Resource.h"
#include "Render.h"
#include "Application.h"

Resource::Resource()
{
}

Resource::Resource(int posX, int posY, Resource* resource)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->type = type;

	resourceLife = resource->resourceLife;
	resourceIdleTexture = resource->resourceIdleTexture;
	resourceGatheringTexture = resource->resourceGatheringTexture;

	entityTexture = resourceIdleTexture;
}


Resource::~Resource()
{
}

bool Resource::Update(float dt)
{
	return true;
}

bool Resource::Draw()
{
	App->render->Blit(entityTexture, entityPosition.x, entityPosition.y);
	return true;
}
