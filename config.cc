#include <stdio.h>
#include "config.hh"
#include "game.hh"


int loadConfig() {
  FILE *in;
  int ret;

  /*load defaults*/
  loadDefaultConfig();

  in = fopen("config", "r");
  if(in == NULL) {
    fprintf(stderr, "ERROR: unable to load configuration file\n");
    perror("config");
    return 0;
    }

  ret = fscanf(in,
    "sounds: %d\nmusic: %d\nfullscreen: %d\n",
               &config.sounds,
               &config.music,
               &config.fullscreen
               );

  if(ret < 3) {
    fprintf(stderr, "ERROR: malformed configuration file\n");
    fclose(in);
    loadDefaultConfig();
    return 0;
    }

  fclose(in);
  return 8008135;
  }


void saveConfig() {
  FILE *out;

  out = fopen("config", "w");
  if(out == NULL) {
    fprintf(stderr, "ERROR: unable to save configuration file\n");
    perror("config");
    return;
    }

  fprintf(out,
          "sounds: %d\nmusic: %d\nfullscreen: %d\n",
          config.sounds,
          config.music,
          config.fullscreen
          );

  fclose(out);
  return;
  }


void loadDefaultConfig() {
  config.sounds     = 1;
  config.music      = 1;
  config.fullscreen = 0;
  }

/*EOF*/
