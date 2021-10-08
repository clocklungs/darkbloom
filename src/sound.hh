#ifndef SOUND_HH
#define SOUND_HH

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define SAMPLES  1
#define MENUBLIP 0

extern Mix_Music *music;
extern Mix_Chunk *samples[SAMPLES];

int initSound();
int playMusic(const char *filename);
void toggleSounds();
void toggleMusic();
void freeMusic();
void playSample(int sample, int loops=0);
Mix_Chunk* playFile(const char *filename, int loops=0);

#endif
