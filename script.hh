#ifndef SCRIPT_HH
#define SCRIPT_HH

#include "var.hh"
#include "object.hh"

extern VAR_paramList script_vars;
extern VAR_paramList sidebar_vars;
extern VAR_paramList dialog_vars;

typedef enum Why {COLLISION=0, ACTIVATION=1};

bool runScript(const char *filename, Object *who=NULL, Object *what=NULL);
bool runScript(Object &who, Object &what, Why why=ACTIVATION);

#endif
