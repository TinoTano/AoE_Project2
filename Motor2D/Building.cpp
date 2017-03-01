#include "Building.h"
#include "Render.h"
#include "Application.h"
#include "Map.h"
#include "Textures.h"
#include "Unit.h"
#include "EntityManager.h"
#include "Collision.h"
#include "p2Log.h"

Building::Building(int posX, int posY, bool isEnemy, buildingType type, buildingFaction faction)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->isEnemy = isEnemy;
	this->type = type;
	this->faction = faction;

	switch (faction) {
	case FREE_MEN_BUILDING:
		switch (type) {
		case BARRACKS:
			entityTexture = App->tex->Load("textures/barracksOrcs.png");
			buildingLife = 30;
			buildingAttack = 4;
			attackDelay = 1;
			buildingDefense = 1;
			break;
		}
	}

	SDL_Rect colliderRect = { entityPosition.x, entityPosition.y, 142, 142 }; // missing w and h of texture
	COLLIDER_TYPE colliderType;
	if (isEnemy) {
		colliderType = COLLIDER_ENEMY_BUILDING;
	}
	else {
		colliderType = COLLIDER_FRIENDLY_BUILDING;
	}
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager);
}

Building::~Building()
{
}

bool Building::Update(float dt)
{
	switch (state) {
	case BUILDING_ATTACKING:
		Attack(dt);
		break;
	case BUILDING_DESTROYING:
		//if (currentAnim->Finished()) {
		App->entityManager->DeleteBuilding(this, isEnemy);
		//}
		break;
	}

	if (isDamaged) {
		//blit fire animation
	}

	return true;
}

bool Building::Draw()
{
	App->render->Blit(entityTexture, entityPosition.x, entityPosition.y);
	return true;
}

void Building::Attack(float dt)
{
	if (timer >= attackDelay) {
		attackUnitTarget->unitLife -= buildingAttack - attackUnitTarget->unitDefense;
		if (attackUnitTarget->unitLife <= 0) {
			attackUnitTarget->Dead();
			if (buildingLife > 0) {
				state = BUILDING_IDLE;
			}
		}
		timer = 0;
	}
	else {
		timer += dt;
	}
}

void Building::Dead()
{
	state = BUILDING_DESTROYING;
	App->collision->DeleteCollider(collider);
}

void Building::SetAnim()
{
}

