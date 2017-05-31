#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;
class Unit;

enum FX {
	BUTTON_SOUND = 1,

	CREATE_UNIT_SOUND,
	CREATE_VILLAGER_SOUND,

	DEAD_SOUND_1,
	DEAD_SOUND_2,
	DEAD_SOUND_3,
	DEAD_SOUND_4,
	DEAD_SOUND_5,
	DEAD_SOUND_6,

	HORSE_DEAD_1,
	HORSE_DEAD_2,
	HORSE_DEAD_3,

	SWORD_ATTACK_1,
	SWORD_ATTACK_2,
	SWORD_ATTACK_3,
	SWORD_ATTACK_4,
	SWORD_ATTACK_5,
	SWORD_ATTACK_6,
	SWORD_ATTACK_7,
	SWORD_ATTACK_8,

	BOW_ATTACK,

	SELECT_HORSE_1,
	SELECT_HORSE_2,
	SELECT_HORSE_3,

	SELECT_UNIT_1,
	SELECT_UNIT_2,
	SELECT_UNIT_3,
	SELECT_UNIT_4,

};

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(int fx_id, int repeat = 0);
	void PlayDeadSound(Unit* unit);
	void PlayFightSound(Unit* unit);
	void PlaySelectSound(Unit* unit);

private:

	_Mix_Music*			music = NULL;
	vector<Mix_Chunk*>	fx;
};

#endif // __AUDIO_H__