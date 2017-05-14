#ifndef __HERO__
#define __HERO__

#include "Unit.h"
#include "Timer.h"

enum Skill_type {

	NO_SKILL = 0,
	DAMAGE_SKILL,
	MOVE_SPEED_SKILL,
	ATTACK_SPEED_SKILL,
	AOE_SKILL

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
	Collider* aoeTargets = nullptr;
	Timer skill_timer;
	Skill* skill = nullptr;

};


class Skill{
public:
	int cooldown = 0;
	int duration = 0;
	bool active = false;
	bool ready = false;
	int damage = 0;
	float range = 0;
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

class MoveSpeedSkill : public Skill {

public:
	float multiplier = 0;

public:

	MoveSpeedSkill() {

		cooldown = 20;
		duration = 10;
		type = MOVE_SPEED_SKILL;

		multiplier = 3;
	}

	void Activate(Hero* hero)
	{
		active = true;
		hero->skill_timer.Start();
		hero->unitMovementSpeed *= multiplier;
	}

	void Deactivate(Hero* hero)
	{
		active = false;
		hero->unitMovementSpeed /= multiplier;
	}

};

class AttackSpeedSkill : public Skill {

public:
	float multiplier;

public:

	AttackSpeedSkill() {

		cooldown = 20;
		duration = 10;
		type = ATTACK_SPEED_SKILL;

		multiplier = 2.5;
	}

	void Activate(Hero* hero)
	{
		active = true;
		hero->skill_timer.Start();
		for (vector<Animation>::iterator it = hero->attackingAnimations.begin(); it != hero->attackingAnimations.end(); it++) {
			(it)->speed *= multiplier;
		}
	}

	void Deactivate(Hero* hero)
	{
		active = false;
		for (vector<Animation>::iterator it = hero->attackingAnimations.begin(); it != hero->attackingAnimations.end(); it++) {
			(it)->speed /= multiplier;
		}
	}

};

class AOESkill : public Skill {

public:

	AOESkill() {

		cooldown = 20;
		duration = 1;
		type = AOE_SKILL;
		damage = 50;
		range = 100;
	}

	void Activate(Hero* hero)
	{
		active = true;
		hero->aoeTargets->enabled = true;
		hero->skill_timer.Start();
	}

	void Deactivate(Hero* hero)
	{
		active = false;
		hero->aoeTargets->enabled = false;
	}

};

#endif