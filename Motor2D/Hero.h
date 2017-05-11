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
	bool ready = false;
	SDL_Texture* effect = nullptr;
	Skill_type type = NO_SKILL;

public:
	virtual void Activate(Hero* hero = nullptr)
	{}

	virtual void Deactivate(Hero* hero = nullptr)
	{}

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

	void Activate(Hero* hero) 
	{
		active = true;
		hero->skill_timer.Start();
		hero->Attack *= multiplier;
	}

	void Deactivate(Hero* hero)
	{
		active = false;
		hero->Attack /= multiplier;
	}

};

#endif