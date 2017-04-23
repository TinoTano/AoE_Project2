#include "Villager.h"
#include "Application.h"
#include "SceneManager.h"

Villager::Villager() {}


Villager::Villager(int posx, int posy, Villager* unit) : Unit(posx, posy, (Unit*) unit){

	gathering_speed = unit->gathering_speed;
	max_capacity = unit->max_capacity;

	unitChoppingTexture = unit->unitChoppingTexture;
	choppingAnimations = unit->choppingAnimations;

	IsVillager = true;
}



void Villager::GatherResource(float dt)
{

	if (currentAnim->Finished()) {
		curr_capacity += MIN(attackTarget->Life, gathering_speed);
		attackTarget->Life -= MIN(attackTarget->Life, gathering_speed);
	}

	if (curr_capacity >= max_capacity || attackTarget->Life <= 0) {

		if (attackTarget->Life <= 0 && attackTarget) 
			attackTarget->Dead();
		

		iPoint destination = App->map->WorldToMap(TOWN_HALL_POS_X, TOWN_HALL_POS_Y);
		SetDestination(destination);
		SetState(UNIT_MOVING);

	}

}

void Villager::Contructing(float dt)
{
	if (currentAnim->Finished()) {
		constructingTarget->Life += buildingSpeed;
	}

	if (constructingTarget->Life >= constructingTarget->MaxLife) {
		if (constructingTarget->Life > constructingTarget->MaxLife) {
			constructingTarget->Life = constructingTarget->MaxLife;
		}
		SetState(UNIT_IDLE);
	}
}



