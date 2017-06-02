#include "Resource.h"
#include "Render.h"
#include "Application.h"
#include "p2Log.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Collision.h"
#include "FogOfWar.h"
#include "Textures.h"

Resource::Resource()
{
}

Resource::Resource(int posX, int posY, Resource* resource)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	name = resource->name;
	res_type = resource->res_type;
	contains = resource->contains;
	MaxLife = resource->Life;
	Life = resource->Life;
	entityTexture = resource->entityTexture;
	selectionWidth = resource->selectionWidth;
	selectionAreaCenterPoint = resource->selectionAreaCenterPoint;
	
	
	int rectID = rand() % resource->blit_rects.size();
	blit_rect = resource->blit_rects.at(rectID);

	if (res_type == BLACK_TREE || res_type == GREEN_TREE || res_type == GOLD_MINE || res_type == BUSH || res_type == STONE_MINE) {
		collider = App->collision->AddCollider({ entityPosition.x, entityPosition.y - selectionAreaCenterPoint.y }, selectionWidth, COLLIDER_RESOURCE, (Module*)App->entityManager, this);
	}

	entityType = ENTITY_RESOURCE;
	
	faction = NATURE;
	App->fog->AddEntity(this);

}

Resource::Resource(int posX, int posY, Resource * resource, SDL_Rect rect)
{
	entityPosition.x = posX;
	entityPosition.y = posY;

	res_type = resource->res_type;
	contains = resource->contains;
	MaxLife = resource->Life;
	Life = resource->Life;
	entityTexture = resource->entityTexture;
	selectionWidth = resource->selectionWidth;
	selectionAreaCenterPoint = resource->selectionAreaCenterPoint;


	blit_rect = rect;

	if (res_type == BLACK_TREE || res_type == GREEN_TREE || res_type == GOLD_MINE || res_type == BUSH || res_type == STONE_MINE) {
		collider = App->collision->AddCollider({ entityPosition.x, entityPosition.y - selectionAreaCenterPoint.y }, selectionWidth, COLLIDER_RESOURCE, (Module*)App->entityManager, this);
	}

	entityType = ENTITY_RESOURCE;

	faction = NATURE;
	App->fog->AddEntity(this);
}


Resource::~Resource()
{
}


bool Resource::Draw()
{
	Sprite resource;
	resource.pos.x = entityPosition.x - (blit_rect.w / 2);
	resource.pos.y = entityPosition.y - blit_rect.h;
	resource.texture = entityTexture;
	resource.priority = entityPosition.y;
	resource.rect = blit_rect;

	App->render->sprites_toDraw.push_back(resource);
	return true;
}


void Resource::Destroy() {

	state = DESTROYED;

	App->collision->DeleteCollider(collider);
	App->entityManager->DeleteEntity(this);
}
