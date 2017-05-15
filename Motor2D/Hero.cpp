#include "Hero.h"
#include "Application.h"
#include "EntityManager.h"
#include "Render.h"

Hero::Hero() 
{
	skill = new Skill();
}

Hero::Hero(int posx, int posy, Hero* unit) : Unit(posx, posy, (Unit*)unit) {

	IsHero = true;
	skill_timer.Start();

	switch (unit->skill->type) {

	case DAMAGE_SKILL:
		skill = (Skill*) new DamageSkill();
		break;
	case MOVE_SPEED_SKILL:
		skill = (Skill*) new MoveSpeedSkill();
		break;
	case ATTACK_SPEED_SKILL:
		skill = (Skill*) new AttackSpeedSkill();
		break;
	case AOE_SKILL:
		skill = (Skill*) new AOESkill();
		aoeTargets = App->collision->AddCollider(entityPosition, skill->range, COLLIDER_AOE_SKILL, App->entityManager, (Entity*)this);
		if (aoeTargets != nullptr) {
			aoeTargets->enabled = false;
		}
		break;
	default:
		break;
	}

}

Hero::~Hero() 
{
	RELEASE(skill);
}

bool Hero::HeroUpdate() {


	if (skill->active) {
		if (skill_timer.ReadSec() > skill->duration)
			skill->Deactivate(this);

		// this should be replaced by skill_effect bliting

		Sprite aux;

		aux.pos = { entityPosition.x, entityPosition.y + (r.h / 2) };
		aux.priority = entityPosition.y - (r.h / 2) + r.h - 1;
		aux.radius = 25;
		aux.r = aux.g = aux.b = 255;

		App->render->sprites_toDraw.push_back(aux);

		if (skill->type == AOE_SKILL) {
			Sprite areaCircle;

			areaCircle.pos = { entityPosition.x, entityPosition.y };
			areaCircle.priority = entityPosition.y - (r.h / 2) + r.h - 1;
			areaCircle.radius = skill->range;
			areaCircle.r = 255;
			areaCircle.g = 0;
			areaCircle.b = 0;

			App->render->sprites_toDraw.push_back(areaCircle);
		}
	}
	if (skill_timer.ReadSec() > skill->cooldown)
		skill->ready = true;
	else skill->ready = false;

	return true;
}
