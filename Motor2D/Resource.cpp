#include "Resource.h"
#include "Render.h"
#include "Application.h"
#include "p2Log.h"

Resource::Resource()
{
}

Resource::Resource(int posX, int posY, Resource* resource, int resourceRectIndex)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->type = type;

	Life = resource->Life;
	resourceIdleTexture = resource->resourceIdleTexture;
	resourceGatheringTexture = resource->resourceGatheringTexture;
	resourceRectVector = resource->resourceRectVector;
	if (resourceRectIndex < resourceRectVector.size()) {
		rectIndex = resource->rectIndex;
		resourceRect = resourceRectVector[rectIndex];
	}
	else {
		LOG("Wrong resourceRectIndex");
	}

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
	App->render->Blit(entityTexture, entityPosition.x, entityPosition.y, &resourceRect);
	return true;
}
