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

Unit::Unit(int posX, int posY, Unit* unit)
{
	entityPosition.x = posX;
	entityPosition.y = posY;
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
	Life = unit->Life;
	MaxLife = unit->MaxLife;
	Attack = unit->Attack;
	Defense = unit->Defense;

	//Animations
	idleAnimations = unit->idleAnimations;
	movingAnimations = unit->movingAnimations;
	attackingAnimations = unit->attackingAnimations;
	dyingAnimations = unit->dyingAnimations;

	entityTexture = unitIdleTexture;

	SetAnim(currentDirection);

	SDL_Rect r = currentAnim->GetCurrentFrame();
	COLLIDER_TYPE colliderType;
	colliderType = COLLIDER_UNIT;
	

	uint w = 0, h = 0;

	collider = App->collision->AddCollider(entityPosition, r.w / 4, colliderType, App->entityManager, (Entity*)this);

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
		if (attackTarget != nullptr) {
			AttackEnemy(dt);
		}
		break;
	case UNIT_DEAD:
		if (currentAnim->Finished()) {
			App->entityManager->DeleteUnit(this);
		}
		break;
	}

	return true;
}

bool Unit::Draw()
{
	if (isVisible) {
		SDL_Rect r = currentAnim->GetCurrentFrame();
		iPoint col_pos;
		if(state == UNIT_MOVING)
			col_pos.create(next_step.x, next_step.y + (r.h / 2));    // an offset var in collider should be implemented for big units
		else
			col_pos.create(entityPosition.x, entityPosition.y + (r.h / 2));

		collider->pos = col_pos;
		App->render->Blit(entityTexture, entityPosition.x - (r.w / 2), entityPosition.y - (r.h / 2), &r, currentAnim->flip);
		
	}
	
	return true;
}

unitType Unit::GetType() const
{
	return type;
}

bool Unit::IsEnemy() const
{
	return (bool)faction;
}

int Unit::GetLife() const
{
	return Life;
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

	if (path != nullptr) {
		App->pathfinding->DeletePath(path);
		path = nullptr;
	}

	iPoint origin = App->map->WorldToMap(entityPosition.x, entityPosition.y);
	path = App->pathfinding->CreatePath(origin, destination);

	SetState(UNIT_MOVING);

	destinationTileWorld = App->map->MapToWorld(path->front().x, path->front().y);
	path->erase(path->begin());
	
	if (attackTarget != nullptr) 
		attackTarget = nullptr;
	
}

void Unit::Move(float dt)
{
	entityPosition = next_step;

	if (entityPosition.DistanceNoSqrt(destinationTileWorld) < 1) {
		if (path->size() > 0) {
			destinationTileWorld = App->map->MapToWorld(path->front().x, path->front().y);
			path->erase(path->begin());
		}
		else {
			App->pathfinding->DeletePath(path);
			SetState(UNIT_IDLE);
		}
	}

	CalculateVelocity();
	LookAt();

	fPoint vel = (velocity * (unitMovementSpeed + 100)) * dt;
	roundf(vel.x);
	roundf(vel.y);

	next_step.x = entityPosition.x + int(vel.x);
	next_step.y = entityPosition.y + int(vel.y);

}


void Unit::CalculateVelocity()
{

	velocity.x = destinationTileWorld.x - entityPosition.x;
	velocity.y = destinationTileWorld.y - entityPosition.y;

	if(velocity.x != 0 || velocity.y != 0)
		velocity.Normalize();
}

void Unit::LookAt()
{

	if (state == UNIT_ATTACKING)
	{
		if (attackTarget != nullptr)
		{
			velocity.x = attackTarget->entityPosition.x - entityPosition.x;
			velocity.y = attackTarget->entityPosition.y - entityPosition.y;
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

void Unit::AttackEnemy(float dt)
{
	LookAt();
	if (timer >= attackSpeed) {
		attackTarget->Life -= Attack - attackTarget->Defense;
		if (attackTarget->Life <= 0) {
			attackTarget->Dead();
			if (Life > 0) {
				SetState(UNIT_IDLE);
				attackTarget = nullptr;
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
		next_step = entityPosition;
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
