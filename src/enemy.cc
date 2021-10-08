#include "enemy.hh"
#include <stdio.h>
#include <stdlib.h>

Enemy enemies[MAX_ENEMIES];
Enemy boss;
int enemy_count;

/*********************************************************************
Enemy::load()
  attempts to load an enemy definition from the specified file
  returns boolean indicating success
*********************************************************************/
bool Enemy::load(const char *filename) {
    FILE *in;
    char buffer[100];
    int line;

    // printf("LOADING ENEMY: %s\n", filename);

    /*open enemy definition in read/text mode*/
    in = fopen(filename, "rt");
    if (in == NULL) {
        perror(filename);
        return false;
    }

    line = 1;

    while (true) {
        if (fscanf(in, "name: %99s\n", buffer) != 1)
            break;
        name = buffer;
        line++;

        if (fscanf(in, "hp: %i\n", &hp) != 1)
            break;
        line++;

        if (fscanf(in, "strength: %i\n", &strength) != 1)
            break;
        line++;

        if (fscanf(in, "agility: %i\n", &agility) != 1)
            break;
        line++;

        if (fscanf(in, "xp: %i\n", &xp) != 1)
            break;
        line++;

        if (fscanf(in, "graphic_idle: %99s\n", buffer) != 1)
            break;
        graphic_idle = buffer;
        line++;

        if (fscanf(in, "graphic_attack: %99s\n", buffer) != 1)
            break;
        graphic_attack = buffer;
        line++;

        if (fscanf(in, "offset_x: %i\n", &offset_x) != 1)
            break;
        line++;

        if (fscanf(in, "offset_y: %i\n", &offset_y) != 1)
            break;
        line++;

        break;
    }

    fclose(in);

    if (line < 10) {
        fprintf(stderr, "%s:%i: malformed enemy definition\n", filename, line);
        return false;
    }

    return true;
}

/*********************************************************************
load_enemies()
  reads a file a line at a time and tries to load the enemy definition
  specified on that line;
  returns the number of enemies successfully loaded
*********************************************************************/
int load_enemies(const char *filename) {
    FILE *in;
    char buffer[100];
    int len;

    /*reset loaded area enemy count*/
    enemy_count = 0;

    /*open enemy list in read/text mode*/
    in = fopen(filename, "rt");
    if (in == NULL) {
        perror(filename);
        return enemy_count;
    }

    /*load up to MAX_ENEMIES enemy definitions*/
    while (enemy_count < MAX_ENEMIES) {
        /*stop if we hit the EOF*/
        if (fgets(buffer, 100, in) == NULL)
            break;
        /*strip trailing newline*/
        len = strlen(buffer);
        if (len)
            buffer[--len] = '\0';
        /*if the line is blank, skip*/
        if (!len)
            continue;
        /*only update enemy_count if loading was successful*/
        if (enemies[enemy_count].load(buffer))
            enemy_count++;
    }

    /*close filehandle*/
    fclose(in);

    return enemy_count;
}

/*********************************************************************
load_boss()
  wraps the call to Enemy::load() for the boss enemy.
  returns boolean indicating successful load of the boss definition
*********************************************************************/
bool load_boss(const char *filename) { return boss.load(filename); }

/*EOF*/
