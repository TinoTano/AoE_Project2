#include "Resource.h"
#include "Render.h"
#include "Application.h"
#include "p2Log.h"
#include "EntityManager.h"

Resource::Resource()
{
}

Resource::Resource(int posX, int posY, Resource* resource, int resourceRectIndex)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->type = resource->type;

	resourceLife = resource->resourceLife;
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
<<<<<<< HEAD
{	
	App->render->Blit(entityTexture, entityPosition.x - (resourceRect.w / 2), entityPosition.y - resourceRect.h, &resourceRect);
=======
{
	Sprite resource;
	resource.pos = { entityPosition.x - (resourceRect.w / 2), entityPosition.y - resourceRect.h };
	resource.texture = entityTexture;
	resource.priority = entityPosition.y;
	resource.rect = resourceRect;

	App->render->sprites_toDraw.push_back(resource);
>>>>>>> origin/master
	return true;
}

void Resource::Dead()
{
	App->entityManager->DeleteResource(this);
}
