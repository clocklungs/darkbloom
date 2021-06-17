#include <string.h>
#include "script.hh"
#include "dialog.hh"
#include "world.hh"
#include "plyr.hh"
#include "video.hh"
#include "sound.hh"
#include "battle.hh"


/*global variables*/
VAR_paramList script_vars;


/*function declarations*/
int gotoSection(int section, FILE *script, int &linenum, char line[LINESIZE]);
int processEval(char *string, FILE *script, const char *filename, int &linenum,
                char line[LINESIZE]);


bool runScript(const char *filename, Object *who, Object *what)
{
  FILE *script;
  std::string output;
  int done;
  int length;
  int section;
  char line[LINESIZE+1];
  char property[LINESIZE+1];
  char value[LINESIZE+1];
  int linenum;
  int temp;
  Mix_Chunk *sound;

  sound = NULL;

  /*open script file in text mode for reading*/
  script = fopen(filename, "rt");

  if(script == NULL)
  {
    perror("script error");
    return false;
  }

  linenum = 0;
  done = 0;
  while(!done && !feof(script))
  {
    length = fgetline(line, LINESIZE, script);
    linenum++;

    if(length < 0) done = 1;
    if(length == 0 || line[0]=='#') continue;
    if(done) break;

    switch(line[0])
    {
      /*boss battle*/
      case 'B':
        if(!sscanf(interpolateVars(&line[1], script_vars), "%i", &temp))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid enemy count\n");
          done = -1;
          break;
        }
        if(!battleloop((temp%3), 1)) done = -1;
        break;
      /*battle*/
      case 'b':
        printf("BATTLE\n");
        if(!sscanf(interpolateVars(&line[1], script_vars), "%i", &temp))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid enemy count\n");
          done = -1;
          break;
        }
        if(!battleloop((temp%5), 0)) done = -1;
        break;
      /*switch level*/
      case 'L':
        world.load(&line[1]);
        break;
      /*set player position*/
      case 'P':
        if(sscanf(interpolateVars(&line[1], script_vars),
           "%i%i", &player.x, &player.y) < 2)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid player coordinates\n");
          done = -1;
          break;
        }
        break;
      /*dialog*/
      case 'D':
        flushEvents();
        temp = playDialog(interpolateVars(&line[1], script_vars));
        if(temp < 0) done = -1;
        else script_vars.set("DIALOG_RETVAL", "int", &temp);
        break;
      /*delay*/
      case 'd':
        if(!sscanf(interpolateVars(&line[1], script_vars), "%i", &temp))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid delay specifier\n");
          done = -1;
          break;
        }
        delay(temp);
        break;
      /*set music*/
      case 'M':
        playMusic(interpolateVars(&line[1], script_vars));
        break;
      /*play sound effect*/
      case 'S':
        sound = playFile(interpolateVars(&line[1], script_vars));
        break;
      /*set property of 'who' object*/
      case 'W':
        if(who == NULL) break;
        if(sscanf(interpolateVars(&line[1], dialog_vars), "%80s = %80s",
           property, value) < 2)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid property=value specification\n");
          done = -1;
          break;
        }
        if(!strcmp(property, "visible"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          who->visible = temp;
          break;
        }
        if(!strcmp(property, "persistent"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          who->persistent = temp;
          break;
        }
        if(!strcmp(property, "active"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          who->active = temp;
          break;
        }
        if(!strcmp(property, "collidable"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          who->collidable = (Collision)temp;
          break;
        }
        if(!strcmp(property, "a_script"))
        {
          who->a_script = value;
          break;
        }
        if(!strcmp(property, "c_script"))
        {
          who->c_script = value;
          break;
        }
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "invalid property=value specification\n");
        fprintf(stderr, "%s = %s\n", property, value);
        done = -1;
        break;
      /*set property of 'what' object*/
      case 'w':
        if(what == NULL) break;
        if(sscanf(interpolateVars(&line[1], dialog_vars), "%80s = %80s",
           property, value) < 2)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid property=value specification\n");
          fprintf(stderr, "%s = %s\n", property, value);
          done = -1;
          break;
        }
        if(!strcmp(property, "visible"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          what->visible = temp;
          break;
        }
        if(!strcmp(property, "persistent"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          what->persistent = temp;
          break;
        }
        if(!strcmp(property, "active"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          what->active = temp;
          break;
        }
        if(!strcmp(property, "collidable"))
        {
          if(!sscanf(value, "%i", &temp))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "invalid property=value specification\n");
            fprintf(stderr, "%s = %s\n", property, value);
            done = -1;
            break;
          }
          what->collidable = (Collision)temp;
          break;
        }
        if(!strcmp(property, "a_script"))
        {
          if(!strcmp(value, "NONE")) value[0] = '\0';
          what->a_script = value;
          break;
        }
        if(!strcmp(property, "c_script"))
        {
          if(!strcmp(value, "NONE")) value[0] = '\0';
          what->c_script = value;
          break;
        }
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "invalid property=value specification\n");
        fprintf(stderr, "%s = %s\n", property, value);
        done = -1;
        break;
      /*set variable*/
      case '$':
        done = processVar(&line[1], script_vars);
        switch(done)
        {
        case -1:
        case -2:
        case -3:
          fprintf(stderr, "line %.3i: no var specified\n", linenum);
          break;
        case -4:
          fprintf(stderr, "line %.3i: no type specified\n", linenum);
          break;
        case -5:
          fprintf(stderr, "line %.3i: no value specified\n", linenum);
          break;
        case -6:
          fprintf(stderr, "line %.3i: bad value for var type\n", linenum);
          break;
        case -7:
          fprintf(stderr, "line %.3i: invalid var type\n", linenum);
          break;
        }
        //script_vars.debug();
        break;
      /*section identifier, we can skip these at this point*/
      case '[':
        break;
      /*exit code*/
      case 'E':
        if(!sscanf(interpolateVars(&line[1], script_vars), "%i", &done))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "malformed exitcode\n");
          done = -1;
        }
        break;
      /*jump*/
      case '@':
        if(!sscanf(interpolateVars(&line[1], script_vars), "%i", &section))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "malformed jump specifier\n");
          done = -1;
          break;
        }
        if(!gotoSection(section, script, linenum, line))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "target section %i does not exist\n", section);
          done = -1;
        }
        break;
      /*eval*/
      case '!':
        done = processEval(&line[1], script, filename, linenum, line);
        break;
      default:
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "invalid starting character: %c\n", line[0]);
        done = -1;
    }
  }

  fclose(script);
  /*free sound sample, if any*/
  if(sound != NULL)
  {
    Mix_FreeChunk(sound);
    sound = NULL;
  }
  return (done > 0);
}


bool runScript(Object &who, Object &what, Why why)
{
//  printf("WHO: %s\n", who.name.c_str());
//  printf("WHAT: %s\n", what.name.c_str());
  if(why == ACTIVATION)
  {
//    printf("A_SCRIPT: %s\n", what.a_script.c_str());
    if(!what.a_script.length()) return true;
    return runScript(what.a_script.c_str(), &who, &what);
  }

//  printf("C_SCRIPT: %s\n", what.c_script.c_str());
  if(!what.c_script.length()) return true;
  return runScript(what.c_script.c_str(), &who, &what);
}


int gotoSection(int section, FILE *script, int &linenum, char line[LINESIZE])
{
  int length;
  int curr;

  if(script == NULL) return -1;
  rewind(script);

  linenum = 0;
  while(!feof(script))
  {
    length = fgetline(line, LINESIZE, script);
    linenum++;
    if(length < 0) return 0;
    if(line[0] != '[') continue;
    if(sscanf(&line[1], "%i", &curr))
    {
      if(curr == section) return 1;
    }
  }

  return 0;
}


int processEval(char *string, FILE *script, const char *filename, int &linenum,
                char line[LINESIZE])
{
  char param[LINESIZE];
  char *search;
  int i;
  int len;
  int length;
  VAR_param *var;

  if(*(string++) != '$' || *(string++) != '(')
  {
    fprintf(stderr, "%s:%i: error: ", filename, linenum);
    fprintf(stderr, "malformed eval\n");
    return -1;
  }

  search = strchr(string, ')');
  if(search == NULL)
  {
    fprintf(stderr, "%s:%i: error: ", filename, linenum);
    fprintf(stderr, "malformed eval\n");
    return -1;
  }
  
  len = search - string;
  if(len <= 0)
  {
    fprintf(stderr, "%s:%i: error: ", filename, linenum);
    fprintf(stderr, "malformed eval\n");
    return -1;
  }

  for(i=0; i<len; ) param[i++] = *(string++);
  param[i] = '\0';

  var = script_vars.get(param);
  if(var == NULL || var->eval() == false)
  {
    while(!feof(script))
    {
      length = fgetline(line, LINESIZE, script);
      linenum++;
      if(length < 0)
      {
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "no statement after eval\n");
        return -1;
      }
      if(length == 0 || line[0]=='#') continue;
      break;
    }
  }
  return 0;
}


/*EOF*/
