#include "Application.h"
#include "FileSystem.h"
#include "Audio.h"
#include "p2Log.h"
#include "Unit.h"
#include "Render.h"
#include <sstream>

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

Audio::Audio() : Module()
{
	music = NULL;
	name = "audio";
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	App->audio->LoadFx("audio/fx/fx_button_click.wav");

	App->audio->LoadFx("audio/fx/Creation_Unit.wav");
	App->audio->LoadFx("audio/fx/relic.wav");
	App->audio->LoadFx("audio/fx/Creation_Villager.wav");

	App->audio->LoadFx("audio/fx/Dead_1.wav");
	App->audio->LoadFx("audio/fx/Dead_2.wav");
	App->audio->LoadFx("audio/fx/Dead_3.wav");
	App->audio->LoadFx("audio/fx/Dead_4.wav");
	App->audio->LoadFx("audio/fx/Dead_5.wav");
	App->audio->LoadFx("audio/fx/Dead_6.wav");

	App->audio->LoadFx("audio/fx/Dead_Horse_1.wav");
	App->audio->LoadFx("audio/fx/Dead_Horse_2.wav");
	App->audio->LoadFx("audio/fx/Dead_Horse_3.wav");

	App->audio->LoadFx("audio/fx/Fight_1.wav");
	App->audio->LoadFx("audio/fx/Fight_2.wav");
	App->audio->LoadFx("audio/fx/Fight_3.wav");
	App->audio->LoadFx("audio/fx/Fight_4.wav");
	App->audio->LoadFx("audio/fx/Fight_5.wav");
	App->audio->LoadFx("audio/fx/Fight_6.wav");
	App->audio->LoadFx("audio/fx/Fight_7.wav");
	App->audio->LoadFx("audio/fx/Fight_8.wav");

	App->audio->LoadFx("audio/fx/arrow1.wav");
	App->audio->LoadFx("audio/fx/arrow2.wav");
	App->audio->LoadFx("audio/fx/arrow3.wav");
	App->audio->LoadFx("audio/fx/arrow4.wav");
	App->audio->LoadFx("audio/fx/arrow5.wav");
	App->audio->LoadFx("audio/fx/arrow6.wav");
	App->audio->LoadFx("audio/fx/arrow7.wav");

	App->audio->LoadFx("audio/fx/Horse_Select_1.wav");
	App->audio->LoadFx("audio/fx/Horse_Select_2.wav");
	App->audio->LoadFx("audio/fx/Horse_Select_3.wav");

	App->audio->LoadFx("audio/fx/Select_Unit_1.wav");
	App->audio->LoadFx("audio/fx/Select_Unit_2.wav");
	App->audio->LoadFx("audio/fx/Select_Unit_3.wav");
	App->audio->LoadFx("audio/fx/Select_Unit_4.wav");

	App->audio->LoadFx("audio/fx/populationlimit.wav");

	App->audio->LoadFx("audio/fx/farming.wav");

	App->audio->LoadFx("audio/fx/mine1.wav");
	App->audio->LoadFx("audio/fx/mine2.wav");
	App->audio->LoadFx("audio/fx/mine3.wav");


	App->audio->LoadFx("audio/fx/build1.wav");
	App->audio->LoadFx("audio/fx/build2.wav");

	App->audio->LoadFx("audio/fx/buildingdeath1.wav");
	App->audio->LoadFx("audio/fx/buildingdeath2.wav");
	App->audio->LoadFx("audio/fx/buildingdeath3.wav");
	App->audio->LoadFx("audio/fx/buildingdeath4.wav");

	App->audio->LoadFx("audio/fx/femaledeath6.wav");

	return ret;
}

void Audio::PlayUnitDeadSound(Unit* unit) {

	if (App->render->CullingCam(unit->entityPosition))
	{
		if (unit->type == ELVEN_CAVALRY || unit->type == GONDOR_KNIGHT || unit->type == ROHAN_KNIGHT || unit->type == MOUNTED_DUNEDAIN)
			App->audio->PlayFx(rand() % ((HORSE_DEAD_3 - HORSE_DEAD_1) + 1) - HORSE_DEAD_1 - 1);
		else if (unit->type == ELF_VILLAGER)
			App->audio->PlayFx(FEMALE_DEATH_6 - 1);
		else
			App->audio->PlayFx(rand() % ((DEAD_SOUND_6 - DEAD_SOUND_1) + 1) - DEAD_SOUND_1 - 1);
	}
}

void Audio::PlayFightSound(Unit* unit) {

	if (App->render->CullingCam(unit->entityPosition))
	{
		if (unit->type == ORC_ARCHER || unit->type == ELVEN_ARCHER || unit->type == ELVEN_CAVALRY || unit->type == DUNEDAIN_RANGE)
			App->audio->PlayFx(rand() % ((BOW_ATTACK_7 - BOW_ATTACK_1) + 1) + BOW_ATTACK_1 - 1);
		else
			App->audio->PlayFx(rand() % ((SWORD_ATTACK_8 - SWORD_ATTACK_1) + 1) + SWORD_ATTACK_1 - 1);
	}
}

void Audio::PlaySelectSound(Unit* unit) {
	if (App->render->CullingCam(unit->entityPosition))
	{
		if (unit->type == ELVEN_CAVALRY || unit->type == GONDOR_KNIGHT || unit->type == ROHAN_KNIGHT || unit->type == MOUNTED_DUNEDAIN)
			App->audio->PlayFx(rand() % ((SELECT_HORSE_3 - SELECT_HORSE_1) + 1) + SELECT_HORSE_1 - 1);
		else
			App->audio->PlayFx(rand() % ((SELECT_UNIT_4 - SELECT_UNIT_1) + 1) + SELECT_UNIT_1 - 1);
	}
}

void Audio::PlayGatherSound(Resource* resource) {
	if (App->render->CullingCam(resource->entityPosition))
	{
		// Falta posar wood

		if (resource->contains == FOOD)
			App->audio->PlayFx(FARMING - 1);
		else if (resource->contains == GOLD)
			App->audio->PlayFx(rand() % ((MINE_3 - MINE_1) + 1) + MINE_1 - 1);
		else if (resource->contains == STONE)
			App->audio->PlayFx(SWORD_ATTACK_7 - 1);
	}
}

// Called before quitting
bool Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	for (vector<Mix_Chunk*>::iterator it = fx.begin(); it != fx.end(); it++) {
		Mix_FreeChunk(*it);
	}

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS_RW(App->fs->Load(path), 1);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(int fx_id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (fx_id > 0 && fx_id <= fx.size())
	{
		Mix_PlayChannel(-1, fx[fx_id - 1], repeat);
	}

	return ret;
}