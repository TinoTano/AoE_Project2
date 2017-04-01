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

			int percent = ((unitMaxLife - unitLife) * 100) / unitMaxLife;
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

		if (attackUnitTarget != nullptr)
		{
			Sprite bar;

			int percent = ((attackUnitTarget->unitMaxLife - attackUnitTarget->unitLife) * 100) / attackUnitTarget->unitMaxLife;
			int barPercent = (percent * hpBarWidth) / 100;

			bar.rect.x = attackUnitTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackUnitTarget->entityPosition.y - ((int)(attackUnitTarget->collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = attackUnitTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 255;

			App->render->sprites_toDraw.push_back(bar);
			
			bar.rect.x = attackUnitTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackUnitTarget->entityPosition.y - ((int)(attackUnitTarget->collider->rect.h / 1.5f));
			bar.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar.rect.h = 5;
			bar.priority = attackUnitTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 0;
			bar.g = 255;

			App->render->sprites_toDraw.push_back(bar);
		}

		if (attackBuildingTarget != nullptr)
		{
			Sprite bar;

			int percent = ((attackBuildingTarget->buildingMaxLife - attackBuildingTarget->buildingLife) * 100) / attackBuildingTarget->buildingMaxLife;
			int barPercent = (percent * hpBarWidth) / 100;

			bar.rect.x = attackBuildingTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackBuildingTarget->entityPosition.y - ((int)(attackBuildingTarget->collider->rect.h / 1.5f));
			bar.rect.w = hpBarWidth;
			bar.rect.h = 5;
			bar.priority = attackBuildingTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 255;

			App->render->sprites_toDraw.push_back(bar);

			bar.rect.x = attackBuildingTarget->entityPosition.x - (hpBarWidth / 2);
			bar.rect.y = attackBuildingTarget->entityPosition.y - ((int)(attackBuildingTarget->collider->rect.h / 1.5f));
			bar.rect.w = min(hpBarWidth, max(hpBarWidth - barPercent, 0));
			bar.rect.h = 5;
			bar.priority = attackBuildingTarget->entityPosition.y - (r.h / 2) + r.h;
			bar.r = 0;
			bar.g = 255;

			App->render->sprites_toDraw.push_back(bar);
		}

		 if (collider != nullptr) App->render->sprites_toDraw.push_back(aux);
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
	// My changes ----------------------------------------------------

	iPoint target;

	if (!isEnemy) {
		App->input->GetMousePosition(target.x, target.y);
		target = App->map->WorldToMap(target.x - App->render->camera.x, target.y - App->render->camera.y);
	}
	else {
		target = App->scene->my_townCenter->GetPosition();
		target = App->map->WorldToMap(target.x, target.y);
	}

	iPoint origin = App->map->WorldToMap(entityPosition.x, entityPosition.y);
	App->pathfinding->CreatePath(origin, target, path);

	// ----------------------------------------------------------------

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
		entityPosition.x += int(vel.x);
		entityPosition.y += int(vel.y);

		if (entityPosition.DistanceNoSqrt(destinationTileWorld) < 1) {
			if (path.size() > 0) {
				destinationTile = path.front();
				path.erase(path.begin());
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
	// My changes ---------------------------------------------------------------------------

	LookAt();

	if (currentAnim->Finished()) {
		attackUnitTarget->unitLife -= unitAttack - attackUnitTarget->unitDefense;
		if (attackUnitTarget->unitLife <= 0) {
			attackUnitTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackUnitTarget = nullptr;
			}
		}
	}

	// ----------------------------------------------------------------------------------------
}

void Unit::AttackEnemyBuilding(float dt)
{
	// My changes -----------------------------------------------------------------------------

	LookAt();
	if (currentAnim->Finished()) {
		attackBuildingTarget->buildingLife -= unitAttack - attackBuildingTarget->buildingDefense;
		if (attackBuildingTarget->buildingLife <= 0) {
			attackBuildingTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackBuildingTarget = nullptr;
			}
		}
	}

	// ------------------------------------------------------------------------------------------
}

void Unit::Dead() {
	SetState(UNIT_DEAD);
	collider->rect = { 0, 0, 0, 0 };
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
		unitAttack *= 10;
		time_active.Start();		
		clicked = true;
	}

	else if (time_active.ReadSec() >= time_attacking && clicked)
	{
		unitAttack /= 10;
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
