#include "Unit.h"
#include "Render.h"
#include "Application.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "p2Log.h"
#include "math.h"
#include "Map.h"
#include "Application.h"
#include "Collision.h"
#include "Textures.h"
#include "p2Defs.h"
#include "Scene.h"
#include "Gui.h"

Unit::Unit()
{
}

Unit::Unit(int posX, int posY, bool isEnemy, Unit* unit)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
	this->isEnemy = isEnemy;
	this->type = unit->type;
	faction = unit->faction;
	direction = unit->direction;
	unitAttackSpeed = unit->attackSpeed;
	unitPiercingDamage = unit->unitPiercingDamage;
	unitMovementSpeed = unit->unitMovementSpeed;
	attackSpeed = unit->unitAttackSpeed;
	currentDirection = unit->currentDirection;
	unitIdleTexture = unit->unitIdleTexture;
	unitMoveTexture = unit->unitMoveTexture;
	unitAttackTexture = unit->unitAttackTexture;
	unitDieTexture = unit->unitDieTexture;
	unitLife = unit->unitLife;
	unitMaxLife = unit->unitMaxLife;
	unitAttack = unit->unitAttack;
	unitDefense = unit->unitDefense;

	//Animations
	idleAnimations = unit->idleAnimations;
	movingAnimations = unit->movingAnimations;
	attackingAnimations = unit->attackingAnimations;
	dyingAnimations = unit->dyingAnimations;

	entityTexture = unitIdleTexture;

	SetAnim(currentDirection);

	SDL_Rect r = currentAnim->GetCurrentFrame();
	SDL_Rect colliderRect = { entityPosition.x - (r.w / 4), entityPosition.y - (r.h / 3), r.w / 2, r.h / 1.25f};
	COLLIDER_TYPE colliderType;
	if (isEnemy) {
		colliderType = COLLIDER_ENEMY_UNIT;
	}
	else {
		colliderType = COLLIDER_FRIENDLY_UNIT;
	}
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager);

	/*if (!isEnemy) {
		isVisible = true;
	}
	else {
		isVisible = false;
	}*/

}

Unit::~Unit()
{
}

bool Unit::Update(float dt)
{
	switch (state) {
	case UNIT_MOVING:
		Move(dt);
		break;
	case UNIT_ATTACKING:
		if (attackUnitTarget != nullptr) {
			AttackEnemyUnit(dt);
		}
		if (attackBuildingTarget != nullptr) {
			AttackEnemyBuilding(dt);
		}
		break;
	case UNIT_DEAD:
		if (currentAnim->Finished()) {
			App->entityManager->DeleteUnit(this, isEnemy);
		}
		break;
	}

	return true;
}

bool Unit::Draw()
{
	if (isVisible) {
		SDL_Rect r = currentAnim->GetCurrentFrame();
		collider->rect.x = entityPosition.x - (r.w / 4);
		collider->rect.y = entityPosition.y - (r.h / 3);

		if (isSelected) {
			int percent = ((unitMaxLife - unitLife) * 100) / unitMaxLife;
			int barPercent = (percent * hpBarWidth) / 100;
			App->render->DrawCircle(entityPosition.x, entityPosition.y + (r.h / 2), 15, 255, 255, 255, 255);
			App->render->Blit(entityTexture, entityPosition.x - (r.w / 2), entityPosition.y - (r.h / 2), &r, currentAnim->flip);
			App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)(collider->rect.h / 1.5f)), hpBarWidth, 5 }, 255, 0, 0);
			App->render->DrawQuad({ entityPosition.x - (hpBarWidth / 2), entityPosition.y - ((int)(collider->rect.h / 1.5f)), min(hpBarWidth, max(hpBarWidth - barPercent , 0)), 5 }, 0, 255, 0);
		}
		else {
			App->render->Blit(entityTexture, entityPosition.x - (r.w / 2), entityPosition.y - (r.h / 2), &r, currentAnim->flip);
		}
	}
	
	return true;
}

unitType Unit::GetType() const
{
	return type;
}

int Unit::GetLife() const
{
	return unitLife;
}

void Unit::SetPos(int posX, int posY)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
}

void Unit::SetSpeed(int amount)
{
	unitMovementSpeed = amount;
}

void Unit::SetDestination(iPoint destination)
{

	iPoint origin = App->map->WorldToMap(entityPosition.x, entityPosition.y);
	App->pathfinding->CreatePath(origin, destination, path);

	if (path.size() > 0) {
		SetState(UNIT_MOVING);
		destinationReached = false;
		if (path.front() == origin) {
			if (path.size() > 1) {
				destinationTile = path.begin()._Ptr->_Next->_Myval;
				path.remove(path.begin()._Ptr->_Next->_Myval);
			}
		}
		else {
			destinationTile = path.front();
		}
		path.erase(path.begin());
	}
	if (attackUnitTarget != nullptr) {
		attackUnitTarget = nullptr;
	}
}

void Unit::Move(float dt)
{
	CalculateVelocity();
	LookAt();

	if (!destinationReached) {

		fPoint vel = (velocity * (unitMovementSpeed + 100)) * dt;
		roundf(vel.x);
		roundf(vel.y);

		if (vel.x > 5 || vel.y > 5)
			int a = 4;

		entityPosition.x += int(vel.x);
		entityPosition.y += int(vel.y);

		if (entityPosition.DistanceNoSqrt(destinationTileWorld) < 1) {
			if (path.size() > 0) {
				destinationTile = path.front();
				path.erase(path.begin());
				LOG("%d %d", destinationTile.x, destinationTile.y);
			}
			else {
				destinationReached = true;
				SetState(UNIT_IDLE);
			}
		}
	}
}

void Unit::CalculateVelocity()
{

	destinationTileWorld = App->map->MapToWorld(destinationTile.x + 1, destinationTile.y);
	velocity.x = destinationTileWorld.x - entityPosition.x;
	velocity.y = destinationTileWorld.y - entityPosition.y;

	if(velocity.x != 0 || velocity.y != 0)
		velocity.Normalize();
}

void Unit::LookAt()
{

	if (state == UNIT_ATTACKING)
	{
		if (attackUnitTarget != nullptr)
		{
			velocity.x = attackUnitTarget->entityPosition.x - entityPosition.x;
			velocity.y = attackUnitTarget->entityPosition.y - entityPosition.y;
		}
		if (attackBuildingTarget != nullptr) {
			velocity.x = attackBuildingTarget->entityPosition.x - entityPosition.x;
			velocity.y = attackBuildingTarget->entityPosition.y - entityPosition.y;
		}
		velocity.Normalize();
	}

	float angle = atan2f(velocity.y, velocity.x) * RADTODEG;

	if (angle < 22.5 && angle > -22.5)
		direction = RIGHT;
	else if (angle >= 22.5 && angle <= 67.5)
		direction = DOWN_RIGHT;
	else if (angle > 67.5 && angle < 112.5)
		direction = DOWN;
	else if (angle >= 112.5 && angle <= 157.5)
		direction = DOWN_LEFT;
	else if (angle > 157.5 || angle < -157.5)
		direction = LEFT;
	else if (angle >= -157.5 && angle <= -112.5)
		direction = UP_LEFT;
	else if (angle > -112.5 && angle < -67.5)
		direction = UP;
	else if (angle >= -67.5 && angle <= -22.5)
		direction = UP_RIGHT;

	if (direction != currentDirection)
	{
		currentDirection = direction;
		SetAnim(currentDirection);
	}
}

void Unit::AttackEnemyUnit(float dt)
{
	LookAt();
	if (timer >= attackSpeed) {
		attackUnitTarget->unitLife -= unitAttack - attackUnitTarget->unitDefense;
		if (attackUnitTarget->unitLife <= 0) {
			attackUnitTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackUnitTarget = nullptr;
			}
		}
		timer = 0;
	}
	else {
		timer += dt;
	}
}

void Unit::AttackEnemyBuilding(float dt)
{
	LookAt();
	if (timer >= attackSpeed) {
		attackBuildingTarget->buildingLife -= unitAttack - attackBuildingTarget->buildingDefense;
		if (attackBuildingTarget->buildingLife <= 0) {
			attackBuildingTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackBuildingTarget = nullptr;
			}
		}
		timer = 0;
	}
	else {
		timer += dt;
	}
}

void Unit::Dead() {
	SetState(UNIT_DEAD);
	App->collision->DeleteCollider(collider);
}

void Unit::SetState(unitState newState)
{
	switch (newState) {
	case UNIT_IDLE:
		this->state = UNIT_IDLE;
		SetAnim(currentDirection);
		entityTexture = unitIdleTexture;
		break;
	case UNIT_MOVING:
		this->state = UNIT_MOVING;
		SetAnim(currentDirection);
		entityTexture = unitMoveTexture;
		break;
	case UNIT_ATTACKING:
		this->state = UNIT_ATTACKING;
		SetAnim(currentDirection);
		entityTexture = unitAttackTexture;
		break;
	case UNIT_DEAD:
		this->state = UNIT_DEAD;
		SetAnim(currentDirection);
		entityTexture = unitDieTexture;
		break;
	}
}

void Unit::SetAnim(unitDirection currentDirection) {

	switch (state) {
	case UNIT_IDLE:
		currentAnim = &idleAnimations[currentDirection];
		break;
	case UNIT_MOVING:
		currentAnim = &movingAnimations[currentDirection];
		break;
	case UNIT_ATTACKING:
		currentAnim = &attackingAnimations[currentDirection];
		break;
	case UNIT_DEAD:
		currentAnim = &dyingAnimations[currentDirection];
		break;
	}
}
