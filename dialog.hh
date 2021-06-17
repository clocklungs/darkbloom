#ifndef DIALOG_HH
#define DIALOG_HH

#include <string>
#include "var.hh"
#include "button.hh"

#define LINESIZE 80


extern VAR_paramList dialog_vars;


typedef struct Choice {
  std::string text;
  int value;
  bool active;
  Button button;
} Choice;


int playDialog(const char *filename);
int fgetline(char *string, int max, FILE *stream);
int getString(char *dest, const char *src);
int processVar(char *string, VAR_paramList &vars);
const char* interpolateVars(char *string, VAR_paramList &vars);

#endif
