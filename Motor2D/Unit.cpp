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
#include "QuadTree.h"
#include "Orders.h"
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

	SetAnim(state);

	SDL_Rect r = currentAnim->GetCurrentFrame();

	collider = App->collision->AddCollider(entityPosition, r.w / 2, COLLIDER_UNIT, App->entityManager, (Entity*)this);
	range = App->collision->AddCollider(entityPosition, r.w , COLLIDER_RANGE, App->entityManager, (Entity*)this);

}

Unit::~Unit()
{
	for (list<Order*>::iterator it = order_list.begin(); it != order_list.end(); it++) 
		RELEASE(*it);
}

bool Unit::Update(float dt)
{
	r = currentAnim->GetCurrentFrame();

	if (Life == -1) {
		SetTexture(DESTROYED);
		App->collision->DeleteCollider(collider);
		App->collision->DeleteCollider(range);
		state = DESTROYED;
	}

	if (state != DESTROYED) {

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
			if (state != IDLE) {
				SetTexture(IDLE);
				state = IDLE;
			}
		}
	}
	else {
		if (currentAnim->Finished()) 
			App->entityManager->DeleteUnit(this);
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

		if (state == ATTACKING)
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
*/
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

void Unit::CalculateVelocity()
{
	velocity.x = destinationTileWorld.x - entityPosition.x;
	velocity.y = destinationTileWorld.y - entityPosition.y;

	if(velocity.x != 0 || velocity.y != 0)
		velocity.Normalize();
	
	LookAt();
}

void Unit::LookAt()
{
	unitDirection direction;

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
		currentDirection = direction;
	
}



void Unit::SetTexture(EntityState texture_of)
{
	Villager* villager = nullptr;

	switch (texture_of) {
	case IDLE:
		entityTexture = unitIdleTexture;
		break;
	case MOVING:
		entityTexture = unitMoveTexture;
		break;
	case ATTACKING:
		entityTexture = unitAttackTexture;
		break;
	case DESTROYED:
		entityTexture = unitDieTexture;
		break;
	case GATHERING:
		villager = (Villager*)this;
		entityTexture = villager->unitChoppingTexture;
		break;
	case CONSTRUCTING:
		villager = (Villager*)this;
		entityTexture = villager->unitChoppingTexture;
		break;
	}

	SetAnim(texture_of);
}

void Unit::SetAnim(EntityState anim_of) {

	Villager* villager = nullptr;

	switch (anim_of) {
	case IDLE:
		currentAnim = &idleAnimations[currentDirection];
		break;
	case MOVING:
		currentAnim = &movingAnimations[currentDirection];
		break;
	case ATTACKING:
		currentAnim = &attackingAnimations[currentDirection];
		break;
	case DESTROYED:
		currentAnim = &dyingAnimations[currentDirection];
		break;
	case GATHERING:
		villager = (Villager*)this;
		currentAnim = &villager->choppingAnimations[currentDirection];
	case CONSTRUCTING:
		villager = (Villager*)this;
		currentAnim = &villager->choppingAnimations[currentDirection];
	default: 
		break;
	}

}
