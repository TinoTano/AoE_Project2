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
	default:
		break;
	}

	entityType = ENTITY_UNIT;
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

		App->render->DrawIsometricCircle(entityPosition.x, entityPosition.y + selectionAreaCenterPoint.y, selectionRadius + 10, 0, 0, 255, 255);

	}
	if (skill_timer.ReadSec() > skill->cooldown)
		skill->ready = true;
	else skill->ready = false;

	return true;
}
