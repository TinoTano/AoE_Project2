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

Building::Building(int posX, int posY, Building* building)
{
	entityPosition.x = posX;
	entityPosition.y = posY;

	type = building->type;
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

	collider = App->collision->AddCollider(entityPosition, imageWidth / 2, COLLIDER_BUILDING, App->entityManager, (Entity*)this);
	range = App->collision->AddCollider(entityPosition, imageWidth, COLLIDER_RANGE, App->entityManager, (Entity*)this);

	hpBarWidth = 50;
	attack_timer.Start();
}

Building::~Building()
{}

bool Building::IsEnemy() const
{
	return (bool)faction;
}

bool Building::Update(float dt)
{
	iPoint mouse;
	App->input->GetMousePosition(mouse.x, mouse.x);
	mouse.x -= App->render->camera.x;
	mouse.y -= App->render->camera.y;

	switch (state) {
	case BUILDING_ATTACKING:
		AttackEnemy(dt);
		break;
	case BUILDING_DESTROYING:
		//if (currentAnim->Finished()) {
		App->entityManager->DeleteBuilding(this);
		//}
		break;
	}

	if (Life < MaxLife / 2) {
		//blit fire animation
	}

	
	/*if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

		if (mouse.y > NOTHUD.y - CAMERA_OFFSET_Y && mouse.y < NOTHUD.h - CAMERA_OFFSET_Y) {

			if (collider->pos.DistanceTo(mouse) < collider->r)
				App->entityManager->selectedBuildingList.push_back(this);

		}
	}*/

	if (onConstruction && Life >= MaxLife) {
		isSelected = false;
		onConstruction = false;
	}

	return true;
}

bool Building::Draw()
{

	Sprite aux;

	aux.texture = entityTexture;
	aux.pos.x = entityPosition.x - (imageWidth / 2);
	aux.pos.y = entityPosition.y - (imageHeight / 2);
	aux.priority = entityPosition.y - (imageHeight / 2) + imageHeight;
	aux.rect.w = imageWidth;
	aux.rect.h = imageHeight;

	if (isSelected)
	{
		Sprite bar;

		int percent = ((MaxLife - Life) * 100) / MaxLife;
		int barPercent = (percent * hpBarWidth) / 100;

		bar.rect.x = entityPosition.x - (hpBarWidth / 2);
		bar.rect.y = entityPosition.y - collider->r;
		bar.rect.w = hpBarWidth;
		bar.rect.h = 5;
		bar.priority = entityPosition.y - (imageHeight / 2) + imageHeight + 10;
		bar.r = 255;
		bar.g = 0;
		bar.b = 0;

		App->render->sprites_toDraw.push_back(bar);

		Sprite bar2;

		bar2.rect.x = entityPosition.x - (hpBarWidth / 2);
		bar2.rect.y = entityPosition.y - collider->r;
		bar2.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
		bar2.rect.h = 5;
		bar2.priority = entityPosition.y - (imageHeight / 2) + imageHeight + 11;
		bar2.r = 0;
		bar2.g = 255;
		bar2.b = 0;

		App->render->sprites_toDraw.push_back(bar2);
	}

	App->render->sprites_toDraw.push_back(aux);
	

	return true;
}

void Building::AttackEnemy(float dt)
{
	if (attack_timer.ReadSec() >= buildingAttackSpeed) {
		attackTarget->Life -= Attack - attackTarget->Defense;
		if (attackTarget->Life <= 0) {
			attackTarget->Dead();
			if (Life > 0) {
				state = BUILDING_IDLE;
			}
		}
		attack_timer.Start();
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

