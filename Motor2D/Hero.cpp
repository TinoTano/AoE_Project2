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

	default:
		break;
	}

	skill->active = true;
}

Hero::~Hero() 
{
	RELEASE(skill);
}

bool Hero::HeroUpdate() {

	if (skill->active = true) {
		if (skill_timer.ReadSec() > skill->duration) 
			skill->active = skill->Deactivate(this);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE)) {
		//if(App->entityManager->FindUnit(this))
		if (skill_timer.ReadSec() > skill->cooldown)
			skill->active = skill->Activate(this);
	}


	if (skill->active)
	{  // this should be replaced by skill_effect bliting

		Sprite aux;

		aux.pos = { entityPosition.x, entityPosition.y + (r.h / 2) };
		aux.priority = entityPosition.y - (r.h / 2) + r.h - 1;
		aux.radius = 25;
		aux.r = aux.g = aux.b = 255;

		App->render->sprites_toDraw.push_back(aux);
	}

	return true;
}
