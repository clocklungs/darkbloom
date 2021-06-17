#include <stdlib.h>
#include "sound.hh"
#include "config.hh"
#include "dbox.hh"


int initSound()
{
  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 512) < 0)
  {
    fprintf(stderr, "error opening audio: %s\n", Mix_GetError());
    config.sounds = 0;
    config.music = 0;
    return 0;
  }
  atexit(Mix_CloseAudio);
  atexit(freeMusic);

  /*16 seems to be a good/common number*/
  Mix_AllocateChannels(16);

  samples[MENUBLIP] = Mix_LoadWAV("sounds/menu_change.ogg");
  if(samples[MENUBLIP] == NULL)
  {
    fprintf(stderr, "error loading sample: %s\n", Mix_GetError());
    config.sounds = 0;
  }

  return playMusic("sounds/midori_hirano_-_i_was.ogg");
}


int playMusic(const char *filename)
{
  /*make sure to free the old music first*/
  if(music != NULL)
  {
    /*stop the music*/
    Mix_HaltMusic();
    /*free the music*/
    Mix_FreeMusic(music);
    music = NULL;
  }

  /*load music*/
  music = Mix_LoadMUS(filename);
  if(music == NULL)
  {
    fprintf(stderr, "error loading music: %s\n", Mix_GetError());
    return -1;
  }

  if(config.music)
  {
    if(Mix_PlayMusic(music, -1) < 0)
    {
      fprintf(stderr, "error playing music: %s\n", Mix_GetError());
      config.music = 0;
      return -1;
    }
  }

  return 1;
}


void toggleSounds()
{
  if(config.sounds) Mix_HaltChannel(-1);
  config.sounds = !config.sounds;
}


void toggleMusic()
{
  config.music = !config.music;
  if(config.music)
  {
    if(Mix_PlayMusic(music, -1) < 0)
    {
      fprintf(stderr, "error playing music: %s\n", Mix_GetError());
      config.music = 0;
    }
  }
  else Mix_HaltMusic();
}


void freeMusic()
{
  int i;

  /*halt all sounds*/
  Mix_HaltChannel(-1);
  for(i=0; i<SAMPLES; ++i)
  {
    /*free sound chunks*/
    Mix_FreeChunk(samples[i]);
    samples[i] = NULL;
  }

  /*free dialog box sound, if any loaded*/
  if(dbox_snd != NULL)
  {
    Mix_FreeChunk(dbox_snd);
    dbox_snd = NULL;
  }

  /*stop the music*/
  Mix_HaltMusic();
  /*free the music*/
  Mix_FreeMusic(music);
  music = NULL;
}


void playSample(int sample, int loops)
{
  /*if sounds are enabled*/
  if(config.sounds)
  {
    /*check for a valid sample*/
    if(sample >= SAMPLES || samples[sample] == NULL)
    {
      fprintf(stderr, "invalid sample/sample not loaded: %d\n", sample);
      return;
    }
    /*try to play the sample*/
    if(Mix_PlayChannel(-1, samples[sample], loops) == -1)
    {
      fprintf(stderr, "error playing sample: %s\n", Mix_GetError());
      /*disable sounds*/
      config.sounds = 0;
    }
  }
}


Mix_Chunk* playFile(const char *filename, int loops)
{
  Mix_Chunk *temp;

  /*try to load the file*/
  temp = Mix_LoadWAV(filename);
  if(temp == NULL)
  {
    fprintf(stderr, "error loading sample: %s\n", Mix_GetError());
    temp = NULL;
  }
  else
  {
    /*try to play the sample*/
    if(Mix_PlayChannel(-1, temp, loops) == -1)
    {
      fprintf(stderr, "error playing sample: %s\n", Mix_GetError());
      Mix_FreeChunk(temp);
      temp = NULL;
    }
  }

  /*return the loaded (playing) chunk*/
  return temp;
}


/*EOF*/
