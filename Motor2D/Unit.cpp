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

Unit::Unit()
{
}

Unit::Unit(int posX, int posY, bool isEnemy, Unit* unit)
{
	time_inactive.Start();

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
	unitRange = unit->unitRange;

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
		if (attackUnitTarget != nullptr && entityPosition.DistanceTo(attackUnitTarget->entityPosition) < unitRange ||
			attackBuildingTarget != nullptr && entityPosition.DistanceTo(attackBuildingTarget->entityPosition) < 150 ||
			resourceTarget != nullptr && entityPosition.DistanceTo(resourceTarget->entityPosition) < 60) {
			if (attackUnitTarget != nullptr) {
				if (attackUnitTarget->isEnemy) {
					if (attackUnitTarget->attackUnitTarget == nullptr) {
						attackUnitTarget->attackUnitTarget = this;
						attackUnitTarget->attackBuildingTarget = nullptr;
						attackUnitTarget->resourceTarget = nullptr;
						attackUnitTarget->destinationReached = true;
					}
				}
			}
			SetState(UNIT_ATTACKING);
			destinationReached = true;
		}
		else {
			if (attackUnitTarget != nullptr) {
				//Uncomment if you want the enemy to go back when it's far from town hall
				//if (isEnemy && entityPosition.DistanceTo(App->sceneManager->level1_scene->my_townCenter->entityPosition) > 1000) {
				//	SetState(UNIT_IDLE);
				//	destinationReached = true;
				//	break;
				//}
				if (destinationReached) {
					iPoint target = App->map->WorldToMap(attackUnitTarget->entityPosition.x, attackUnitTarget->entityPosition.y);
					SetDestination(target);
				}
				//Uncomment if you want the enemy to go back when it's far from attacker
				//else {
				//	if (entityPosition.DistanceTo(attackUnitTarget->entityPosition) > unitRange + 50) {
				//		if (isEnemy && attackUnitTarget->attackUnitTarget == nullptr && attackBuildingTarget == nullptr) {
				//			SetState(UNIT_IDLE);
				//			destinationReached = true;
				//			break;
				//		}
				//	}
				//}
			}
			Move(dt);
		}
		break;
	case UNIT_ATTACKING:
		if (attackUnitTarget != nullptr) {
			if (entityPosition.DistanceTo(attackUnitTarget->entityPosition) > unitRange) {
				SetState(UNIT_MOVING);
			}
			else {
				AttackEnemyUnit(dt);
			}
		}
		else if (attackBuildingTarget != nullptr) {
			AttackEnemyBuilding(dt);
		}
		else if (resourceTarget != nullptr) {
			GatherResource(dt);
		}
		break;
	case UNIT_DEAD:
		if (currentAnim->Finished()) {
			App->entityManager->DeleteUnit(this, isEnemy);
			App->collision->DeleteCollider(collider);
		}
		break;
	}

	SDL_Rect cam = App->render->culling_cam;
	r = currentAnim->GetCurrentFrame();
	if (entityPosition.x >= cam.x && entityPosition.x <= cam.x + cam.w && entityPosition.y > cam.y && entityPosition.y < cam.y + cam.h) {
		isVisible = true;
	}
	else {
		isVisible = false;
	}
	if (isVisible) {
		Draw();
	}

	if (isHero) Hero_Special_Attack();

	return true;
}

bool Unit::Draw()
{
	Sprite aux;

	if (isVisible)
	{
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
			}
			else {
				destinationReached = true;
				if (attackUnitTarget == nullptr) {
					SetState(UNIT_IDLE);
				}
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
		if (velocity.x != 0 || velocity.y != 0)
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

	if (currentAnim->Finished()) {
		attackUnitTarget->unitLife -= unitAttack - attackUnitTarget->unitDefense;

		if (attackUnitTarget->unitLife < 0)
			attackUnitTarget->unitLife = 0;

		if (attackUnitTarget->unitLife == 0) {
			attackUnitTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackUnitTarget = nullptr;
			}
		}
	}
}

void Unit::AttackEnemyBuilding(float dt)
{
	LookAt();
	if (currentAnim->Finished()) {
		attackBuildingTarget->buildingLife -= unitAttack - attackBuildingTarget->buildingDefense;


		if (attackBuildingTarget->buildingLife < 0)
			attackBuildingTarget->buildingLife = 0;

		if (attackBuildingTarget->buildingLife == 0) {
			attackBuildingTarget->Dead();
			if (unitLife > 0) {
				SetState(UNIT_IDLE);
				attackBuildingTarget = nullptr;
			}
		}
	}
}

void Unit::GatherResource(float dt)
{
	LookAt();
	if (currentAnim->Finished()) {
		int decreaseLife = resourceTarget->resourceLife;
		resourceTarget->resourceLife -= unitAttack;
		if (resourceTarget->resourceLife > 0) decreaseLife -= resourceTarget->resourceLife;
		switch (resourceTarget->type) {
		case BLACK_TREE:
			App->sceneManager->level1_scene->woodCount += decreaseLife;
			App->sceneManager->level1_scene->wood->SetString(to_string(App->sceneManager->level1_scene->woodCount));
			break;
		case GREEN_TREE:
			App->sceneManager->level1_scene->woodCount += decreaseLife;
			App->sceneManager->level1_scene->wood->SetString(to_string(App->sceneManager->level1_scene->woodCount));
			break;
		}
		if (resourceTarget->resourceLife <= 0) {
			resourceTarget->Dead();
			SetState(UNIT_IDLE);
			resourceTarget = nullptr;
		}
	}
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

bool Unit::Hero_Special_Attack()
{
	int cooldown = 10;
	int time_attacking = 3;

	// Attack

	if (isSelected && time_inactive.ReadSec() >= cooldown && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !clicked)
	{
		unitAttack += 20;
		time_active.Start();
		clicked = true;
	}

	else if (time_active.ReadSec() >= time_attacking && clicked)
	{
		unitAttack -= 20;
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
