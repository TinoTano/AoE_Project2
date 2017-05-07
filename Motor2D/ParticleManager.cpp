#include "ParticleManager.h"
#include "Window.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "p2Defs.h"
#include "p2Log.h"
#include <stdlib.h>  
#include <time.h>
#include <math.h>

ParticleManager::ParticleManager() : Module()
{
	name = "particlemanager";
}

ParticleManager::~ParticleManager()
{
}

// We are loading all the information from config here.
// Ideally we would have the position also taken from the xml.

bool ParticleManager::Awake(pugi::xml_node &config)
{
	return true;
}

bool ParticleManager::Start()
{
	srand(time(NULL));
	App->win->GetWindowSize(window_size.first, window_size.second);

	LoadParticleData();

	for (uint i = 0; i < info.size(); ++i) {
		info[i].texture = App->tex->Load(info[i].path.c_str());
	}

	return true;
}

bool ParticleManager::Update(float dt)
{
	update.Start();
	for (list<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		it._Ptr->_Myval->Update();
	}
	update.Start();
	for (list<Emitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
	{
		it._Ptr->_Myval->Update(dt);
	}
	return true;
}

bool ParticleManager::PostUpdate()
{
	for (list<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		if (it._Ptr->_Myval->alive == false)
			DestroyParticle(it._Ptr->_Myval);
	}
	return true;
}

bool ParticleManager::CleanUp()
{
	for (list<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		if (it._Ptr->_Myval != nullptr)
			DestroyParticle(it._Ptr->_Myval);
	}

	for (list<Emitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
	{
		if (it._Ptr->_Myval != nullptr)
			DestroyEmitter(it._Ptr->_Myval);
	}

	for (uint i = 0; i < info.size(); ++i) {
		App->tex->UnLoad(info[i].texture);
	}

	particles.clear();
	return true;
}

Particle * ParticleManager::CreateArrow(int spd, pair<uint, uint> source, pair<uint, uint> destination)
{
	Particle* ret = nullptr;
	if (particles.size() < MAX_PARTICLES)
	{
		ret = new Arrow(spd, source, destination);
		ret->texture = info[ARROW].texture;
		ret->anim = info[ARROW].anim;
		ret->type = (ParticleType)info[ARROW].id;
		ret->name = info[ARROW].name;
		ret->lifetime = info[ARROW].lifespan;
		ret->states = info[ARROW].states;
		particles.push_back(ret);
	}
	else {
		LOG("Maximum particles achieved.");
	}
	return ret;
}

Particle * ParticleManager::CreateMovableParticle(pair<float, float> startingposition, pair<float, float> startingforce, bool gravity, ParticleType type)
{
	Particle* ret = nullptr;
	if (particles.size() < MAX_PARTICLES)
	{
		ret = new MovableParticle(gravity, startingforce, startingposition);
		ret->texture = info[type].texture;
		ret->anim = info[type].anim;
		ret->type = (ParticleType)info[type].id;
		ret->name = info[type].name;
		ret->lifetime = info[type].lifespan;
		ret->states = info[type].states;
		particles.push_back(ret);
	}
	else {
		LOG("Maximum particles achieved.");
	}
	return ret;
}

Particle * ParticleManager::CreateStaticBucle(pair<float, float> startingposition, bool finite, ParticleType type)
{
	Particle* ret = nullptr;
	if (particles.size() < MAX_PARTICLES)
	{
		ret = new StaticBucle(startingposition, finite);
		ret->texture = info[type].texture;
		ret->anim = info[type].anim;
		ret->type = (ParticleType)info[type].id;
		ret->name = info[type].name;
		ret->lifetime = info[type].lifespan;
		particles.push_back(ret);
	}
	else {
		LOG("Maximum particles achieved.");
	}
	return ret;
}

Emitter* ParticleManager::CreateEmitter(pair<float, float> startingposition, bool finite, float duration, ParticleType type)
{
	Emitter* ret = nullptr;
	ret = new Emitter(startingposition, finite, duration);
	ret->type = type;
	emitters.push_back(ret);
	return ret;
}


bool ParticleManager::DestroyParticle(Particle * curr)
{
	bool ret = true;
	if (curr != nullptr) {
		particles.remove(curr);
		//curr->CleanUp();
		delete curr;
	}
	else ret = false;
	return ret;
}

bool ParticleManager::DestroyEmitter(Emitter * curr)
{
	bool ret = true;
	if (curr != nullptr) {
		emitters.remove(curr);
		delete curr;
	}
	else ret = false;
	return ret;
}



Arrow::Arrow(int spd, pair<int, int> source, pair<int, int> dest) : src(source), dst(dest) {
	//blits = anim.frames()
	collider.x = src.first;
	collider.y = src.second;
	collider.w = 30;
	collider.h = 18;
	pos.first = (int)src.first;
	pos.second = (int)src.second;

	if (src.first >= dst.first) {
		flipped = false;
		time *= -1;
	}
	else
		flipped = true;

	speed = spd;

	////////////////////////////

	uint distance = sqrt((dst.second - src.second) * (dst.second - src.second) + (dst.first - src.first) * (dst.first - src.first));

	dg = atan((dst.second - src.second) / (dst.first - src.first))  * (180 / M_PI);
	if (flipped)
		dg *= -1;



	timer.Start();
}

void Arrow::Update() {

	spd_x = speed * cos(dg * M_PI / 180);

	spd_y = speed * -sin(dg * M_PI / 180);


	if (pos.first >= dst.first) {
		flipped = false;
		time *= -1;
	}
	else
		flipped = true;

	if (!flipped) {
		spd_x *= -1;
	}

	pos.first = src.first + spd_x * timer.ReadSec();
	pos.second = src.second + (spd_y)* timer.ReadSec();

	curr_spd.second = (dst.second - src.second) / timer.ReadSec();
	curr_spd.first = (dst.first - src.first) / timer.ReadSec();


	float dg = atan(curr_spd.second / curr_spd.first);
	dg = dg * (180 / M_PI);

	if (flipped)
		dg *= -1;

	int num;
	for (int i = -90, num = 0; i <= 90; i += 5, ++num) {
		if (i >= dg) {
			if (num == 17) num = 18;
			blit = anim.frames[num];
			break;
		}
	}

	Draw();
	alive = IsAlive();
}

void Arrow::Draw() {
	Sprite arrow;
	if (flipped)
		arrow.flip = SDL_FLIP_HORIZONTAL;

	arrow.pos.x = pos.first;
	arrow.pos.y = pos.second;
	arrow.texture = texture;
	arrow.rect = blit;

	App->render->sprites_toDraw.push_back(arrow);
}
bool Arrow::IsAlive() {
	bool ret = true;

	if (!flipped) {
		if (pos.first < dst.first || pos.first + collider.w < dst.first)
			ret = false;
	}
	else if (pos.first > dst.first || pos.first + collider.w > dst.first)
		ret = false;
	else if (timer.ReadSec() >= lifetime) {
		ret = false;
	}
	return ret;
}


// --------------------------
//     MOVABLE PARTICLES
// --------------------------
// Particles that are not meant to be static, which means
// that they follow 'physics' laws and have speed and acceleration.
// 1. Notice the code doesn't accept movable particles with animations,
// you should be able to do it easily following the other particles guidelines.
// 2. Notice the code introduces a lifetime to all particles automatically,
// this can be removed, although I don't recommend it.

MovableParticle::MovableParticle(bool gravity, pair<float, float> startingforce, pair<float, float> startingposition)
{
	spd = startingforce;
	pos = startingposition;
	this->gravity = gravity;
	// Deactivate gravity on moving particles by sending 'false' on creation
	timer.Start();
}

void MovableParticle::Update() {
	// These are the simple physics formulas I used for the particles.
	float secs = timer.ReadSec();
	pos.first = pos.first + spd.first * timer.ReadSec();
	if (gravity) {
		pos.second = pos.second + spd.second * timer.ReadSec() + ((GRAVITY / 2) * (timer.ReadSec() * timer.ReadSec()));
	}
	else {
		pos.second = pos.second + spd.second * timer.ReadSec();
	}
	// You can be creative and use other movement functions so they follow new movement patterns!
	Draw();
	alive = IsAlive();
}

void MovableParticle::Draw() {
	App->render->Blit(texture, pos.first, pos.second, &anim.GetCurrentFrame());
}

bool MovableParticle::IsAlive() {
	bool ret = true;
	if (timer.ReadSec() >= lifetime) {
		ret = false;
	}
	else if (pos.first >= App->particlemanager->window_size.first || pos.second >= App->particlemanager->window_size.second || pos.first < 0 || pos.second < 0)
	{
		ret = false;
	}
	return ret;
}

// -----------------------
//     STATIC BUCLES
// -----------------------
// Particles that stay in place for an indefinite amount
// of time repeating (often) the same animation.
// 1. The code makes all the animation print on the same place
// following the same pattern. This is not ideal, you might want your code to
// randomly print a different frame.
// 2. You can give it a lifetime on the xml and send 'true' on creation. Or
// You can send 'false' so it bucles forever.
//
// NOTICE that non finite particles that leave the screen are deleted, is that what your game needs?

StaticBucle::StaticBucle(pair<float, float> startingposition, bool argfinite)
{
	pos = startingposition;
	finite = argfinite;
	anim.loop = false;
	timer.Start();
}

void StaticBucle::Update()
{
	Draw();
	alive = IsAlive();
}

void StaticBucle::Draw()
{
	App->render->Blit(texture, pos.first, pos.second, &anim.GetCurrentFrame());
}

bool StaticBucle::IsAlive()
{
	bool ret = true;
	if (timer.ReadSec() >= lifetime && finite == true) {
		ret = false;
	}
	else if (pos.first >= App->particlemanager->window_size.first || pos.second >= App->particlemanager->window_size.second || pos.first < 0 || pos.second < 0)
	{
		ret = false;
	}
	return ret;
}
// -----------------------
//     EMITTER
// -----------------------
// Emitters are tools that automatically create movable particles to (more or less)
// randomised directions.
//
// 1. This Emetter ALWAYS creates finite particles. 'finite' on creation stands for
// the lifetime of the emetter itself.
// 2. #define EMITTER_SPEED at the beginning of the ParticleSystem.h define
// the frequency of particle creation. It works with the dt send to the module in the Update.
// 3. Emitters should always stop working outside the camera.
// 5. SetPos and SetSpd functions can help you to create travelling emetters
// for example: smoke from a rocket, thrown spells, etc

Emitter::Emitter(pair<float, float> startingposition, bool finite, float duration) : pos(startingposition), finite(finite), lifetime(duration)
{
}

void Emitter::Update(float dt)
{
	speed += dt;
	if (speed > EMITTER_SPEED)
	{

		// Ideally you want to set up your own forces depending on the emitter.
		force.first = (float)(rand() % 8 + 1);
		bool negative = rand() % 2;
		if (negative) force.first *= -1;
		force.first += speed_orig.first;
		// speed_orig is 0 unless stated otherwise with the SetSpd() method.
		force.second = (float)(rand() % 8 + 1);
		negative = rand() % 2;
		if (negative) force.second *= -1;
		force.second += speed_orig.second;
		//
		App->particlemanager->CreateMovableParticle(pos, force, true, type);
		speed = 0;
	}
	alive = IsAlive();
}

bool Emitter::IsAlive()
{
	bool ret = true;
	if (timer.ReadSec() >= lifetime && finite == true) {
		ret = false;
	}
	return ret;
}

void Emitter::SetPos(pair<float, float> pos)
{
	this->pos = pos;
}

void Emitter::SetSpd(pair<float, float> extra_speed)
{
	speed_orig = extra_speed;
}

bool ParticleManager::LoadParticleData()
{
	bool ret = false;
	pugi::xml_document ParticleDataFile;
	pugi::xml_node ParticleData;

	ParticleData = App->LoadParticleDataFile(ParticleDataFile);

	if (ParticleData.empty() == false)
	{
		for (pugi::xml_node node = ParticleData.child("particle"); node; node = node.next_sibling("particle"))
		{
			string name(node.child("Info").child("Name").attribute("value").as_string());
			int id = node.child("Info").child("ID").attribute("value").as_int();
			uint states = node.child("Info").child("States").attribute("value").as_uint();
			string path(node.child("Textures").child("Texture").attribute("value").as_string());
			int lifespan = node.child("Stats").child("Lifespan").attribute("value").as_int();
			int w = node.child("Info").child("Collider").attribute("w").as_int();
			int h = node.child("Info").child("Collider").attribute("h").as_int();
			int rows = node.child("Animations").child("Animation").attribute("rows").as_int();
			int columns = node.child("Animations").child("Animation").attribute("columns").as_int();
			// -----------------------------
			SDL_Rect our_rect{ 0,0,w,h };
			Animation anim;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					our_rect.x = j* w;
					our_rect.y = i* h;
					anim.PushBack(our_rect);
				}
				our_rect.x = 0;
			}
			anim.loop = true;
			anim.speed = 0.2f;

			ParticleInfo curr(name, path, id, lifespan, anim, states);
			info.push_back(curr);
		}
	}
	return ret;
}
