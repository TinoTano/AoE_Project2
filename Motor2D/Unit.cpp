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
#include "SceneManager.h"
#include "Hero.h"
#include "Villager.h"

Unit::Unit()
{
}

Unit::Unit(int posX, int posY, Unit* unit)
{

	entityPosition.x = posX;
	entityPosition.y = posY;
	type = unit->type;
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
	unitRange = unit->unitRange;
	unitRangeOffset = unit->unitRangeOffset;

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

	collider = App->collision->AddCollider(entityPosition, r.w / 4, COLLIDER_UNIT, App->entityManager, (Entity*)this);
	range = App->collision->AddCollider(entityPosition, r.w , COLLIDER_RANGE, App->entityManager, (Entity*)this);
}

Unit::~Unit()
{
}

bool Unit::Update(float dt)
{
	r = currentAnim->GetCurrentFrame();
	Villager* villager = nullptr;
	if (type == VILLAGER || ELF_VILLAGER) {
		villager = (Villager*)this;
	}

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
	case UNIT_GATHERING:
		villager->GatherResource(dt);
		break;
	case UNIT_BUILDING:
		villager->Contructing(dt);
		break;
	}

	if (IsHero) {
		Hero* hero = (Hero*)this;
		hero->HeroUpdate();
	}

	return true;
}

bool Unit::Draw()
{
	Sprite aux;

	aux.rect = r;
	aux.texture = entityTexture;
	aux.pos.x = entityPosition.x - (r.w / 2);
	aux.pos.y = entityPosition.y - (r.h / 2);
	aux.priority = entityPosition.y - (r.h / 2) + r.h;
	aux.flip = currentAnim->flip;

	/*	if (isSelected)
		{
			Sprite circle;

			circle.pos = { entityPosition.x, entityPosition.y + (r.h / 2) };
			circle.priority = entityPosition.y - (r.h / 2) + r.h - 1;
			circle.radius = 15;
			circle.r = 255;
			circle.g = 255;
			circle.b = 255;

			App->render->sprites_toDraw.push_back(circle);

			int percent;
			int barPercent;

			if (MaxLife > 0) {
				percent = ((MaxLife - Life) * 100) / MaxLife;
				barPercent = (percent * hpBarWidth) / 100;
			}
			else {
				barPercent = 0;
			}

			Sprite bar;

			bar.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = entityPosition.y - ((int)(aux.rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = entityPosition.y - (r.h / 2) + r.h + 1;
			bar.r = 255;
			bar.g = 0;
			bar.b = 0;

			App->render->sprites_toDraw.push_back(bar);

			Sprite bar2;

			bar2.rect.x = entityPosition.x - (hpBarWidth / 2);
			bar2.rect.y = entityPosition.y - ((int)(aux.rect.h / 1.5f));
			bar2.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar2.rect.h = 5;
			bar2.priority = entityPosition.y - (r.h / 2) + r.h + 2;
			bar2.r = 0;
			bar2.g = 255;
			bar2.b = 0;

			App->render->sprites_toDraw.push_back(bar2);
		}
*/
		if (attackTarget != nullptr)
		{
			Sprite bar;

			int percent;
			int barPercent;

			if (attackTarget->MaxLife > 0) {
				percent = ((attackTarget->MaxLife - attackTarget->Life) * 100) / attackTarget->MaxLife;
				barPercent = (percent * hpBarWidth) / 100;
			}
			else {
				barPercent = 0;
			}

			bar.rect.x = attackTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackTarget->entityPosition.y; //- ((int)(attackTarget->collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = attackTarget->entityPosition.y - (r.h / 2) + r.h + 1;
			bar.r = 255;
			bar.g = 0;
			bar.b = 0;

			App->render->sprites_toDraw.push_back(bar);

			Sprite bar2;

			bar2.rect.x = attackTarget->entityPosition.x - (hpBarWidth / 2);
			bar2.rect.y = attackTarget->entityPosition.y; //- ((int)(attackTarget->collider->rect.h / 1.5f));
			bar2.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar2.rect.h = 5;
			bar2.priority = attackTarget->entityPosition.y - (r.h / 2) + r.h + 2;
			bar2.r = 0;
			bar2.g = 255;
			bar2.b = 0;

			App->render->sprites_toDraw.push_back(bar2);
		}

	App->render->sprites_toDraw.push_back(aux);
	
	
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

	iPoint origin = App->map->WorldToMap(collider->pos.x, collider->pos.y);
	path = App->pathfinding->CreatePath(origin, destination);
}

void Unit::Move(float dt)
{
	entityPosition = next_step;
	collider->pos = entityPosition;
	range->pos = entityPosition;

	if (collider->pos.DistanceTo(destinationTileWorld) < 10) {
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

	fPoint vel = velocity * unitMovementSpeed * dt;
	roundf(vel.x);
	roundf(vel.y);

	next_step.x = entityPosition.x + int(vel.x);
	next_step.y = entityPosition.y + int(vel.y);
}

void Unit::CalculateVelocity()
{
	if (state == UNIT_ATTACKING && attackTarget != nullptr)
	{
		velocity.x = attackTarget->entityPosition.x - collider->pos.x;
		velocity.y = attackTarget->entityPosition.y - collider->pos.y;
	}
	else {
		velocity.x = destinationTileWorld.x - collider->pos.x;
		velocity.y = destinationTileWorld.y - collider->pos.y;
	}

	if(velocity.x != 0 || velocity.y != 0)
		velocity.Normalize();
	
	LookAt();
}

void Unit::LookAt()
{

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
	CalculateVelocity();

	if (currentAnim->Finished()) {
		attackTarget->Life -= Attack - attackTarget->Defense;

		if (attackTarget->Life <= 0) {
			attackTarget->Life = -1;
			attackTarget->Dead();
			if (Life > 0) {
				SetState(UNIT_IDLE);
				attackTarget = nullptr;
			}
		}
	}
}



void Unit::Dead() {
	SetState(UNIT_DEAD);
	App->collision->DeleteCollider(collider);
	App->collision->DeleteCollider(range);
}

void Unit::SetState(unitState newState)
{
	Villager* villager = nullptr;

	switch (newState) {
	case UNIT_IDLE:
		entityTexture = unitIdleTexture;
		break;
	case UNIT_MOVING:

		destinationTileWorld = App->map->MapToWorld(path->front().x, path->front().y);
		destinationTileWorld.x += 48;             // to center the unit in the tile
		destinationTileWorld.y += 48;

		if (path->size() > 0)
			path->erase(path->begin());

		next_step = entityPosition;

		if (collider->pos.DistanceNoSqrt(destinationTileWorld) > 1) 
			entityTexture = unitMoveTexture;

		if (attackTarget != nullptr)
			attackTarget = nullptr;

		break;
	case UNIT_ATTACKING:
		entityTexture = unitAttackTexture;
		break;
	case UNIT_DEAD:
		entityTexture = unitDieTexture;
		break;
	case UNIT_GATHERING:
		villager = (Villager*)this;
		entityTexture = villager->unitChoppingTexture;
		break;
	case UNIT_BUILDING:
		villager = (Villager*)this;
		entityTexture = villager->unitChoppingTexture;
		break;
	}
	this->state = newState;
	SetAnim(currentDirection);
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
	case UNIT_GATHERING:
		Villager* villager = (Villager*)this;
		villager->currentAnim = &villager->choppingAnimations[currentDirection];
	}
}
