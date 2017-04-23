#ifndef __HERO__
#define __HERO__

#include "Unit.h"
#include "Timer.h"

enum Skill_type { 

	NO_SKILL = 0,
	DAMAGE_SKILL,

};

class Skill;

class Hero : public Unit
{
public:
	Hero();
	Hero(int posx, int posy, Hero* unit = nullptr);
	~Hero();
	bool HeroUpdate();

public:

	Timer skill_timer;
	Skill* skill = nullptr;

};


class Skill{
public:
	int cooldown = 0;
	int duration = 0;
	bool active = false;

	SDL_Texture* effect = nullptr;
	Skill_type type = NO_SKILL;

public:
	virtual bool Activate(Hero* hero = nullptr)
	{
		return true;
	}

	virtual bool Deactivate(Hero* hero = nullptr)
	{
		return true;
	}

};


class DamageSkill : public Skill {

public:
	float multiplier;

public:

	DamageSkill(){

		cooldown = 20;
		duration = 5;
		type = DAMAGE_SKILL;
		
		multiplier = 2.5;
	}

	bool Activate(Hero* hero) 
	{
		hero->skill_timer.Start();
		hero->Attack *= multiplier;
		return true;
	}

	bool Deactivate(Hero* hero)
	{
		hero->Attack /= multiplier;
		return false;
	}

};

#endif