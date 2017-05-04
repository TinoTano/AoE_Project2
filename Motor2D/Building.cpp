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
	constructingPhase1 = building->constructingPhase1;
	constructingPhase2 = building->constructingPhase2;
	constructingPhase3 = building->constructingPhase3;
	Life = building->Life;
	MaxLife = building->MaxLife;
	Attack = building->Attack;
	Defense = building->Defense;
	canAttack = building->canAttack;

	entityTexture = buildingIdleTexture;
	GetBuildingBoundaries();

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

void Building::GetBuildingBoundaries()
{
	App->tex->GetSize(entityTexture, imageWidth, imageHeight);
}

bool Building::Update(float dt)
{

	if (Life == -1) {
		Destroy();
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

	if (state == BEING_BUILT) {
		if (Life > MaxLife / 1.5f) {
			entityTexture = constructingPhase3;
			GetBuildingBoundaries();
		}
		else if (Life > MaxLife / 3) {
			entityTexture = constructingPhase2;
			GetBuildingBoundaries();
		}
	}

	if (waitingToPlace) {
		if (collider->colliding) {
			SDL_SetTextureColorMod(entityTexture, 255, 0, 0);
			canBePlaced = false;
		}
		else {
			SDL_SetTextureColorMod(entityTexture, 255, 255, 255);
			canBePlaced = true;
		}
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
		iPoint p(entityPosition.x - 25, entityPosition.y - (imageHeight / 2));
		drawLife(p);
	}

	return true;
}

void Building::Destroy() {

	App->collision->DeleteCollider(collider);
	App->collision->DeleteCollider(range);
	App->collision->DeleteCollider(los);
	state = DESTROYED;
	App->entityManager->DeleteBuilding(this);

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