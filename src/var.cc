/*********************************************************************
variable handling structures

pcarley@lssu.edu
(c) trained circus idiot, ink.
this is GPL'ed software (see COPYING for license)

modified:
  Tue Jul 19 23:04:56 EDT 2005
    -added setData(), toString() and eval() functions to VAR_param
  Tue Jul 19 13:21:16 EDT 2005
    -added flush() and get() functions to VAR_paramList
    -add() renamed to set() and vars are now type and value switched
    -removed some debugging statements
  Thu Jun 23 22:28:05 EDT 2005

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "var.hh"

std::string var_error;

const char* VAR_GetError()
{
  return var_error.c_str();
}


void VAR_SetError(const char* error)
{
  var_error = error;
}


/*********************************************************************
VAR_param constructor
  initializes the structure as empty
*********************************************************************/
VAR_param::VAR_param()
{
  key = "";
  type = VAR_STRING;
  data.string_data = NULL;
  persistent = false;
  next = NULL;
}


/*********************************************************************
VAR_param constructor
  initializes the structure with the appropriate keyvalue
*********************************************************************/
VAR_param::VAR_param(const char *k)
{
  key = k;
  type = VAR_STRING;
  data.string_data = NULL;
  persistent = false;
  next = NULL;
}


/*********************************************************************
VAR_param destructor
  frees up the allocated string if any
*********************************************************************/
VAR_param::~VAR_param()
{
  if(type == VAR_STRING && data.string_data != NULL)
  {
    delete data.string_data;
    data.string_data = NULL;
  }
}


/*********************************************************************
VAR_param setData()
  sets the data type and value
*********************************************************************/
void VAR_param::setData(VAR_datatype datatype, const void *value)
{
  /*delete old string, if there was one*/
  if(type==VAR_STRING && data.string_data!=NULL) delete data.string_data;

  type = datatype;
  switch(type)
  {
    case VAR_STRING:
      data.string_data = new std::string;
      if(value == NULL) *(data.string_data) = "";
      else *(data.string_data) = (char*)value;
      break;
    case VAR_BOOL:
      if(value == NULL) data.bool_data = false;
      else data.bool_data = *((bool*)value);
      break;
    case VAR_INT:
      if(value == NULL) data.int_data = 0;
      else data.int_data = *((int*)value);
      break;
    case VAR_FLOAT:
      if(value == NULL) data.float_data = 0;
      else data.float_data = *((float*)value);
      break;
  }
}


/*********************************************************************
VAR_param eval()
  returns the value as true or false
*********************************************************************/
bool VAR_param::eval()
{
  switch(type)
  {
    case VAR_STRING:
      if(data.string_data == NULL) return false;
      if(*(data.string_data)->c_str()) return true;
      break;
    case VAR_BOOL:
      return data.bool_data;
    case VAR_INT:
      return (bool)data.int_data;
    case VAR_FLOAT:
      return (bool)data.float_data;
  }
  return false;
}



/*********************************************************************
VAR_param toString()
  converts the value to a string
*********************************************************************/
char* VAR_param::toString(char *buffer, int num)
{
  switch(type)
  {
    case VAR_STRING:
      if(data.string_data == NULL) return NULL;
      strncpy(buffer, data.string_data->c_str(), num-1);
      buffer[num-1] = '\0';
    break;
    case VAR_BOOL:
      if(data.bool_data) strcpy(buffer, "true");
      else strcpy(buffer, "false");
      break;
    case VAR_INT:
      snprintf(buffer, num, "%i", data.int_data);
      break;
    case VAR_FLOAT:
      snprintf(buffer, num, "%.2f", data.float_data);
      break;
  }
  return buffer;
}


/*********************************************************************
VAR_param toInt()
  converts the value to a string
*********************************************************************/
int VAR_param::toInt()
{
  switch(type)
  {
    case VAR_STRING:
      return 0;
    case VAR_BOOL:
      if(data.bool_data) return 1;
      else return 0;
    case VAR_INT:
      return data.int_data;
    case VAR_FLOAT:
      return (int)data.float_data;
  }
  return 0;
}


/*********************************************************************
VAR_paramList constructor
  initializes the structure as empty
*********************************************************************/
VAR_paramList::VAR_paramList()
{
  head = NULL;
}


/*********************************************************************
VAR_paramList destructor
  steps through all nodes, freeing the used memory
*********************************************************************/
VAR_paramList::~VAR_paramList()
{
  /*local variables*/
  VAR_param *search, *temp;

  search = head;
  while(search != NULL)
  {
    temp = search;
    search = search->next;
    delete temp;
  }
  head = NULL;
}


/*********************************************************************
VAR_paramList set(const char *param,
                  const char *datatype,
                  const void *value);
  returns the VAR_param that was added (or alternately the one that
  was already attached to that paramname) on successful addition of
  the parameter or NULL on invalid type specification or other error
*********************************************************************/
VAR_param* VAR_paramList::set(const char *param,
                              const char *datatype,
                              const void *value)
{
  /*local variables*/
  VAR_param *node, *search;
  VAR_datatype type;
  std::string typestring;

  typestring = datatype;
  /*convert typestring to type enum*/
  if(typestring == "string") type = VAR_STRING;
  else
    if(typestring == "bool") type = VAR_BOOL;
    else
      if(typestring == "int") type = VAR_INT;
      else
        if(typestring == "float") type = VAR_FLOAT;
        else
        {
          fprintf(stderr, "DEBUG: invalid type specification: %s\n",
                  typestring.c_str());
          return NULL;
        }

  search = head;

  /*not an empty list?*/
  if(head != NULL)
  {
    /*find node in front of insertion position or previous instance*/
    while((search->next != NULL) && (search->next->key <= param))
    {
      search = search->next;
    }
    /*already in list?*/
    if(search->key == param)
    {
      search->setData(type, value);
      return search;
    }
  }

  /*create new node*/
  node = new VAR_param(param);
  node->setData(type, value);

  /*insert at front?*/
  if((head == NULL) || (search == head && search->key > param))
  {
    node->next = search;
    head = node;
  } else
    /*insert at end?*/
    if(search->next == NULL)
    {
      search->next = node;
      node->next   = NULL;
    } else
      /*insert in middle*/
      {
        node->next   = search->next;
        search->next = node;
      }

  /*return the added node*/
  return node;
}


/*********************************************************************
VAR_paramList get(const char *param);
  returns the VAR_param that is attached to that paramname or NULL if
  the parameter has not been set.
*********************************************************************/
VAR_param* VAR_paramList::get(const char *param)
{
  /*local variables*/
  VAR_param *search;

  search = head;

  /*not an empty list?*/
  if(search != NULL)
  {
    /*find node*/
    while((search->key != param) && (search->next != NULL))
    {
      search = search->next;
    }
    /*in list?*/
    if(search->key == param) return search;
  }
  /*not in list*/
  return NULL;
}


/*********************************************************************
VAR_paramList perist();
  marks the parameter as persistent (adding it if it doesn't exist)
  returns the VAR_param that was added (or alternately the one that
  was already attached to that paramname)
*********************************************************************/
VAR_param* VAR_paramList::persist(const char *param)
{
  /*local variables*/
  VAR_param *node, *search;

  search = head;

  /*not an empty list?*/
  if(head != NULL)
  {
    /*find node in front of insertion position or previous instance*/
    while((search->next != NULL) && (search->next->key <= param))
    {
      search = search->next;
    }
    /*already in list?*/
    if(search->key == param)
    {
      search->persistent = true;
      return search;
    }
  }

  /*create new node*/
  node = new VAR_param(param);
  node->persistent = true;

  /*insert at front?*/
  if((head == NULL) || (search == head && search->key > param))
  {
    node->next = search;
    head = node;
  } else
    /*insert at end?*/
    if(search->next == NULL)
    {
      search->next = node;
      node->next   = NULL;
    } else
      /*insert in middle*/
      {
        node->next   = search->next;
        search->next = node;
      }

  /*return the added node*/
  return node;
}


/*********************************************************************
VAR_paramList eval(const char *param);
  returns true or false for the specified parameter based on the
  existence and/or value of the parameter
*********************************************************************/
bool VAR_paramList::eval(const char *param)
{
  /*local variables*/
  VAR_param *search;

  search = head;

  /*not an empty list?*/
  if(search != NULL)
  {
    /*find node*/
    while((search->key != param) && (search->next != NULL))
    {
      search = search->next;
    }
    /*in list?, then returns its evaluated value*/
    if(search->key == param) return search->eval();
  }
  /*not in list, so it is false*/
  return false;
}


/*********************************************************************
VAR_paramList toString(const char *param, char *buffer, int num);
  copies the string representation of the variable to the buffer (at
  most num bytes in length) and returns a pointer to the buffer or
  NULL if the variable is not in the list
*********************************************************************/
char* VAR_paramList::toString(const char *param, char *buffer, int num)
{
  /*local variables*/
  VAR_param *search;

  search = head;

  /*not an empty list?*/
  if(search != NULL)
  {
    /*find node*/
    while((search->key != param) && (search->next != NULL))
    {
      search = search->next;
    }
    /*in list?, then returns its evaluated value*/
    if(search->key == param) return search->toString(buffer, num);
  }
  /*not in list, so it is false*/
  return NULL;
}


/*********************************************************************
VAR_paramList toInt(const char *param);
  returns the integer representation of the variable, (0 if it has not
  been set)
*********************************************************************/
int VAR_paramList::toInt(const char *param)
{
  /*local variables*/
  VAR_param *search;

  search = head;

  /*not an empty list?*/
  if(search != NULL)
  {
    /*find node*/
    while((search->key != param) && (search->next != NULL))
    {
      search = search->next;
    }
    /*in list?, then returns its evaluated value*/
    if(search->key == param) return search->toInt();
  }
  /*not in list, so it is 0*/
  return 0;
}


/*********************************************************************
VAR_paramList flush()
  removes all non-persistent values from list
*********************************************************************/
void VAR_paramList::flush()
{
  /*local variables*/
  VAR_param *search, *prev, *temp;

  search = head;
  prev = NULL;
  while(search != NULL)
  {
    /*if it is a keeper*/
    if(search->persistent)
    {
      /*if the isn't the first node, set the previous node's next pointer*/
      if(prev != NULL) prev->next = search;
      /*else set the head pointer to point here*/
      else head = search;
      /*set this as the previous preserved node*/
      prev = search;
      search = search->next;
      continue;
    }
    /*delete this param*/
    temp = search;
    search = search->next;
    delete temp;
  }
  /*make sure list is NULL terminated*/
  if(prev != NULL) prev->next = NULL;
  else head = NULL;
}


/*********************************************************************
VAR_paramList clear()
  removes all values from list
*********************************************************************/
void VAR_paramList::clear()
{
  /*local variables*/
  VAR_param *search, *temp;

  search = head;
  while(search != NULL)
  {
    temp = search;
    search = search->next;
    delete temp;
  }
  head = NULL;
}


/*********************************************************************
VAR_paramList debug()
  prints out debugging information on all nodes
*********************************************************************/
void VAR_paramList::debug()
{
  /*local variables*/
  VAR_param *search;
  int nodes;

  printf("\nparamList: DEBUG\n");

  search = head;
  nodes  = 0;
  while(search != NULL)
  {
    printf("  key: %s\n", search->key.c_str());
    switch(search->type)
    {
      case VAR_STRING:
        printf("value: %s\n", search->data.string_data->c_str());
      break;
      case VAR_BOOL:
        if(search->data.bool_data) printf("value: true\n");
        else printf("value: false\n");
        break;
      case VAR_INT:
        printf("value: %i\n", search->data.int_data);
        break;
      case VAR_FLOAT:
        printf("value: %f\n", search->data.float_data);
        break;
    }
    search = search->next;
    nodes++;
  }

  printf("paramList: %i nodes total\n\n", nodes);
}

/*EOF*/
