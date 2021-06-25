/*
JSON parser

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//Based on tinyjson5 by r-yleh (https://github.com/r-lyeh/tinybits/blob/master/tinyjson5.c)

#ifndef _HLH_JSON_H_
	
#define _HLH_JSON_H_

typedef enum HLH_json5_type 
{
   HLH_json5_undefined,
   HLH_json5_null,
   HLH_json5_bool,
   HLH_json5_object,
   HLH_json5_string,
   HLH_json5_array,
   HLH_json5_integer,
   HLH_json5_real,
}HLH_json5_type;

typedef struct HLH_json5 HLH_json5;

typedef struct 
{
   uint32_t used;
   uint32_t size;
   HLH_json5 *data;
}HLH_json5_dyn_array;

typedef struct HLH_json5
{
   char *name;
   unsigned type:3;
   unsigned count:29;
   union 
   {
      HLH_json5_dyn_array array;
      HLH_json5_dyn_array nodes;
      int64_t integer;
      double real;
      char *string;
      int boolean;
   };
}HLH_json5;

typedef struct
{
   char *data;
   size_t data_size;
   HLH_json5 root;
}HLH_json5_root;

HLH_json5_root *HLH_json_parse_file(const char *path);
HLH_json5_root *HLH_json_parse_file_stream(FILE *f);
HLH_json5_root *HLH_json_parse_char_buffer(const char *buffer, size_t size); //buffer must be allocated by user
void            HLH_json_write_file(FILE *f, HLH_json5 *j);
void            HLH_json_free(HLH_json5_root *r);

//JSON file creation
//ALL char * MUST be persistend until HLH_json_write_file function call
HLH_json5_root *HLH_json_create_root();
HLH_json5       HLH_json_create_object();
HLH_json5       HLH_json_create_array();
void            HLH_json_object_add_string(HLH_json5 *j, char *name, char *value);
void            HLH_json_object_add_real(HLH_json5 *j, char *name, double value);
void            HLH_json_object_add_integer(HLH_json5 *j, char *name, int64_t value);
void            HLH_json_object_add_boolean(HLH_json5 *j, char *name, int value);
void            HLH_json_object_add_object(HLH_json5 *j, char *name, HLH_json5 o); 
void            HLH_json_object_add_array(HLH_json5 *j, char *name, HLH_json5 a); 
void            HLH_json_array_add_string(HLH_json5 *a, char *value);
void            HLH_json_array_add_real(HLH_json5 *a, double value);
void            HLH_json_array_add_integer(HLH_json5 *a, int64_t value);
void            HLH_json_array_add_boolean(HLH_json5 *a, int value);
void            HLH_json_array_add_object(HLH_json5 *a, HLH_json5 o); 
void            HLH_json_array_add_array(HLH_json5 *a, HLH_json5 ar); 

HLH_json5      *HLH_json_get_object(HLH_json5 *json, const char *name);
HLH_json5      *HLH_json_get_array_item(HLH_json5 *json, int index);
int             HLH_json_get_array_size(const HLH_json5 *json);

//Save access methods
//Returns the value of the variable if it exists
//If the variable does not exist or is of a different type, 
//the function will return the fallback value
//If you request an integer/real and the variable is of type real/integer
//the value will be converted
char           *HLH_json_get_object_string(HLH_json5 *json, const char *name, char *fallback);
double          HLH_json_get_object_real(HLH_json5 *json, const char *name, double fallback);
int64_t         HLH_json_get_object_integer(HLH_json5 *json, const char *name, int64_t fallback);
int             HLH_json_get_object_boolean(HLH_json5 *json, const char *name, int fallback);
HLH_json5      *HLH_json_get_object_object(HLH_json5 *json, const char *name, HLH_json5 *fallback);
char           *HLH_json_get_array_string(HLH_json5 *json, int index, char *fallback);
double          HLH_json_get_array_real(HLH_json5 *json, int index, double fallback);
int64_t         HLH_json_get_array_integer(HLH_json5 *json, int index, int64_t fallback);
int             HLH_json_get_array_boolean(HLH_json5 *json, int index, int fallback);
HLH_json5      *HLH_json_get_array_object(HLH_json5 *json, int index, HLH_json5 *fallback);

 #endif

#ifdef HLH_JSON_IMPLEMENTATION
#ifndef HLH_JSON_IMPLEMENTATION_ONCE
#define HLH_JSON_IMPLEMENTATION_ONCE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#ifndef HLH_JSON_MALLOC
#define HLH_JSON_MALLOC malloc
#endif

#ifndef HLH_JSON_FREE
#define HLH_JSON_FREE free
#endif

#ifndef HLH_JSON_REALLOC
#define HLH_JSON_REALLOC realloc
#endif

#ifndef JSON5_ASSERT
#define JSON5_ASSERT do { printf("JSON5: Error L%d while parsing '%c' in '%.16s'\n", __LINE__, p[0], p); assert(0); } while(0)
#endif

static void json5_push(HLH_json5_dyn_array *array, HLH_json5 ob);
static void json5_array_free(HLH_json5_dyn_array *array);
static char *json5_parse(HLH_json5 *root, char *source, int flags);
static void json5_write(FILE *f, const HLH_json5 *o,int indent);
static void json5_free(HLH_json5 *root);
static char *json5__parse_value(HLH_json5 *obj, char *p, char **err_code);
static char *json5__trim(char *p);
static char *json5__parse_string(HLH_json5 *obj, char *p, char **err_code);
static char *json5__parse_object(HLH_json5 *obj, char *p, char **err_code);

HLH_json5_root *HLH_json_parse_file(const char *path)
{
   //Load data from file
   HLH_json5_root *r = HLH_JSON_MALLOC(sizeof(*r));
   memset(r,0,sizeof(*r));
   FILE *f = fopen(path,"rb");
   fseek(f,0,SEEK_END);
   r->data_size = ftell(f);
   fseek(f,0,SEEK_SET);
   r->data = HLH_JSON_MALLOC(r->data_size+1);
   fread(r->data,r->data_size,1,f);
   r->data[r->data_size] = '\0';
   fclose(f);

   //Parsing
   json5_parse(&r->root,r->data,0);

   return r;
}

HLH_json5_root *HLH_json_parse_file_stream(FILE *f)
{
   //Load data from file
   HLH_json5_root *r = HLH_JSON_MALLOC(sizeof(*r));
   memset(r,0,sizeof(*r));
   fseek(f,0,SEEK_END);
   r->data_size = ftell(f);
   rewind(f);
   r->data = HLH_JSON_MALLOC(sizeof(*r->data)*(r->data_size+1));
   fread(r->data,r->data_size,1,f);
   r->data[r->data_size] = '\0';

   //Parsing
   json5_parse(&r->root,r->data,0);

   return r;
}

HLH_json5_root *HLH_json_parse_char_buffer(const char *buffer, size_t size)
{
   HLH_json5_root *r = HLH_JSON_MALLOC(sizeof(*r));
   memset(r,0,sizeof(*r));
   r->data = HLH_JSON_MALLOC(size+1);
   memcpy(r->data,buffer,size);
   r->data[size] = '\0';
   r->data_size = size;

   //Parsing
   json5_parse(&r->root,r->data,0);

   return r;
}

void HLH_json_write_file(FILE *f, HLH_json5 *j)
{
   json5_write(f,j,0);
}

void HLH_json_free(HLH_json5_root *r)
{
   HLH_JSON_FREE(r->data);
   json5_free(&r->root);
   HLH_JSON_FREE(r);
}

HLH_json5_root *HLH_json_create_root()
{
   HLH_json5_root *r = HLH_JSON_MALLOC(sizeof(*r));
   r->data = NULL;
   r->data_size = 0;
   r->root.name = NULL;
   r->root.type = HLH_json5_object;
   r->root.count = 0;
   r->root.nodes.data = NULL;

   return r;
}

HLH_json5 HLH_json_create_object()
{
   HLH_json5 j = {0}; 
   j.type = HLH_json5_object;
   j.count = 0;
   j.nodes.data = NULL;

   return j;
}

HLH_json5 HLH_json_create_array()
{
   HLH_json5 j = {0}; 
   j.type = HLH_json5_array;
   j.count = 0;
   j.array.data = NULL;

   return j;
}

void HLH_json_object_add_string(HLH_json5 *j, char *name, char *value)
{
   HLH_json5 node = {0};
   node.name = name;
   node.type = HLH_json5_string;
   node.string = value;

   json5_push(&j->nodes,node);
   ++j->count;
}

void HLH_json_object_add_real(HLH_json5 *j, char *name, double value)
{
   HLH_json5 node = {0};
   node.name = name;
   node.type = HLH_json5_real;
   node.real = value;

   json5_push(&j->nodes,node);
   ++j->count;
}

void HLH_json_object_add_integer(HLH_json5 *j, char *name, int64_t value)
{
   HLH_json5 node = {0};
   node.name = name;
   node.type = HLH_json5_integer;
   node.integer = value;

   json5_push(&j->nodes,node);
   ++j->count;
}

void HLH_json_object_add_boolean(HLH_json5 *j, char *name, int value)
{
   HLH_json5 node = {0};
   node.name = name;
   node.type = HLH_json5_bool;
   node.boolean = value;

   json5_push(&j->nodes,node);
   ++j->count;
}

void HLH_json_object_add_object(HLH_json5 *j, char *name, HLH_json5 o)
{
   o.name = name;
   json5_push(&j->nodes,o);
   ++j->count;
}

void HLH_json_object_add_array(HLH_json5 *j, char *name, HLH_json5 a)
{
   a.name = name;
   json5_push(&j->nodes,a);
   ++j->count;
}

void HLH_json_array_add_string(HLH_json5 *a, char *value)
{
   HLH_json5 node = {0};
   node.name = NULL;
   node.type = HLH_json5_string;
   node.string = value;

   json5_push(&a->array,node);
   ++a->count;
}

void HLH_json_array_add_real(HLH_json5 *a, double value)
{
   HLH_json5 node = {0};
   node.name = NULL;
   node.type = HLH_json5_real;
   node.real = value;

   json5_push(&a->array,node);
   ++a->count;
}

void HLH_json_array_add_integer(HLH_json5 *a, int64_t value)
{
   HLH_json5 node = {0};
   node.name = NULL;
   node.type = HLH_json5_integer;
   node.integer = value;

   json5_push(&a->array,node);
   ++a->count;
}

void HLH_json_array_add_boolean(HLH_json5 *a, int value)
{
   HLH_json5 node = {0};
   node.name = NULL;
   node.type = HLH_json5_bool;
   node.boolean = value;

   json5_push(&a->array,node);
   ++a->count;
}

void HLH_json_array_add_object(HLH_json5 *a, HLH_json5 o)
{
   o.name = NULL;
   json5_push(&a->array,o);
   ++a->count;
}

void HLH_json_array_add_array(HLH_json5 *a, HLH_json5 ar)
{
   ar.name = NULL;
   json5_push(&a->array,ar);
   ++a->count;
}


HLH_json5 *HLH_json_get_object(HLH_json5 *json, const char *name)
{
   //Not an object
   if(json->type!=HLH_json5_object)
      return NULL;

   for(int i = 0;i<json->count;i++)
   {
      if(strcmp(name,json->nodes.data[i].name)==0)
         return &json->nodes.data[i];
   }

   //Not found
   return NULL;
}

int HLH_json_get_array_size(const HLH_json5 *json)
{
   if(json->type!=HLH_json5_array)
      return -1;

   return json->count;
}

HLH_json5 *HLH_json_get_array_item(HLH_json5 *json, int index)
{
   //Not an array
   if(json->type!=HLH_json5_array)
      return NULL;

   //Out of bounds
   if(index<0||index>=json->count)
      return NULL;

   return &json->array.data[index];
}

char *HLH_json_get_object_string(HLH_json5 *json, const char *name, char *fallback)
{
   if(!json||json->type!=HLH_json5_object)
      return fallback;

   HLH_json5 *o = HLH_json_get_object(json,name);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_string)
      return o->string;

   return fallback;
}

double HLH_json_get_object_real(HLH_json5 *json, const char *name, double fallback)
{
   if(!json||json->type!=HLH_json5_object)
      return fallback;

   HLH_json5 *o = HLH_json_get_object(json,name);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_real)
      return o->real;

   if(o->type==HLH_json5_integer)
      return (double)o->integer;

   return fallback;
}

int64_t HLH_json_get_object_integer(HLH_json5 *json, const char *name, int64_t fallback)
{
   if(!json||json->type!=HLH_json5_object)
      return fallback;

   HLH_json5 *o = HLH_json_get_object(json,name);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_integer)
      return o->integer;

   if(o->type==HLH_json5_real)
      return (int64_t)o->real;

   return fallback;
}

int HLH_json_get_object_boolean(HLH_json5 *json, const char *name, int fallback)
{
   if(!json||json->type!=HLH_json5_object)
      return fallback;

   HLH_json5 *o = HLH_json_get_object(json,name);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_bool)
      return o->boolean;

   return fallback;
}

HLH_json5 *HLH_json_get_object_object(HLH_json5 *json, const char *name, HLH_json5 *fallback)
{
   if(!json||json->type!=HLH_json5_object)
      return fallback;

   HLH_json5 *o = HLH_json_get_object(json,name);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_object)
      return o;

   return fallback;
}

char *HLH_json_get_array_string(HLH_json5 *json, int index, char *fallback)
{
   if(!json||json->type!=HLH_json5_array)
      return fallback;

   HLH_json5 *o = HLH_json_get_array_item(json,index);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_string)
      return o->string;

   return fallback;
}

double HLH_json_get_array_real(HLH_json5 *json, int index, double fallback)
{
   if(!json||json->type!=HLH_json5_array)
      return fallback;

   HLH_json5 *o = HLH_json_get_array_item(json,index);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_real)
      return o->real;

   if(o->type==HLH_json5_integer)
      return (double)o->integer;

   return fallback;
}

int64_t HLH_json_get_array_integer(HLH_json5 *json, int index, int64_t fallback)
{
   if(!json||json->type!=HLH_json5_array)
      return fallback;

   HLH_json5 *o = HLH_json_get_array_item(json,index);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_integer)
      return o->integer;

   if(o->type==HLH_json5_real)
      return (int64_t)o->real;

   return fallback;
}

int HLH_json_get_array_boolean(HLH_json5 *json, int index, int fallback)
{
   if(!json||json->type!=HLH_json5_array)
      return fallback;

   HLH_json5 *o = HLH_json_get_array_item(json,index);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_bool)
      return o->boolean;

   return fallback;
}

HLH_json5 *HLH_json_get_array_object(HLH_json5 *json, int index, HLH_json5 *fallback)
{
   if(!json||json->type!=HLH_json5_array)
      return fallback;

   HLH_json5 *o = HLH_json_get_array_item(json,index);
   if(!o)
      return fallback;

   if(o->type==HLH_json5_object)
      return o;

   return fallback;
}


//tinyjson5 code:

// JSON5 + SJSON parser module
//
// License:
// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.
// No warranty is implied, use at your own risk.
//
// Credits:
// Dominik Madarasz (original code) (GitHub: zaklaus)
// r-lyeh (fork)

// vector library -------------------------------------------------------------

static void json5_push(HLH_json5_dyn_array *array, HLH_json5 ob)
{
   if(array->data==NULL)
   {
      array->size = 16;
      array->used = 0;
      array->data = HLH_JSON_MALLOC(sizeof(ob)*array->size);
   }
   
   array->data[array->used++] = ob;
   if(array->used==array->size)
   {
      array->size+=16;
      array->data = HLH_JSON_REALLOC(array->data,sizeof(ob)*array->size);
   }
}

static void json5_array_free(HLH_json5_dyn_array *array)
{
   HLH_JSON_FREE(array->data);
   array->data = NULL;
   array->size = 0;
   array->used = 0;
}

// json5 ----------------------------------------------------------------------
static char *json5__trim(char *p)
{
   while (*p) 
   {
      if(isspace(*p)) 
      {
         ++p;
      }
      else if(p[0]=='/'&&p[1]=='*') 
      { 
         //skip C comment
         for(p+=2;*p&&!(p[0]=='*'&&p[1]=='/');++p);
         if(*p) 
            p+=2;
      }
      else if(p[0]=='/'&&p[1]=='/') 
      { 
         //skip C++ comment
         for(p+=2;*p&&p[0]!='\n';++p);
         if( *p ) 
            ++p;
      }
      else 
      {
         break;
      }
   }

   return p;
}

static char *json5__parse_string(HLH_json5 *obj, char *p, char **err_code)
{
   assert(obj&&p);

   if(*p=='"'||*p=='\''||*p=='`') 
   {
      obj->type = HLH_json5_string;
      obj->string = p+1;

      char eos_char = *p;
      char *b = obj->string;
      char *e = b;
      while (*e) 
      {
         if(*e=='\\'&&(e[1]==eos_char)) 
            ++e;
         else if(*e=='\\'&&(e[1]=='\r'||e[1]=='\n')) 
            *e = ' ';
         else if(*e==eos_char) 
            break;
         ++e;
      }

      *e = '\0';
      return p = e+1;
   }

   //JSON5_ASSERT; *err_code = "json5_error_invalid_value";
   return NULL;
}

static char *json5__parse_object(HLH_json5 *obj, char *p, char **err_code)
{
   assert(obj&&p);

   if(1) /* <-- for SJSON */
   { 
      int skip = *p=='{'; /* <-- for SJSON */
      obj->type = HLH_json5_object;
      obj->nodes.data = NULL;
      obj->nodes.size = 0;
      obj->nodes.used = 0;

      while(*p)
      {
         HLH_json5 node = {0};
         do 
         { 
            p = json5__trim(p+skip);
            skip = 1; 
         }
         while(*p ==',');

         if(*p =='}') 
         {
            ++p;
            break;
         }
         // @todo: is_unicode() (s[0] == '\\' && isxdigit(s[1]) && isxdigit(s[2]) && isxdigit(s[3]) && isxdigit(s[4]))) {
         else if(isalpha(*p)||*p=='_'||*p=='$') 
         { 
             // also || is_unicode(p)
            node.name = p;

            do 
            {
               ++p;
            } 
            while(*p&&(*p=='_'||isalpha(*p)||isdigit(*p))); // also || is_unicode(p)

            char *e = p;
            p = json5__trim(p);
            *e = '\0';
         }
         else 
         { 
            //if( *p == '"' || *p == '\'' || *p == '`' ) {
            char *ps = json5__parse_string(&node, p, err_code);
            if(!ps) 
            {
               return NULL;
            }
            p = ps;
            node.name = node.string;
            p = json5__trim(p);
         }

         // @todo: https://www.ecma-international.org/ecma-262/5.1/#sec-7.6
         if(!(node.name && node.name[0])) 
         { 
            // !json5__validate_name(node.name) ) {
            JSON5_ASSERT; *err_code = "json5_error_invalid_name";
            return NULL;
         }

         if(!p||(*p&&(*p!=':'&&*p!='='/*<-- for SJSON */))) 
         {
            JSON5_ASSERT; *err_code = "json5_error_invalid_name";
            return NULL;
         }
         p = json5__trim(p + 1);
         p = json5__parse_value(&node, p, err_code);

         if(*err_code[0]) 
         {
            return NULL;
         }

         if(node.type!=HLH_json5_undefined) 
         {
            json5_push(&obj->nodes, node);
            ++obj->count;
         }

         if(*p =='}') 
         { 
            ++p; 
            break; 
         }
      }
      return p;
   }

   JSON5_ASSERT; *err_code = "json5_error_invalid_value";
   return NULL;
}

static char *json5__parse_value(HLH_json5 *obj, char *p, char **err_code) 
{
   assert(obj&&p);

   p = json5__trim(p);

   char *is_string = json5__parse_string(obj,p,err_code);

   if(is_string) 
   {
      p = is_string;
      if(*err_code[0])
      {
         return NULL;
      }
   }
   else if(*p=='{') 
   {
      p = json5__parse_object(obj,p,err_code);
      if(*err_code[0]) 
      {
         return NULL;
      }
   }
   else if(*p=='[')
   {
      obj->type = HLH_json5_array;
      obj->array.data = NULL;

      while (*p) 
      {
         HLH_json5 elem = {0};
         elem.array.data = NULL;

         do
         { 
            p = json5__trim(p+1); 
         } 
         while(*p==',');

         if(*p ==']') 
         { 
            ++p; 
            break; 
         }

         p = json5__parse_value(&elem,p,err_code);

         if(*err_code[0]) 
         {
            return NULL;
         }

         if(elem.type!=HLH_json5_undefined) 
         {
            json5_push(&obj->array, elem);
            ++obj->count;
         }
         if(*p==']') 
         { 
            ++p; 
            break; 
         }
      }
   }
   else if(isalpha(*p)||(*p=='-'&&!isdigit(p[1]))) 
   {
      const char *labels[] = { "null", "on","true", "off","false", "nan","NaN", "-nan","-NaN", "inf","Infinity", "-inf","-Infinity" };
      const int lenghts[] = { 4, 2,4, 3,5, 3,3, 4,4, 3,8, 4,9 };
      for(int i = 0;labels[i];++i)
      {
         if(!strncmp(p,labels[i],lenghts[i])) 
         {
            p += lenghts[i];
#ifdef _MSC_VER // somehow, NaN is apparently signed in MSC
            if(i>=5) 
            {
               obj->type = json5_real;
               obj->real = i>=11?-INFINITY:i>=9?INFINITY:i>=7?NAN:-NAN;
            }
#else
            if(i>=5)
            {
               obj->type = HLH_json5_real;
               obj->real = i>=11?-INFINITY:i>=9?INFINITY:i>=7?-NAN:NAN;
            }
#endif
            else if(i>=1) 
            {
               obj->type = HLH_json5_bool;
               obj->boolean = i <= 2;
            }
            else
            {
               obj->type = HLH_json5_null;
            }
            break;
         }
      }
      if(obj->type==HLH_json5_undefined ) 
      {
         JSON5_ASSERT; *err_code = "json5_error_invalid_value";
         return NULL;
      }
   }
   else if(isdigit(*p)||*p=='+'||*p=='-'||*p=='.')
   {
      char buffer[16] = {0};
      char *buf = buffer;
      char is_hex = 0;
      char is_dbl = 0;
      while(*p&&strchr("+-.xX0123456789aAbBcCdDeEfF",*p)) 
      {
         is_hex |= (*p | 32) == 'x';
         is_dbl |= *p == '.';
         *buf++ = *p++;
      }
      obj->type = is_dbl?HLH_json5_real:HLH_json5_integer;
      if(is_dbl) 
         sscanf(buffer,"%lf",&obj->real);
      else if(is_hex) 
         sscanf(buffer,"%"PRIx64,&obj->integer); // SCNx64 -> inttypes.h
      else
         sscanf(buffer,"%" PRId64,&obj->integer); // SCNd64 -> inttypes.h
   }
   else 
   {
      return NULL;
   }

   return p;
}

static char *json5_parse(HLH_json5 *root, char *p, int flags) 
{
   assert(root&&p);

   char *err_code = "";
   *root = (HLH_json5) {0};

   p = json5__trim(p);
   if(*p=='[') 
   { /* <-- for SJSON */
      json5__parse_value(root, p, &err_code);
   } 
   else 
   {
      json5__parse_object(root,p,&err_code); /* <-- for SJSON */
   }

   return err_code[0] ? err_code : 0;
}

static void json5_free(HLH_json5 *root) 
{
   if(root->type==HLH_json5_array&&root->array.data!=NULL) 
   {
      for(int i = 0, cnt = root->array.used;i<cnt;i++) 
      {
         json5_free(&root->array.data[i]);
      }
      json5_array_free(&root->array);
   } 

   if(root->type==HLH_json5_object&&root->nodes.data!=NULL) 
   {
      for(int i = 0,cnt = root->nodes.used;i<cnt;i++) 
      {
         json5_free(&root->nodes.data[i]);
      }
      json5_array_free(&root->nodes);
   }

   *root = (HLH_json5) {0}; // needed?
}

static void json5_write(FILE *f, const HLH_json5 *o,int indent)
{
   static const char *tabs = 
   "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
   "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
   if(o->name)
   {
      fprintf(f,"%.*s\"%s\":",indent,tabs,o->name);
   }

   if(o->type==HLH_json5_null)
      fprintf(f,"%s","null");
   else if(o->type==HLH_json5_bool)
      fprintf(f,"%s",o->boolean?"true":"false");
   else if(o->type==HLH_json5_integer) 
      fprintf(f,"%" PRId64,o->integer);
   else if(o->type==HLH_json5_real) 
   {
      if(isnan(o->real)) 
         fprintf(f,"%s",signbit(o->real)?"-nan":"nan");
      else if(isinf(o->real)) 
         fprintf(f,"%s",signbit(o->real)?"-inf":"inf");
      else 
         fprintf(f,"%.4lf",o->real);
   }
   else if(o->type==HLH_json5_string) 
   {
      // write (escaped) string
      char chars[] = "\\\"\n\r\b\f\v";
      char remap[] = "\\\"nrbfv";
      char esc[256];
      for(int i = 0;chars[i];++i) 
         esc[(unsigned)chars[i]] = remap[i];

      const char *b = o->string;
      const char *e = strpbrk(b, chars);
      const char *sep = "\"";
      while(e) 
      {
         fprintf(f,"%s%.*s\\%c",sep,(int)(e-b),b,esc[(unsigned char)*e]);
         e = strpbrk( b = e + 1, chars);
         sep = "";
      }
      fprintf(f, "%s%s\"", sep, b);
   }
   else if(o->type==HLH_json5_array) 
   {
      const char *sep = "";
      fprintf(f, "%s", "[ ");
      for(int i = 0, cnt = o->count; i < cnt; ++i ) 
      {
         fprintf(f, "%s", sep); sep = ", ";
         json5_write(f, &o->array.data[i],indent+1);
      }
      fprintf(f, "%s", " ]");
   }
   else if(o->type==HLH_json5_object) 
   {
      const char *sep = "";
      fprintf(f, "%.*s{\n", 0 * (++indent), tabs);
      for( int i = 0, cnt = o->count; i < cnt; ++i ) 
      {
         fprintf(f, "%s", sep); sep = ",\n";
         json5_write(f, &o->nodes.data[i],indent+1);
      }
      fprintf(f, "\n%.*s}", --indent, tabs);
   } 
   else 
   {
      char p[16] = {0};
      JSON5_ASSERT; /* "json5_error_invalid_value"; */
   }
}

#endif
#endif
