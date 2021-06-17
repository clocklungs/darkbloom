/*********************************************************************
variable handling structures

pcarley@lssu.edu
(c) trained circus idiot, ink.
this is GPL'ed software (see COPYING for license)

modified:
  Tue Jul 19 16:57:52 EDT 2005
    -VAR_param is now a class instead of struct
  Mon Jul 18 16:45:49 EDT 2005
    -changed for standalone VAR class
  Thu Jun 23 13:25:32 EDT 2005
    -original implementation started

*********************************************************************/

#ifndef VAR_HH
#define VAR_HH

#include <string>


extern const char* VAR_GetError();


typedef enum VAR_datatype {VAR_STRING, VAR_BOOL, VAR_INT, VAR_FLOAT};


typedef union VAR_paramData
{
  std::string *string_data;
  bool bool_data;
  int int_data;
  float float_data;
};


class VAR_param
{
  public:
    std::string key;
    VAR_datatype type;
    VAR_paramData data;
    bool persistent;
    VAR_param *next;
    
    VAR_param();
    VAR_param(const char *k);
    ~VAR_param();
    void setData(VAR_datatype datatype, const void *value);
    bool eval();
    char* toString(char *buffer, int num);
    int toInt();
};


/*this would make more sense as a tree, for speed
unless we ever start deleting individual parameters
but probably won't use enough vars to make much difference*/
class VAR_paramList
{
  private:
    VAR_param *head;
  public:
    VAR_paramList();
    ~VAR_paramList();
    VAR_param* set(const char *param,
                   const char *datatype,
                   const void *value);
    VAR_param* get(const char *param);
    VAR_param* persist(const char *param);
    bool eval(const char *param);
    char* toString(const char *param, char *buffer, int num);
    int toInt(const char *param);
    void flush();
    void clear();
    void debug();
};

#endif
