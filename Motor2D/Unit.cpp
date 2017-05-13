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
#include "QuestManager.h"

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
	selectionRadius = unit->selectionRadius;
	selectionAreaCenterPoint = unit->selectionAreaCenterPoint;

	Life = unit->Life;
	MaxLife = unit->MaxLife;
	Attack = unit->Attack;
	Defense = unit->Defense;
	cost = unit->cost;

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
	los = App->collision->AddCollider(entityPosition, r.w * 4, COLLIDER_LOS, App->entityManager, (Entity*)this);

}

Unit::~Unit()
{
	for (list<Order*>::iterator it = order_list.begin(); it != order_list.end(); it++) 
		RELEASE(*it);
}

bool Unit::Update(float dt)
{
	r = currentAnim->GetCurrentFrame();

	if (state != DESTROYED) {

		if (IsHero) {
			Hero* hero = (Hero*)this;
			hero->HeroUpdate();
		}

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

			if (IsVillager && faction == App->entityManager->AI_faction->faction)
				App->ai->Fetch_AICommand((Villager*)this);
		}
	}
	else {
		if (currentAnim->Finished()) 
			App->entityManager->DeleteEntity(this);
	}


	return true;
}

void Unit::Destroy() {

	if (App->quest->TriggerKillCallback(this->type) == false)
		App->quest->StepKillCallback(this->type);

	SetTexture(DESTROYED);
	App->collision->DeleteCollider(collider);
	App->collision->DeleteCollider(range);
	App->collision->DeleteCollider(los);
	state = DESTROYED;
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

	App->render->sprites_toDraw.push_back(aux);

	if (last_life != Life) {
		lifebar_timer.Start();
		last_life = Life;
	}

	if (lifebar_timer.ReadSec() < 5)
		drawLife({ entityPosition.x - 25, entityPosition.y - (r.h / 2) }); //25:  HPBAR_WIDTH / 2

	return true;
}

void Unit::GetUnitBoundaries()
{
	App->tex->GetSize(entityTexture, imageWidth, imageHeight);
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

bool Unit::SetDestination(iPoint destination)
{

	if (path != nullptr) {
		App->pathfinding->DeletePath(path);
		path = nullptr;
	}

	iPoint origin = App->map->WorldToMap(collider->pos.x, collider->pos.y);
	path = App->pathfinding->CreatePath(origin, destination);

	return (!path->empty());
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
