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

Unit::Unit(int posX, int posY, bool isEnemy, unitType type, unitFaction race)
{
	entityPosition = App->map->MapToWorld(posX, posY);
	this->isEnemy = isEnemy;
	this->type = type;
	this->race = race;

	entityPosition.x -= App->map->data.tile_width / 2;
	entityPosition.y -= App->map->data.tile_height / 2;

	//Provisional
	switch (race) 
	{
	case FREE_MEN:
		switch (type)
		{
		case ELVEN_ARCHER:
			unitIdleTexture = App->tex->Load("textures/ElvenArcherIdle.png");
			unitMoveTexture = App->tex->Load("textures/ElvenArcherMove.png");
			unitAttackTexture = App->tex->Load("textures/ElvenArcherAttack.png");
			unitDieTexture = App->tex->Load("textures/ElvenArcherDie.png");
			entityTexture = unitIdleTexture;
			attackDelay = 1 / 2.2f;
			unitLife = 35;
			unitAttack = 5;
			unitDefense = 0;
			unitPiercingDamage = 1;
			unitMovementSpeed = 101.1f;

			//Anim Idle//
			//Anim idle down
			idleDown.PushBack({ 0,0,53,55 });
			idleDown.PushBack({ 53,0,53,55 });
			idleDown.PushBack({ 106,0,53,55 });
			idleDown.PushBack({ 159,0,53,55 });
			idleDown.PushBack({ 212,0,53,55 });
			idleDown.PushBack({ 265,0,53,55 });
			idleDown.PushBack({ 318,0,53,55 });
			idleDown.PushBack({ 371,0,53,55 });
			idleDown.PushBack({ 424,0,53,55 });
			idleDown.PushBack({ 477,0,53,55 });
			idleDown.speed = 0.2f;
			//Anim idle down-left
			idleDownLeft.PushBack({ 0,55,53,55 });
			idleDownLeft.PushBack({ 53,55,53,55 });
			idleDownLeft.PushBack({ 106,55,53,55 });
			idleDownLeft.PushBack({ 159,55,53,55 });
			idleDownLeft.PushBack({ 212,55,53,55 });
			idleDownLeft.PushBack({ 265,55,53,55 });
			idleDownLeft.PushBack({ 318,55,53,55 });
			idleDownLeft.PushBack({ 371,55,53,55 });
			idleDownLeft.PushBack({ 424,55,53,55 });
			idleDownLeft.PushBack({ 477,55,53,55 });
			idleDownLeft.speed = 0.2f;
			//Anim idle left
			idleLeft.PushBack({ 0,110,53,55 });
			idleLeft.PushBack({ 53,110,53,55 });
			idleLeft.PushBack({ 106,110,53,55 });
			idleLeft.PushBack({ 159,110,53,55 });
			idleLeft.PushBack({ 212,110,53,55 });
			idleLeft.PushBack({ 265,110,53,55 });
			idleLeft.PushBack({ 318,110,53,55 });
			idleLeft.PushBack({ 371,110,53,55 });
			idleLeft.PushBack({ 424,110,53,55 });
			idleLeft.PushBack({ 477,110,53,55 });
			idleLeft.speed = 0.2f;
			//Anim idle up-left
			idleUpLeft.PushBack({ 0,165,53,55 });
			idleUpLeft.PushBack({ 53,165,53,55 });
			idleUpLeft.PushBack({ 106,165,53,55 });
			idleUpLeft.PushBack({ 159,165,53,55 });
			idleUpLeft.PushBack({ 212,165,53,55 });
			idleUpLeft.PushBack({ 265,165,53,55 });
			idleUpLeft.PushBack({ 318,165,53,55 });
			idleUpLeft.PushBack({ 371,165,53,55 });
			idleUpLeft.PushBack({ 424,165,53,55 });
			idleUpLeft.PushBack({ 477,165,53,55 });
			idleUpLeft.speed = 0.2f;
			//Anim idle up
			idleUp.PushBack({ 53,220,53,55 });
			idleUp.PushBack({ 106,220,53,55 });
			idleUp.PushBack({ 159,220,53,55 });
			idleUp.PushBack({ 212,220,53,55 });
			idleUp.PushBack({ 265,220,53,55 });
			idleUp.PushBack({ 318,220,53,55 });
			idleUp.PushBack({ 371,220,53,55 });
			idleUp.PushBack({ 424,220,53,55 });
			idleUp.PushBack({ 477,220,53,55 });
			idleUp.PushBack({ 0,275,53,55 });
			idleUp.speed = 0.2f;
			//Anim idle down-right
			idleDownRight = idleDownLeft;
			idleDownRight.flipAnim = true;
			//Anim idle right
			idleRight = idleLeft;
			idleRight.flipAnim = true;
			//Anim idle up-right
			idleUpRight = idleUpLeft;
			idleUpRight.flipAnim = true;

			//Anim Move//
			//Anim moving down
			movingDown.PushBack({ 0,0,57,58 });
			movingDown.PushBack({ 57,0,57,58 });
			movingDown.PushBack({ 114,0,57,58 });
			movingDown.PushBack({ 171,0,57,58 });
			movingDown.PushBack({ 228,0,57,58 });
			movingDown.PushBack({ 285,0,57,58 });
			movingDown.PushBack({ 342,0,57,58 });
			movingDown.PushBack({ 399,0,57,58 });
			movingDown.PushBack({ 456,0,57,58 });
			movingDown.PushBack({ 513,0,57,58 });
			movingDown.speed = 0.2f;
			//Anim moving down-left
			movingDownLeft.PushBack({ 0,58,57,58 });
			movingDownLeft.PushBack({ 57,58,57,58 });
			movingDownLeft.PushBack({ 114,58,57,58 });
			movingDownLeft.PushBack({ 171,58,57,58 });
			movingDownLeft.PushBack({ 228,58,57,58 });
			movingDownLeft.PushBack({ 285,58,57,58 });
			movingDownLeft.PushBack({ 342,58,57,58 });
			movingDownLeft.PushBack({ 399,58,57,58 });
			movingDownLeft.PushBack({ 456,58,57,58 });
			movingDownLeft.PushBack({ 513,58,57,58 });
			movingDownLeft.speed = 0.2f;
			//Anim moving left
			movingLeft.PushBack({ 0,116,57,58 });
			movingLeft.PushBack({ 57,116,57,58 });
			movingLeft.PushBack({ 114,116,57,58 });
			movingLeft.PushBack({ 171,116,57,58 });
			movingLeft.PushBack({ 228,116,57,58 });
			movingLeft.PushBack({ 285,116,57,58 });
			movingLeft.PushBack({ 342,116,57,58 });
			movingLeft.PushBack({ 399,116,57,58 });
			movingLeft.PushBack({ 456,116,57,58 });
			movingLeft.PushBack({ 513,116,57,58 });
			movingLeft.speed = 0.2f;
			//Anim moving up-left
			movingUpLeft.PushBack({ 0,174,57,58 });
			movingUpLeft.PushBack({ 57,174,57,58 });
			movingUpLeft.PushBack({ 114,174,57,58 });
			movingUpLeft.PushBack({ 171,174,57,58 });
			movingUpLeft.PushBack({ 228,174,57,58 });
			movingUpLeft.PushBack({ 285,174,57,58 });
			movingUpLeft.PushBack({ 342,174,57,58 });
			movingUpLeft.PushBack({ 399,174,57,58 });
			movingUpLeft.PushBack({ 456,174,57,58 });
			movingUpLeft.PushBack({ 513,174,57,58 });
			movingUpLeft.speed = 0.2f;
			//Anim moving up
			movingUp.PushBack({ 0,232,57,58 });
			movingUp.PushBack({ 57,232,57,58 });
			movingUp.PushBack({ 114,232,57,58 });
			movingUp.PushBack({ 171,232,57,58 });
			movingUp.PushBack({ 228,232,57,58 });
			movingUp.PushBack({ 285,232,57,58 });
			movingUp.PushBack({ 342,232,57,58 });
			movingUp.PushBack({ 399,232,57,58 });
			movingUp.PushBack({ 456,232,57,58 });
			movingUp.PushBack({ 513,232,57,58 });
			movingUp.speed = 0.2f;
			//Anim moving down-right
			movingDownRight = movingDownLeft;
			movingDownRight.flipAnim = true;
			//Anim moving right
			movingRight = movingLeft;
			movingRight.flipAnim = true;
			//Anim moving up-right
			movingUpRight = movingUpLeft;
			movingUpRight.flipAnim = true;
			
			//Anim Attack//
			//Anim attacking down
			attackingDown.PushBack({ 0,0,62,74 });
			attackingDown.PushBack({ 62,0,62,74 });
			attackingDown.PushBack({ 124,0,62,74 });
			attackingDown.PushBack({ 186,0,62,74 });
			attackingDown.PushBack({ 248,0,62,74 });
			attackingDown.PushBack({ 310,0,62,74 });
			attackingDown.PushBack({ 372,0,62,74 });
			attackingDown.PushBack({ 434,0,62,74 });
			attackingDown.PushBack({ 496,0,62,74 });
			attackingDown.PushBack({ 558,0,62,74 });
			attackingDown.speed = 0.2f;
			//Anim attacking down-left
			attackingDownLeft.PushBack({ 0,74,62,74 });
			attackingDownLeft.PushBack({ 62,74,62,74 });
			attackingDownLeft.PushBack({ 124,74,62,74 });
			attackingDownLeft.PushBack({ 186,74,62,74 });
			attackingDownLeft.PushBack({ 248,74,62,74 });
			attackingDownLeft.PushBack({ 310,74,62,74 });
			attackingDownLeft.PushBack({ 372,74,62,74 });
			attackingDownLeft.PushBack({ 434,74,62,74 });
			attackingDownLeft.PushBack({ 496,74,62,74 });
			attackingDownLeft.PushBack({ 558,74,62,74 });
			attackingDownLeft.speed = 0.2f;
			//Anim attacking left
			attackingLeft.PushBack({ 0,148,62,74 });
			attackingLeft.PushBack({ 62,148,62,74 });
			attackingLeft.PushBack({ 124,148,62,74 });
			attackingLeft.PushBack({ 186,148,62,74 });
			attackingLeft.PushBack({ 248,148,62,74 });
			attackingLeft.PushBack({ 310,148,62,74 });
			attackingLeft.PushBack({ 372,148,62,74 });
			attackingLeft.PushBack({ 434,148,62,74 });
			attackingLeft.PushBack({ 496,148,62,74 });
			attackingLeft.PushBack({ 558,148,62,74 });
			attackingLeft.speed = 0.2f;
			//Anim attacking up-left
			attackingUpLeft.PushBack({ 0,222,62,74 });
			attackingUpLeft.PushBack({ 62,222,62,74 });
			attackingUpLeft.PushBack({ 124,222,62,74 });
			attackingUpLeft.PushBack({ 186,222,62,74 });
			attackingUpLeft.PushBack({ 248,222,62,74 });
			attackingUpLeft.PushBack({ 310,222,62,74 });
			attackingUpLeft.PushBack({ 372,222,62,74 });
			attackingUpLeft.PushBack({ 434,222,62,74 });
			attackingUpLeft.PushBack({ 496,222,62,74 });
			attackingUpLeft.PushBack({ 558,222,62,74 });
			attackingUpLeft.speed = 0.2f;
			//Anim attacking up
			attackingUp.PushBack({ 0,296,62,74 });
			attackingUp.PushBack({ 62,296,62,74 });
			attackingUp.PushBack({ 124,296,62,74 });
			attackingUp.PushBack({ 186,296,62,74 });
			attackingUp.PushBack({ 248,296,62,74 });
			attackingUp.PushBack({ 310,296,62,74 });
			attackingUp.PushBack({ 372,296,62,74 });
			attackingUp.PushBack({ 434,296,62,74 });
			attackingUp.PushBack({ 496,296,62,74 });
			attackingUp.PushBack({ 558,296,62,74 });
			attackingUp.speed = 0.2f;
			//Anim attacking down-right
			attackingDownRight = attackingDownLeft;
			attackingDownRight.flipAnim = true;
			//Anim attacking right
			attackingRight = attackingLeft;
			attackingRight.flipAnim = true;
			//Anim attacking up-right
			attackingUpRight = attackingUpLeft;
			attackingUpRight.flipAnim = true;

			//Anim Dead//
			//Anim dying down
			dyingDown.PushBack({ 0,0,142,107 });
			dyingDown.PushBack({ 142,0,142,107 });
			dyingDown.PushBack({ 284,0,142,107 });
			dyingDown.PushBack({ 426,0,142,107 });
			dyingDown.PushBack({ 568,0,142,107 });
			dyingDown.PushBack({ 710,0,142,107 });
			dyingDown.PushBack({ 852,0,142,107 });
			dyingDown.PushBack({ 994,0,142,107 });
			dyingDown.PushBack({ 1136,0,142,107 });
			dyingDown.PushBack({ 1278,0,142,107 });
			dyingDown.speed = 0.2f;
			dyingDown.loop = false;
			//Anim dying down-left
			dyingDownLeft.PushBack({ 0,107,142,107 });
			dyingDownLeft.PushBack({ 142,107,142,107 });
			dyingDownLeft.PushBack({ 284,107,142,107 });
			dyingDownLeft.PushBack({ 426,107,142,107 });
			dyingDownLeft.PushBack({ 568,107,142,107 });
			dyingDownLeft.PushBack({ 710,107,142,107 });
			dyingDownLeft.PushBack({ 852,107,142,107 });
			dyingDownLeft.PushBack({ 994,107,142,107 });
			dyingDownLeft.PushBack({ 1136,107,142,107 });
			dyingDownLeft.PushBack({ 1278,107,142,107 });
			dyingDownLeft.speed = 0.2f;
			dyingDownLeft.loop = false;
			//Anim dying left
			dyingLeft.PushBack({ 0,214,142,107 });
			dyingLeft.PushBack({ 142,214,142,107 });
			dyingLeft.PushBack({ 284,214,142,107 });
			dyingLeft.PushBack({ 426,214,142,107 });
			dyingLeft.PushBack({ 568,214,142,107 });
			dyingLeft.PushBack({ 710,214,142,107 });
			dyingLeft.PushBack({ 852,214,142,107 });
			dyingLeft.PushBack({ 994,214,142,107 });
			dyingLeft.PushBack({ 1136,214,142,107 });
			dyingLeft.PushBack({ 1278,214,142,107 });
			dyingLeft.speed = 0.2f;
			dyingLeft.loop = false;
			//Anim dying up-left
			dyingUpLeft.PushBack({ 0,321,142,107 });
			dyingUpLeft.PushBack({ 142,321,142,107 });
			dyingUpLeft.PushBack({ 284,321,142,107 });
			dyingUpLeft.PushBack({ 426,321,142,107 });
			dyingUpLeft.PushBack({ 568,321,142,107 });
			dyingUpLeft.PushBack({ 710,321,142,107 });
			dyingUpLeft.PushBack({ 852,321,142,107 });
			dyingUpLeft.PushBack({ 994,321,142,107 });
			dyingUpLeft.PushBack({ 1136,321,142,107 });
			dyingUpLeft.PushBack({ 1278,321,142,107 });
			dyingUpLeft.speed = 0.2f;
			dyingUpLeft.loop = false;
			//Anim dying up
			dyingUp.PushBack({ 0,428,142,107 });
			dyingUp.PushBack({ 142,428,142,107 });
			dyingUp.PushBack({ 284,428,142,107 });
			dyingUp.PushBack({ 426,428,142,107 });
			dyingUp.PushBack({ 568,428,142,107 });
			dyingUp.PushBack({ 710,428,142,107 });
			dyingUp.PushBack({ 852,428,142,107 });
			dyingUp.PushBack({ 994,428,142,107 });
			dyingUp.PushBack({ 1136,428,142,107 });
			dyingUp.PushBack({ 1278,428,142,107 });
			dyingUp.speed = 0.2f;
			dyingUp.loop = false;
			//Anim dying down-right
			dyingDownRight = dyingDownLeft;
			dyingDownRight.flipAnim = true;
			//Anim dying right
			dyingRight = dyingLeft;
			dyingRight.flipAnim = true;
			//Anim dying up-right
			dyingUpRight = dyingUpLeft;
			dyingUpRight.flipAnim = true;
			break;
		default:
			break;
		}
		break;
	case SAURON_ARMY:
		switch (type)
		{
		case ELVEN_ARCHER:
			break;
		default:
			break;
		}
		break;
	}

	SDL_Rect colliderRect = { entityPosition.x, entityPosition.y, 47, 50 }; // missing w and h of texture
	COLLIDER_TYPE colliderType;
	if (isEnemy) {
		colliderType = COLLIDER_ENEMY_UNIT;
	}
	else {
		colliderType = COLLIDER_FRIENDLY_UNIT;
	}
	collider = App->collision->AddCollider(colliderRect, colliderType, App->entityManager);

	currentDirection = RIGHT; // starting direction
	SetAnim(currentDirection);
}

Unit::~Unit()
{
}

bool Unit::Update(float dt)
{
	switch (state) {
	case MOVING:
		Move(dt);
		break;
	case ATTACKING:
		AttackEnemyUnit(dt);
		break;
	case DEAD:
		if (currentAnim->Finished()) {
			App->entityManager->DeleteUnit(this,isEnemy);
		}
		break;
	}

	return true;
}

bool Unit::Draw()
{
	if (state != IDLE) {
		App->render->Blit(entityTexture, currentAnim->flipAnim, entityPosition.x, entityPosition.y, &(currentAnim->GetCurrentFrame()));
	}
	else {
		App->render->Blit(entityTexture, currentAnim->flipAnim, entityPosition.x, entityPosition.y, &(currentAnim->GetCurrentFrame()));
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

void Unit::SetDestination()
{
	iPoint target;
	App->input->GetMousePosition(target.x, target.y);
	target = App->map->WorldToMap(target.x - App->render->camera.x, target.y - App->render->camera.y);

	iPoint origin;
	origin.x = entityPosition.x + (App->map->data.tile_width / 2);
	origin.y = entityPosition.y + (App->map->data.tile_height / 2);
	
	origin = App->map->WorldToMap(origin.x, origin.y);
	App->pathfinding->CreatePath(origin, target, path);
	
	if (path.size() > 0) {
		SetState(MOVING);
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

}

void Unit::Move(float dt)
{
	CalculateVelocity();
	LookAt();

	if (!destinationReached) {

		fPoint vel = (velocity * unitMovementSpeed) * dt;
		roundf(vel.x);
		roundf(vel.y);
		entityPosition.x += int(vel.x);
		entityPosition.y += int(vel.y);
		collider->rect.x += int(vel.x);
		collider->rect.y += int(vel.y);

		if (entityPosition.DistanceNoSqrt(destinationTileWorld) < 1) {
			if (path.size() > 0) {
				destinationTile = path.front();
				path.erase(path.begin());
			}
			else {
				destinationReached = true;
				SetState(IDLE);
			}
		}
	}
}

void Unit::CalculateVelocity()
{
	destinationTileWorld = App->map->MapToWorld(destinationTile.x, destinationTile.y);
	velocity.x = destinationTileWorld.x - entityPosition.x;
	velocity.y = destinationTileWorld.y - entityPosition.y;

	velocity.Normalize();
}

void Unit::LookAt()
{

	if (state == ATTACKING)
	{
		if (attackUnitTarget)
		{
			velocity.x = attackUnitTarget->entityPosition.x - entityPosition.x;
			velocity.y = attackUnitTarget->entityPosition.y - entityPosition.y;

			velocity.Normalize();
		}
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
	if (timer >= attackDelay) {
		attackUnitTarget->unitLife -= unitAttack - attackUnitTarget->unitDefense;
		if (attackUnitTarget->unitLife <= 0) {
			attackUnitTarget->Dead();
			if (unitLife > 0) {
				SetState(IDLE);
			}
		}
		timer = 0;
	}
	else {
		timer += dt;
	}
	
}

void Unit::Dead() {
	SetState(DEAD);
	App->collision->DeleteCollider(collider);
}

void Unit::SetState(unitState state)
{
	switch (state) {
	case IDLE:
		this->state = IDLE;
		SetAnim(currentDirection);
		entityTexture = unitIdleTexture;
		break;
	case MOVING:
		this->state = MOVING;
		SetAnim(currentDirection);
		entityTexture = unitMoveTexture;
		break;
	case ATTACKING:
		this->state = ATTACKING;
		SetAnim(currentDirection);
		entityTexture = unitAttackTexture;
		break;
	case DEAD:
		this->state = DEAD;
		SetAnim(currentDirection);
		entityTexture = unitDieTexture;
		break;
	}
}

void Unit::SetAnim(unitDirection currentDirection) {

	switch (state) {
	case IDLE:
		switch (currentDirection) {
		case UP:
			currentAnim = &idleUp;
			break;
		case UP_RIGHT:
			currentAnim = &idleUpRight;
			break;
		case RIGHT:
			currentAnim = &idleRight;
			break;
		case DOWN_RIGHT:
			currentAnim = &idleDownRight;
			break;
		case DOWN:
			currentAnim = &idleDown;
			break;
		case DOWN_LEFT:
			currentAnim = &idleDownLeft;
			break;
		case LEFT:
			currentAnim = &idleLeft;
			break;
		case UP_LEFT:
			currentAnim = &idleUpLeft;
			break;
		}
		break;
	case MOVING:
		switch (currentDirection) {
		case UP:
			currentAnim = &movingUp;
			break;
		case UP_RIGHT:
			currentAnim = &movingUpRight;
			break;
		case RIGHT:
			currentAnim = &movingRight;
			break;
		case DOWN_RIGHT:
			currentAnim = &movingDownRight;
			break;
		case DOWN:
			currentAnim = &movingDown;
			break;
		case DOWN_LEFT:
			currentAnim = &movingDownLeft;
			break;
		case LEFT:
			currentAnim = &movingLeft;
			break;
		case UP_LEFT:
			currentAnim = &movingUpLeft;
			break;
		}
		break;
	case ATTACKING:
		switch (currentDirection) {
		case UP:
			currentAnim = &attackingUp;
			break;
		case UP_RIGHT:
			currentAnim = &attackingUpRight;
			break;
		case RIGHT:
			currentAnim = &attackingRight;
			break;
		case DOWN_RIGHT:
			currentAnim = &attackingDownRight;
			break;
		case DOWN:
			currentAnim = &attackingDown;
			break;
		case DOWN_LEFT:
			currentAnim = &attackingDownLeft;
			break;
		case LEFT:
			currentAnim = &attackingLeft;
			break;
		case UP_LEFT:
			currentAnim = &attackingUpLeft;
			break;
		}
		break;
	case DEAD:
		switch (currentDirection) {
		case UP:
			currentAnim = &dyingUp;
			break;
		case UP_RIGHT:
			currentAnim = &dyingUpRight;
			break;
		case RIGHT:
			currentAnim = &dyingRight;
			break;
		case DOWN_RIGHT:
			currentAnim = &dyingDownRight;
			break;
		case DOWN:
			currentAnim = &dyingDown;
			break;
		case DOWN_LEFT:
			currentAnim = &dyingDownLeft;
			break;
		case LEFT:
			currentAnim = &dyingLeft;
			break;
		case UP_LEFT:
			currentAnim = &dyingUpLeft;
			break;
		}
		break;
	}
}