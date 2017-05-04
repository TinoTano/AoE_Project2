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
	los = App->collision->AddCollider(entityPosition, imageWidth * 1.5, COLLIDER_LOS, App->entityManager, (Entity*)this);

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

	if (state != BEING_BUILT && state != DESTROYED) {

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

	App->render->sprites_toDraw.push_back(aux);
	
	if (last_life != Life) {
		lifebar_timer.Start();
		last_life = Life;
	}

	if (lifebar_timer.ReadSec() < 5) {
		iPoint p( entityPosition.x - 25, entityPosition.y - (imageHeight / 2) );
		drawLife(p);
	}

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

