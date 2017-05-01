#include "Building.h"
#include "Render.h"
#include "Application.h"
#include "Map.h"
#include "Textures.h"
#include "Unit.h"
#include "EntityManager.h"
#include "Collision.h"
#include "p2Log.h"
#include "Orders.h"
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

	if (Life == -1) {
		App->collision->DeleteCollider(collider);
		App->collision->DeleteCollider(range);
		state = DESTROYED;
		App->entityManager->DeleteBuilding(this);
		return false;
	}

	if (state != BEING_BUILT) {

		if (!order_list.empty()) {
			Order* current_order = order_list.front();

			if (current_order->state == NEEDS_START)
				current_order->Start((Entity*)this);

			if (current_order->state == EXECUTING)
				current_order->Execute();

			if (current_order->state == COMPLETED)
				order_list.pop_front();
		}
		else {
			if (state != IDLE)
				state = IDLE;
		}
	}

	if (Life < MaxLife / 2) {
		//blit fire animation
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

	/*if (isSelected)
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
	}*/

	App->render->sprites_toDraw.push_back(aux);
	

	return true;
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

