#include "Resource.h"
#include "Render.h"
#include "Application.h"
#include "p2Log.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Collision.h"
#include "Textures.h"

Resource::Resource()
{
}

Resource::Resource(int posX, int posY, Resource* resource)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	type = resource->type;
	visual = resource->visual;
	MaxLife = resource->Life;
	Life = resource->Life;
	resourceIdleTexture = resource->resourceIdleTexture;
	resourceGatheringTexture = resource->resourceGatheringTexture;
	

	if (resource->resourceRectVector.size() > 0) {
		int rectID = (rand() % resource->resourceRectVector.size()) - 1;
		
		if (rectID < 0)
			rectID = 0;

		resourceRect = resource->resourceRectVector.at(rectID);
	}
	else {
		uint w = 0, h = 0;
		App->tex->GetSize(resourceIdleTexture, w, h);
		resourceRect.x = 0, resourceRect.y = 0, resourceRect.w = w, resourceRect.h = h;
	}


	collider = App->collision->AddCollider({ entityPosition.x, entityPosition.y - (resourceRect.w / 2)}, resourceRect.w / 2, COLLIDER_RESOURCE, (Module*)App->entityManager, this);
	entityTexture = resourceIdleTexture;
	faction = NATURE;
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
	Sprite resource;
	resource.pos.x = entityPosition.x - (resourceRect.w / 2);
	resource.pos.y = entityPosition.y - resourceRect.h;
	resource.texture = entityTexture;
	resource.priority = entityPosition.y;
	resource.rect = resourceRect;

	App->render->sprites_toDraw.push_back(resource);
	return true;
}

void Resource::Damaged() {

	entityTexture = resourceGatheringTexture;
	uint w = 0, h = 0;
	App->tex->GetSize(resourceGatheringTexture, w, h);
	resourceRect.x = 0, resourceRect.y = 0, resourceRect.w = w, resourceRect.h = h;

}

void Resource::Destroy() {

	App->collision->DeleteCollider(collider);
	App->entityManager->DeleteResource(this);

}
