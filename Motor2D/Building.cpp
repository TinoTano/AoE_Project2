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
	this->type = building->type;

	faction = building->faction;
	buildingAttackSpeed = building->buildingAttackSpeed;
	buildingPiercingDamage = building->buildingPiercingDamage;
	buildingWoodCost = building->buildingWoodCost;
	buildingStoneCost = building->buildingStoneCost;
	buildingBuildTime = building->buildingBuildTime;
	buildingIdleTexture = building->buildingIdleTexture;
	buildingDieTexture = building->buildingDieTexture;
	buildingLife = building->buildingLife;
	buildingMaxLife = building->buildingMaxLife;
	buildingAttack = building->buildingAttack;
	buildingDefense = building->buildingDefense;
	canAttack = building->canAttack;

	entityTexture = buildingIdleTexture;

	App->tex->GetSize(buildingIdleTexture, imageWidth, imageHeight);
	SDL_Rect colliderRect;
	if (type == SAURON_TOWER) {
		colliderRect = { entityPosition.x - ((int)imageWidth / 2), entityPosition.y + ((int)imageHeight / 4), (int)imageWidth, 150 };
	}
	else {
		colliderRect = { entityPosition.x - ((int)imageWidth / 2), entityPosition.y - ((int)imageHeight / 2), (int)imageWidth, (int)imageHeight };
	}
	COLLIDER_TYPE colliderType;
	if (isEnemy) {
		colliderType = COLLIDER_ENEMY_BUILDING;
	}
	else {
		colliderType = COLLIDER_FRIENDLY_BUILDING;
	}
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager);

	isSelected = false;
	hpBarWidth = 50;
}

Building::~Building()
{

}

bool Building::Update(float dt)
{
	App->input->GetMousePosition(mouseX, mouseY);
	mouseX -= App->render->camera.x;
	mouseY -= App->render->camera.y;

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
	if (mouseY > NOTHUD.y - CAMERA_OFFSET_Y && mouseY < NOTHUD.h - CAMERA_OFFSET_Y)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			int x;
			int y;
			App->input->GetMousePosition(x, y);
			if (x < entityPosition.x + (collider->rect.w / 2) && x > entityPosition.x - (collider->rect.w / 2) &&
				y < entityPosition.y + (collider->rect.h / 2) && y > entityPosition.y - (collider->rect.h / 2)) {
				if (isVisible) {
					isSelected = true;
					App->entityManager->selectedBuildingList.push_back(this);
				}
			}
			else {
				if (isSelected) {
					isSelected = false;
					App->entityManager->selectedBuildingList.remove(this);
				}
			}
		}
	}

	SDL_Rect cam = App->render->culling_cam;

	if (entityPosition.x >= cam.x && entityPosition.x <= cam.x + cam.w && entityPosition.y + collider->rect.h > cam.y && entityPosition.y < cam.y + cam.h) {
		isVisible = true;
	}
	else {
		isVisible = false;
	}
	if (isVisible) {
		Draw();
	}

	return true;
}

bool Building::Draw()
{
	if (isVisible)
	{
		Sprite aux;

		aux.texture = entityTexture;
		aux.pos.x = entityPosition.x - (imageWidth / 2);
		aux.pos.y = entityPosition.y - (imageHeight / 2);
		aux.priority = entityPosition.y - (imageHeight / 2) + imageHeight;
		aux.rect.w = imageWidth;
		aux.rect.h = imageHeight;
		aux.flip = SDL_FLIP_HORIZONTAL;

		if (isSelected)
		{
			Sprite bar;

			int percent = ((buildingMaxLife - buildingLife) * 100) / buildingMaxLife;
			int barPercent = (percent * hpBarWidth) / 100;

			bar.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = entityPosition.y - ((int)(collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = entityPosition.y - (imageHeight / 2) + imageHeight;
			bar.r = 255;

			App->render->sprites_toDraw.push_back(bar);

			bar.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = entityPosition.y - ((int)(collider->rect.h / 1.5f));
			bar.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar.rect.h = 5;
			bar.priority = entityPosition.y - (imageHeight / 2) + imageHeight;
			bar.r = 0;
			bar.g = 255;

			App->render->sprites_toDraw.push_back(bar);
		}

		App->render->sprites_toDraw.push_back(aux);
	}

	
	return true;
}

void Building::Attack(float dt)
{
	if (timer >= buildingAttackSpeed) {
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

