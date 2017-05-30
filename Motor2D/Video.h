#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "Module.h"
#include "PerfTimer.h"

enum THEORAPLAY_VideoFormat;
struct THEORAPLAY_AudioPacket;
struct THEORAPLAY_Decoder;
struct THEORAPLAY_VideoFrame;

struct AudioQueue
{
	const THEORAPLAY_AudioPacket *audio;
	int offset;
	struct AudioQueue *next;
};

class Video : public Module
{
public:

	Video();

	// Destructor
	virtual ~Video();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Update videoframes and audiopackets
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a video file
	void PlayVideo(const char *fname);

	// Audio methods
	static void SDLCALL audio_callback(void *userdata, Uint8 *stream, int len);
	static void queue_audio(const THEORAPLAY_AudioPacket *audio);
	bool want_to_play;

private:
	void ResetValues();

	// Load video file
	void LoadVideo(const char *fname);

private:
	THEORAPLAY_Decoder* decoder;
	const THEORAPLAY_VideoFrame* video;
	const THEORAPLAY_AudioPacket* audio;
	SDL_Window* screen;
	SDL_Texture* texture;
	SDL_AudioSpec spec;
	SDL_Event event;

	Uint32 baseticks;
	Uint32 framems;
	int init_failed;
	int quit;

	void* pixels;
	int pitch;

	static AudioQueue* audio_queue;
	static AudioQueue* audio_queue_tail;

	SDL_Rect rendering_rect;

	bool videoPlaying = false;
	string videoName;
};

#endif // __VIDEO_H__
