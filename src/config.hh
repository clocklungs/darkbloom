#ifndef CONFIG_HH
#define CONFIG_HH

typedef struct {
  int sounds;
  int music;
  int fullscreen;
  } Config;

extern Config config;

int loadConfig();
void saveConfig();
void loadDefaultConfig();

#endif
