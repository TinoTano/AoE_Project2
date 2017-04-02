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
	// My changes --------------------------

	time_inactive.Start();

	// -------------------------------------

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
	SDL_Rect colliderRect = { entityPosition.x - (r.w / 4), entityPosition.y - (r.h / 3), r.w / 2, r.h / 1.25f};
	COLLIDER_TYPE colliderType;

	colliderType = COLLIDER_UNIT;

	uint w = 0, h = 0;

	collider = App->collision->AddCollider(entityPosition, r.w / 6, colliderType, App->entityManager, (Entity*)this);
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
			App->collision->DeleteCollider(collider);
		}
		break;
	}


	return true;
}

bool Unit::Draw()
{
	// My changes ---------------------------------------------------------

	Sprite aux;
  /*
	if (isVisible) 
	{
		SDL_Rect r = currentAnim->GetCurrentFrame();

		collider->rect.x = entityPosition.x - (r.w / 4);
		collider->rect.y = entityPosition.y - (r.h / 3);

		aux.rect = r;
		aux.texture = entityTexture;
		aux.pos.x = entityPosition.x - (r.w / 2);
		aux.pos.y = entityPosition.y - (r.h / 2);
		aux.priority = entityPosition.y - (r.h / 2) + r.h;
		aux.flip = currentAnim->flip;

		if (isSelected) 
		{
			Sprite bar;

			bar.pos = { entityPosition.x, entityPosition.y + (r.h / 2) };
			bar.priority = entityPosition.y - (r.h / 2) + r.h - 1;
			bar.radius = 15;
			bar.r = 255;
			bar.g = 255;
			bar.b = 255;

			App->render->sprites_toDraw.push_back(bar);

			int percent = ((MaxLife - Life) * 100) / MaxLife;
			int barPercent = (percent * hpBarWidth) / 100;

			bar.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = entityPosition.y - ((int)(collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = entityPosition.y - (r.h / 2) + r.h;
			bar.r = 255;
			bar.g = 0;
			bar.b = 0;

			App->render->sprites_toDraw.push_back(bar);

			bar.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = entityPosition.y - ((int)(collider->rect.h / 1.5f));
			bar.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar.rect.h = 5;
			bar.priority = entityPosition.y - (r.h / 2) + r.h;
			bar.r = 0;
			bar.r = 0;
			bar.g = 255;
			bar.b = 0;

			App->render->sprites_toDraw.push_back(bar);
		}

		if (attackTarget != nullptr)
		{
			Sprite bar;

			int percent = ((attackTarget->MaxLife - attackTarget->Life) * 100) / attackTarget->MaxLife;
			int barPercent = (percent * hpBarWidth) / 100;

			bar.rect.x = attackTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackTarget->entityPosition.y - ((int)(attackTarget->collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = attackTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 255;

			App->render->sprites_toDraw.push_back(bar);
			
			bar.rect.x = attackTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackTarget->entityPosition.y - ((int)(attackTarget->collider->rect.h / 1.5f));
			bar.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar.rect.h = 5;
			bar.priority = attackTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 0;
			bar.g = 255;

			App->render->sprites_toDraw.push_back(bar);
		}

		 if (collider != nullptr) App->render->sprites_toDraw.push_back(aux);
    */
		iPoint col_pos;
		if(state == UNIT_MOVING)
			col_pos.create(next_step.x, next_step.y + (r.h / 2));    // an offset var in collider should be implemented for big units
		else
			col_pos.create(entityPosition.x, entityPosition.y + (r.h / 2));

		collider->pos = col_pos;
		App->render->Blit(entityTexture, entityPosition.x - (r.w / 2), entityPosition.y - (r.h / 2), &r, currentAnim->flip);

	}

	if (isHero) Hero_Special_Attack();

	// ---------------------------------------------------------------------

	return true;
}

unitType Unit::GetType() const
{
	return type;
}

int Unit::GetLife() const
{
	return Life;
}

bool Unit::IsEnemy() const
{
	return (bool)faction;
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
	// My changes ----------------------------------------------------


	if (faction != FREE_PEOPLES) {
		destination = App->scene->my_townCenter->GetPosition();
		destination = App->map->WorldToMap(target.x, target.y);
	}

	if (path != nullptr) {
		App->pathfinding->DeletePath(path);
		path = nullptr;
	}

	iPoint origin = App->map->WorldToMap(collider->pos.x, collider->pos.y);
	path = App->pathfinding->CreatePath(origin, destination);
	
}

void Unit::Move(float dt)
{
	entityPosition = next_step;

	if (collider->pos.DistanceNoSqrt(destinationTileWorld) < 3) {
		if (path->size() > 0) {
			destinationTileWorld = App->map->MapToWorld(path->front().x, path->front().y);
			destinationTileWorld.x += 48;             // to center the unit in the tile
			destinationTileWorld.y += 48;
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

	velocity.x = destinationTileWorld.x - collider->pos.x;
	velocity.y = destinationTileWorld.y - collider->pos.y;

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
	if (state != UNIT_DEAD) {

		switch (newState) {
		case UNIT_IDLE:
			this->state = UNIT_IDLE;
			SetAnim(currentDirection);
			entityTexture = unitIdleTexture;
			break;
		case UNIT_MOVING:

			destinationTileWorld = App->map->MapToWorld(path->front().x, path->front().y);
			destinationTileWorld.x += 48;             // to center the unit in the tile
			destinationTileWorld.y += 48;

			if (path->size() > 0)
				path->erase(path->begin());

			this->state = UNIT_MOVING;
			next_step = entityPosition;

			if (collider->pos.DistanceNoSqrt(destinationTileWorld) > 1) {
				SetAnim(currentDirection);
				entityTexture = unitMoveTexture;
			}

			if (attackTarget != nullptr)
				attackTarget = nullptr;
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
}

bool Unit::Load(pugi::xml_node & node)
{
	return true;
}

bool Unit::Save(pugi::xml_node & node) const
{
	return true;
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

pugi::xml_node Unit::LoadUnitInfo(unitType type)
{
	return pugi::xml_node();
}

// My changes ---------------------------------------------------

bool Unit::Hero_Special_Attack()
{
	int cooldown = 10;
	int time_attacking = 3;

	// Attack

	if (isSelected && time_inactive.ReadSec() >= cooldown && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !clicked)
	{
		Attack *= 10;
		time_active.Start();		
		clicked = true;
	}

	else if (time_active.ReadSec() >= time_attacking && clicked)
	{
		Attack /= 10;
		time_inactive.Start();
		clicked = false;
	}

	// Drawing double circle to show that is doing the special attack

	if (time_inactive.ReadSec() >= cooldown && time_active.ReadSec() <= time_attacking)
	{
		SDL_Rect r = currentAnim->GetCurrentFrame();
		Sprite aux;

		aux.pos = { entityPosition.x, entityPosition.y + (r.h / 2) };
		aux.priority = entityPosition.y - (r.h / 2) + r.h - 1;
		aux.radius = 25;
		aux.r = 255;
		aux.g = 255;
		aux.b = 255;

		App->render->sprites_toDraw.push_back(aux);
	}

	return true;
}

// -------------------------------------------------------------
