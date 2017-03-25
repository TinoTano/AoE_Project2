#include "Building.h"
#include "Render.h"
#include "Application.h"
#include "Map.h"
#include "Textures.h"
#include "Unit.h"
#include "EntityManager.h"
#include "Collision.h"
#include "p2Log.h"
#include "math.h"



Building::Building()
{
}

Building::Building(int posX, int posY, bool isEnemy, Building* building)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->isEnemy = isEnemy;
	this->type = type;

	faction = building->faction;
	buildingAttackSpeed = building->buildingAttackSpeed;
	buildingPiercingDamage = building->buildingPiercingDamage;
	buildingWoodCost = building->buildingWoodCost;
	buildingStoneCost = building->buildingStoneCost;
	buildingBuildTime = building->buildingBuildTime;
	buildingIdleTexture = building->buildingIdleTexture;
	buildingDieTexture = building->buildingDieTexture;
	Life = building->Life;
	MaxLife = building->MaxLife;
	Attack = building->Attack;
	Defense = building->Defense;
	canAttack = building->canAttack;

	entityTexture = buildingIdleTexture;

	App->tex->GetSize(buildingIdleTexture, imageWidth, imageHeight);
	SDL_Rect colliderRect = { entityPosition.x - (imageWidth / 2), entityPosition.y - (imageHeight / 2), imageWidth, imageHeight};
	COLLIDER_TYPE colliderType;
	if (isEnemy) {
		colliderType = COLLIDER_ENEMY_BUILDING;
	}
	else {
		colliderType = COLLIDER_FRIENDLY_BUILDING;
	}
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager, (Entity*)this);

	isSelected = false;
	/*if (!isEnemy) {
		isVisible = true;
	}
	else {
		isVisible = false;
	}*/

	hpBarWidth = 50;
}

Building::~Building()
{
}

bool Building::Update(float dt)
{
	switch (state) {
	case BUILDING_ATTACKING:
		AttackEnemy(dt);
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

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		int x;
		int y;
		App->input->GetMousePosition(x, y);
		if (x < entityPosition.x + (collider->rect.w / 2) && x > entityPosition.x - (collider->rect.w / 2) &&
			y < entityPosition.y + (collider->rect.h / 2) && y > entityPosition.y - (collider->rect.h / 2)) {
			if (isVisible) {
				isSelected = true;
			}
		}
		else {
			if (isSelected) {
				isSelected = false;
			}
		}
	}

	return true;
}

bool Building::Draw()
{
	if (isVisible) {
		if (isSelected) {
			App->render->Blit(entityTexture, entityPosition.x - ((int)imageWidth / 2), entityPosition.y - ((int)imageHeight / 2));
			int percent = ((MaxLife - Life) * 100) / MaxLife;
			int barPercent = (percent * hpBarWidth) / 100;
			App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)imageHeight / 2), hpBarWidth, 5 }, 255, 0, 0);
			App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)imageHeight / 2), min(hpBarWidth, max(hpBarWidth - barPercent, 0)), 5 }, 0, 255, 0);
		}
		else {
			App->render->Blit(entityTexture, entityPosition.x - ((int)imageWidth / 2), entityPosition.y - ((int)imageHeight / 2));
		}
	}
	
	return true;
}

void Building::AttackEnemy(float dt)
{
	if (timer >= buildingAttackSpeed) {
		attackTarget->Life -= Attack - attackTarget->Defense;
		if (attackTarget->Life <= 0) {
			attackTarget->Dead();
			if (Life > 0) {
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

pugi::xml_node Building::LoadBuildingInfo(buildingType type)
{
	
	return pugi::xml_node();
}

bool Building::Load(pugi::xml_node &)
{
	return true;
}

bool Building::Save(pugi::xml_node &) const
{
	return true;
}

