#include "Building.h"
#include "Render.h"
#include "Application.h"
#include "Map.h"
#include "Textures.h"
#include "Unit.h"
#include "EntityManager.h"
#include "Collision.h"
#include "p2Log.h"
#include "Orders.h"
#include "math.h"
#include "AI.h"


Building::Building()
{
}

Building::Building(int posX, int posY, Building* building)
{
	entityPosition.x = posX;
	entityPosition.y = posY;

	name = building->name;
	type = building->type;
	faction = building->faction;
	buildingPiercingDamage = building->buildingPiercingDamage;
	cost = building->cost;
	

	MaxLife = building->MaxLife;
	Attack = building->Attack;
	Defense = building->Defense;
	canAttack = building->canAttack;
	selectionWidth = building->selectionWidth;
	selectionAreaCenterPoint = building->selectionAreaCenterPoint;

	if (type == TOWN_CENTER || type == SAURON_TOWER) {
		Life = MaxLife;
		entityTexture = building->entityTexture;
	}
	else {
		Life = 1;
		state = BEING_BUILT;
		entityTexture = App->entityManager->constructingPhase1;
	}

	GetBuildingBoundaries();

	collider = App->collision->AddCollider({ entityPosition.x, entityPosition.y + ((int)imageHeight - selectionAreaCenterPoint.y - 15) }, imageWidth / 2, COLLIDER_BUILDING, App->entityManager, (Entity*)this);
	
	if(building->canAttack)
		range = App->collision->AddCollider({ entityPosition.x, entityPosition.y + ((int)imageHeight - selectionAreaCenterPoint.y - 15) }, imageWidth, COLLIDER_RANGE, App->entityManager, (Entity*)this);

	attack_timer.Start();
}

Building::~Building()
{}

void Building::GetBuildingBoundaries()
{
	App->tex->GetSize(entityTexture, imageWidth, imageHeight);
}

bool Building::Update(float dt)
{

	if (state != DESTROYED && state != BEING_BUILT) {

		if (!units_in_queue.empty()) {
			
			if (App->entityManager->unitsDB[units_in_queue.front()]->faction == FREE_MEN)
			drawUnitsInQueue((int)(App->entityManager->unitsDB[units_in_queue.front()]->cooldown_time * 1000), (int)creation_timer.Read() - (int) aux_timer, App->entityManager->unitsDB[units_in_queue.front()]->IsHero);
			
			/*(int)(tech->research_time * 1000), (int)tech->research_timer.Read() - (int)tech->aux_timer*/
			
			if (creation_timer.Read() - aux_timer > App->entityManager->unitsDB[units_in_queue.front()]->cooldown_time * 1000) {

				iPoint creation_place = App->map->WorldToMap(entityPosition.x, entityPosition.y + 250);
				creation_place = App->pathfinding->FindNearestAvailable(creation_place, 5);
				creation_place = App->map->MapToWorld(creation_place.x, creation_place.y);

				Unit* unit = App->entityManager->CreateUnit(creation_place.x, creation_place.y, units_in_queue.front());

				if (unit->type == SLAVE_VILLAGER)
					App->ai->requested_villagers--;

				if (unit->faction == SAURON_ARMY)
					unit->squad = App->ai->AssignUnit(unit);

				units_in_queue.pop_front();
				aux_timer = creation_timer.Read();
			}
		}
		else 
			aux_timer = creation_timer.Read();


		if (state == ATTACKING && attack_timer.ReadSec() > 3) {    //  3: building atatack speed (provisional)

			if (Entity* enemy = App->entityManager->FindTarget(this)) {
				if (range->CheckCollision(enemy->collider)) 
					enemy->Life -= MAX(Attack - enemy->Defense, buildingPiercingDamage);  // this should cast an arrow particle
				else
					state = IDLE;
			}
			else state = IDLE;
		}
	}
	
	return true;
}

bool Building::Draw()
{
	Sprite aux;

	aux.texture = entityTexture;
	aux.pos.x = entityPosition.x - (imageWidth / 2);
	aux.pos.y = entityPosition.y - (imageHeight / 2);
	aux.priority = entityPosition.y/* - (imageHeight / 2) + imageHeight*/;
	aux.rect.w = imageWidth;
	aux.rect.h = imageHeight;

	App->render->sprites_toDraw.push_back(aux);

	if (last_life != Life) {
		lifebar_timer.Start();
		last_life = Life;
	}

	if (lifebar_timer.ReadSec() < 5) {
		iPoint p(entityPosition.x - 25, entityPosition.y - (imageHeight / 2));
		drawLife(p);
	}

	techpos = { (int)(entityPosition.x - 25), (int) (entityPosition.y - (imageHeight / 2) + 5)};


	return true;
}

void Building::Destroy() {

	if (faction == App->entityManager->player->faction) {
		App->entityManager->player->buildings.remove(this);
		App->ai->RemoveThreats(this);
	}
	else {
		App->entityManager->AI_faction->buildings.remove(this);
		App->ai->build_requests.push_back(type);
	}

	App->collision->DeleteCollider(collider);

	if(canAttack)
		App->collision->DeleteCollider(range);

	state = DESTROYED;
	App->entityManager->DeleteEntity(this);

}


bool Building::Load(pugi::xml_node &)
{
	return true;
}

bool Building::Save(pugi::xml_node &) const
{
	return true;
}

void Building::drawTechnology(int MaxTech, int Tech)
{

		Sprite bar;
		Sprite bar2;

		int percent = ((MaxTech - Tech) * 100) / MaxTech;
		int barPercent = (percent * TECHBAR_WIDTH) / 100;

		bar.rect.x = bar2.rect.x = techpos.x;
		bar.rect.y = bar2.rect.y = techpos.y;
		bar.rect.w = TECHBAR_WIDTH;
		bar.rect.h = bar2.rect.h = 5;
		bar.priority = entityPosition.y + 10;
		bar.r = 255;
		bar.g = 255;
		bar.b = 255;

		bar2.rect.w = MIN(TECHBAR_WIDTH, MAX(TECHBAR_WIDTH - barPercent, 0));
		bar2.priority = entityPosition.y + 11;
		bar2.r = 0;
		bar2.g = 25;
		bar2.b = 255;

		App->render->sprites_toDraw.push_back(bar);
		App->render->sprites_toDraw.push_back(bar2);

}

void Building::drawUnitsInQueue(int MaxTime, int Time, bool isHero)
{
		Sprite bar;
		Sprite bar2;

		if (MaxTime <= 0) MaxTime = Time;
		int percent = (((MaxTime - Time) * 100) / MaxTime);
		int barPercent = (percent * TECHBAR_WIDTH) / 100;

		bar.rect.x = bar2.rect.x = techpos.x;
		bar.rect.y = bar2.rect.y = techpos.y + 5;
		bar.rect.w = TECHBAR_WIDTH;
		bar.rect.h = bar2.rect.h = 5;
		bar.priority = entityPosition.y + 10;
		bar.r = 255;
		bar.g = 255;
		bar.b = 255;

		bar2.rect.w = MIN(TECHBAR_WIDTH, MAX(TECHBAR_WIDTH - barPercent, 0));
		bar2.priority = entityPosition.y + 11;

		if (isHero)
		{
			bar2.r = 255;
			bar2.g = 180;
			bar2.b = 30;
		}
		else
		{
			bar2.r = 255;
			bar2.g = 127;
			bar2.b = 80;
		}

		App->render->sprites_toDraw.push_back(bar);
		App->render->sprites_toDraw.push_back(bar2);

}
