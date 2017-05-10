#include "Villager.h"

Villager::Villager() {}


Villager::Villager(int posx, int posy, Villager* unit) : Unit(posx, posy, (Unit*) unit){

	gathering_speed = unit->gathering_speed;
	max_capacity = unit->max_capacity;

	unitChoppingTexture = unit->unitChoppingTexture;
	choppingAnimations = unit->choppingAnimations;

	IsVillager = true;
}
