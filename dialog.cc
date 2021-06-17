#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dialog.hh"
#include "dbox.hh"
#include "video.hh"


/*function declarations*/
int dialogChoice(const char *title, Choice choices[DBOX_CHOICES]);
void dialogFlush();
void dialogAdd(const char *string);
int gotoSection(int section);
int processChoice();
int processEval(char *string);


/*global variables*/
VAR_paramList dialog_vars;
FILE *in;
int linenum;
char line[LINESIZE+1];
const char *filename;


int playDialog(const char *name)
{
  std::string output;
  int done;
  int length;
  int exitcode;
  int section;

  filename = name;

  /*open text file for reading*/
  in = fopen(filename, "rt");

  if(in == NULL)
  {
    fprintf(stderr, "error opening file: %s\n", filename);
    return -1;
  }

  /*clear any left over crud in the dialog system, shouldn't be any*/
  dbox_line = 0;
  dialogFlush();

  exitcode = 0;
  linenum = 0;
  done = 0;
  while(!feof(in))
  {
    length = fgetline(line, LINESIZE, in);
    linenum++;
    if(length < 0) done = 1;
    if(length == 0 || line[0]=='#') continue;
    if(done) break;

    switch(line[0])
    {
      /*set variable*/
      case '$':
        done = processVar(&line[1], dialog_vars);
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
        //dialog_vars.debug();
        break;
      /*section identifier, we can skip these at this point*/
      case '[':
        break;
      /*texty text, how fun*/
      case '"':
        output = interpolateVars(&line[1], dialog_vars);
        dialogAdd(output.c_str());
        break;
      /*beginning of a choice section*/
      case '?':
        done = processChoice();
        break;
      /*exit code*/
      case 'E':
        dialogFlush();
        done = sscanf(interpolateVars(&line[1], dialog_vars), "%i", &exitcode);
        if(done == 0)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "malformed exitcode\n");
          done = -1;
        }
        break;
      /*jump*/
      case '@':
        if(!sscanf(interpolateVars(&line[1], dialog_vars), "%i", &section))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "malformed jump specifier\n");
          done = -1;
          break;
        }
        if(!gotoSection(section))
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "target section %i does not exist\n", section);
          done = -1;
        }
        break;
      /*flush*/
      case '^':
        dialogFlush();
        break;
      /*literal string*/
      case '\'':
        output = &line[1];
        dialogAdd(output.c_str());
        break;
      /*eval*/
      case '!':
        done = processEval(&line[1]);
        break;
      default:
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "invalid starting character: %c\n", line[0]);
        done = 1;
    }
    if(done) break;
  }

  if(done < 0) fprintf(stderr, "%s\n", line);

  fclose(in);
  updateDisplay();
  return exitcode;
}


/*doy! wrote this before noticing that fgets DOES stop if it hits an EOL
however, this one returns the length, which i use above, so haven't
replaced it.*/
int fgetline(char *string, int num, FILE *stream)
{
  int size;

  if(string == NULL) return -1;
  if(num <= 0) return -2;
  if(stream == NULL) return -3;

  size = 0;
  do
  {
    *(string) = fgetc(stream);
    if(feof(stream))
    {
      *(string) = '\0';
      return size;
    }
  }
  while(*(string++)!='\n' && ++size<num);

  if(size != num) string--;
  *(string) = '\0';

  return size;
}


int getString(char *dest, const char *src)
{
  int size;

  size = 0;
  while(isspace(*(src))) src++;
  while(*(src)!='\0' && !isspace(*(src)))
  {
    *(dest++) = *(src++);
    size++;
  }
  *(dest) = '\0';
  return size;
}


int processVar(char *string, VAR_paramList &vars)
{
  char param[LINESIZE];
  char type[LINESIZE];
  char boolval[LINESIZE];
  char buffer[LINESIZE];
  char *search;
  int len, i, d_int;
  bool d_bool;
  float d_float;
  VAR_param *var;

  if(*(string++) != '(') return -1;

  search = strchr(string, ')');
  if(search == NULL) return -2;

  len = search - string;
  if(len <= 0) return -3;

  for(i=0; i<len; ) param[i++] = *(string++);
  param[i] = '\0';

  len = getString(type, ++string);
  if(len == 0) return -4;

  for(i=0; i<len; ++i) ++string;
  ++string;

  if(*(string++) == '\0') return -5;

  if(strcmp(type, "string") == 0)
  {
    var = vars.set(param, type, string);
    if(var != NULL)
    {
      var->toString(buffer, LINESIZE);
      var->setData(VAR_STRING, interpolateVars(buffer, dialog_vars));
    }
  }
  else
    if(strcmp(type, "bool") == 0)
    {
      len = getString(boolval, interpolateVars(string, dialog_vars));
      if(len == 0) return -6;
      if(strcmp(boolval, "false") == 0) d_bool = 0;
      else
        if(strcmp(boolval, "true") == 0) d_bool = 1;
        else return -6;
      vars.set(param, type, &d_bool);
    }
    else
      if(strcmp(type, "int") == 0)
      {
        len = sscanf(interpolateVars(string, dialog_vars), "%i", &d_int);
        if(len == 0) return -6;
        vars.set(param, type, &d_int);
      }
      else
        if(strcmp(type, "float") == 0)
        {
          len = sscanf(interpolateVars(string, dialog_vars), "%f", &d_float);
          if(len == 0) return -6;
          vars.set(param, type, &d_float);
        }
        else
        {
          return -7;
        }

  return 0;
}


const char* interpolateVars(char *string, VAR_paramList &vars)
{
  std::string output;
  std::string param;
  char *temp, *search;
  char buffer[LINESIZE];
  VAR_param *var;
  int len, i;

  while(*(string)!='\0')
  {
    /*we might have a var*/
    if(*(string) == '$')
    {
      temp = string;
      if(*(++temp) == '(')
      {
        search = strchr(temp, ')');
        if(search != NULL)
        {
          len = search - temp;
          if(len > 1)
          {
            for(i=1; i<len; ++i) param += *(++temp);
            var = vars.get(param.c_str());
            if(var != NULL)
            {
            output += var->toString(buffer, LINESIZE);
            string = ++temp;
            ++string;
            continue;
            }
          }
        }
      }
    }
    output += *(string++);
  }
  return output.c_str();
}


int dialogChoice(const char *title, Choice choices[DBOX_CHOICES])
{
  dialogFlush();
  return dboxChoice(title, choices);
}


void dialogFlush()
{
  int i;

  /*if there is something to flush*/
  if(dbox_line > 0) dboxText();

  /*clear previous lines, if any*/
  for(i=0; i<DBOX_MAXLINES; ++i) dbox_lines[i][0] = '\0';
  dbox_line = 0;
}


/*adds the string to the dialogbox as text, attempting to wrap at the
last word that will fit on a line*/
void dialogAdd(const char *string)
{
  int i;
  const char *start;
  const char *end;

  /*if we already have as many lines as allowed, flush them first*/
  if(dbox_line >= DBOX_MAXLINES) dialogFlush();

  start = string;
  end = string;

  /*find the last space character on this line*/
  for(i=0; i<DBOX_TEXTMAX && *(string)!='\0'; ++i)
  {
    if(isspace(*(string++))) end = string;
  }

  /*there were no spaces or we fit the whole string in*/
  if(end == start || string-start < DBOX_TEXTMAX) end = string;

  strncpy(dbox_lines[dbox_line], start, end-start);
  dbox_lines[dbox_line][end-start] = '\0';

  /*mark that we've filled up another line*/
  dbox_line++;
  /*if we still have more, make a recursive call*/
  if(*(end)!='\0') dialogAdd(end);
}


int gotoSection(int section)
{
  int length;
  int curr;

  rewind(in);

  linenum = 0;
  while(!feof(in))
  {
    length = fgetline(line, LINESIZE, in);
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


int processChoice()
{
  std::string menutitle;
  Choice choices[DBOX_CHOICES];
  int choice;
  int end;
  int i;
  int length;
  int section;
  int retval;

  if(line[1] != '?')
  {
    fprintf(stderr, "%s:%i: error: ", filename, linenum);
    fprintf(stderr, "choice outside of choice menu\n");
    return -1;
  }
  /*get menu title from rest of line*/
  menutitle = interpolateVars(&line[2], dialog_vars);

  /*now lets get the choices*/
  end = 0;
  choice = 0;
  for(i=0; i<DBOX_CHOICES; ++i) choices[i].active = false;
  while(!feof(in))
  {
    length = fgetline(line, LINESIZE, in);
    linenum++;
    if(length < 0) end = 1;
    if(length == 0 || line[0]=='#') continue;
    if(end) break;
    switch(line[0])
    {
      /*set variable*/
      case '$':
        retval = processVar(&line[1], dialog_vars);
        switch(retval)
        {
        case -1:
        case -2:
        case -3:
          fprintf(stderr, "line %.3i: no var specified\n", linenum);
          return retval;
        case -4:
          fprintf(stderr, "line %.3i: no type specified\n", linenum);
          return retval;
        case -5:
          fprintf(stderr, "line %.3i: no value specified\n", linenum);
          return retval;
        case -6:
          fprintf(stderr, "line %.3i: bad value for var type\n", linenum);
          return retval;
        case -7:
          fprintf(stderr, "line %.3i: invalid var type\n", linenum);
          return retval;
        }
        break;
      case '?':
        /*end of choice section*/
        if(line[1] == '?')
        {
          if(choice<DBOX_CHOICES && (!choice || choices[choice].active))
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "no or incomplete choice\n");
            return -1;
          }
          end = 1;
          break;
        }
        choices[choice].text = interpolateVars(&line[1], dialog_vars);
        if(choices[choice].active)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "invalid char in choice section\n");
          return -1;
        }
        choices[choice].active = true;
        break;
      case '>':
        if(choices[choice].active == false)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "choice value with no choice\n");
          return -1;
        }
        end = !sscanf(&line[1], "%i", &choices[choice].value);
        if(end)
        {
          fprintf(stderr, "%s:%i: error: ", filename, linenum);
          fprintf(stderr, "malformed choice value\n");
          return -1;
        }
        else 
        {
          if(++choice > DBOX_CHOICES)
          {
            fprintf(stderr, "%s:%i: error: ", filename, linenum);
            fprintf(stderr, "more than %i choices\n", DBOX_CHOICES);
            return -1;
          }
        }
        break;
      default:
        fprintf(stderr, "%s:%i: error: ", filename, linenum);
        fprintf(stderr, "invalid char in choice section\n");
        return -1;
    }
    if(end) break;
  }

  section = dialogChoice(menutitle.c_str(), choices);
  if(!gotoSection(section))
  {
    fprintf(stderr, "%s:%i: error: ", filename, linenum);
    fprintf(stderr, "target section %i does not exist\n", section);
    return -1;
  }
  return 0;
}


int processEval(char *string)
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

  var = dialog_vars.get(param);
  if(var == NULL || var->eval() == false)
  {
    while(!feof(in))
    {
      length = fgetline(line, LINESIZE, in);
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
