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

Building::Building(int posX, int posY, bool isEnemy, buildingType type)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->isEnemy = isEnemy;
	this->type = type;

	pugi::xml_document gameDataFile;
	pugi::xml_node gameData;
	pugi::xml_node buildingNodeInfo;

	gameData = App->LoadGameData(gameDataFile);

	if (gameData.empty() == false)
	{
		pugi::xml_node building;

		for (building = gameData.child("Buildings").child("Building"); building; building = building.next_sibling("Building")) {
			LOG("%d", building.child("Info").child("ID").attribute("value").as_int());
			if (building.child("Info").child("ID").attribute("value").as_int() == type) {
				buildingNodeInfo = building;
			}
		}
	}

	string idleTexturePath = buildingNodeInfo.child("Textures").child("Idle").attribute("value").as_string();
	string dieTexturePath = buildingNodeInfo.child("Textures").child("Die").attribute("value").as_string();

	//faction = (buildingFaction)buildingNodeInfo.child("Stats").child("Faction").attribute("value").as_int();
	//buildingAttackSpeed = 1 / buildingNodeInfo.child("Stats").child("AttackSpeed").attribute("value").as_float();
	buildingLife = buildingNodeInfo.child("Stats").child("Life").attribute("value").as_int();
	buildingMaxLife = buildingLife;
	//buildingAttack = buildingNodeInfo.child("Stats").child("Attack").attribute("value").as_int();
	//buildingDefense = buildingNodeInfo.child("Stats").child("Defense").attribute("value").as_int();
	//buildingPiercingDamage = buildingNodeInfo.child("Stats").child("PiercingDamage").attribute("value").as_int();
	buildingWoodCost = buildingNodeInfo.child("Stats").child("WoodCost").attribute("value").as_int();
	buildingStoneCost = buildingNodeInfo.child("Stats").child("StoneCost").attribute("value").as_int();
	buildingBuildTime = buildingNodeInfo.child("Stats").child("BuildTime").attribute("value").as_int();
	canAttack = buildingNodeInfo.child("Stats").child("CanAttack").attribute("value").as_bool();

	buildingIdleTexture = App->tex->Load(idleTexturePath.c_str());
	buildingDieTexture = App->tex->Load(dieTexturePath.c_str());

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
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager);

	isSelected = false;
	if (!isEnemy) {
		isVisible = true;
	}
	else {
		isVisible = false;
	}

	hpBarWidth = 50;
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
	
	if (isSelected) {
		App->render->Blit(entityTexture, entityPosition.x - ((int)imageWidth / 2), entityPosition.y - ((int)imageHeight / 2));
		int percent = ((buildingMaxLife - buildingLife) * 100) / buildingMaxLife;
		int barPercent = (percent * hpBarWidth) / 100;
		App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)imageHeight / 2), hpBarWidth, 5 }, 255, 0, 0);
		App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)imageHeight / 2), min(hpBarWidth, max(hpBarWidth - barPercent, 0)), 5 }, 0, 255, 0);
	}
	else {
		App->render->Blit(entityTexture, entityPosition.x - ((int)imageWidth / 2), entityPosition.y - ((int)imageHeight / 2));
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

