/*
 HLH - base layer - json parser

 Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

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
   HLH_string name;
   unsigned type:3;
   unsigned count:29;
   union
   {
      HLH_json5_dyn_array array;
      HLH_json5_dyn_array nodes;
      int64_t integer;
      double real;
      HLH_string string;
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
void            HLH_json_object_add_string(HLH_json5 *j, HLH_string name, HLH_string value); // Provided string needs at least same lifetime as json
void            HLH_json_object_add_real(HLH_json5 *j, HLH_string name, double value);
void            HLH_json_object_add_integer(HLH_json5 *j, HLH_string name, int64_t value);
void            HLH_json_object_add_boolean(HLH_json5 *j, HLH_string name, int value);
void            HLH_json_object_add_object(HLH_json5 *j, HLH_string name, HLH_json5 o);
void            HLH_json_object_add_array(HLH_json5 *j, HLH_string name, HLH_json5 a);
void            HLH_json_array_add_string(HLH_json5 *a, HLH_string value); // Provided string needs at least same lifetime as json
void            HLH_json_array_add_real(HLH_json5 *a, double value);
void            HLH_json_array_add_integer(HLH_json5 *a, int64_t value);
void            HLH_json_array_add_boolean(HLH_json5 *a, int value);
void            HLH_json_array_add_object(HLH_json5 *a, HLH_json5 o);
void            HLH_json_array_add_array(HLH_json5 *a, HLH_json5 ar);

HLH_json5      *HLH_json_get_object(HLH_json5 *json, HLH_string name);
HLH_json5      *HLH_json_get_array_item(HLH_json5 *json, int index);
int             HLH_json_get_array_size(const HLH_json5 *json);

//Save access methods
//Returns the value of the variable if it exists
//If the variable does not exist or is of a different type,
//the function will return the fallback value
//If you request an integer/real and the variable is of type real/integer
//the value will be converted
HLH_string      HLH_json_get_object_string(HLH_json5 *json, HLH_string name, HLH_string fallback); // String has same lifetime as json / fallback
double          HLH_json_get_object_real(HLH_json5 *json, HLH_string name, double fallback);
int64_t         HLH_json_get_object_integer(HLH_json5 *json, HLH_string name, int64_t fallback);
int             HLH_json_get_object_boolean(HLH_json5 *json, HLH_string name, int fallback);
HLH_json5      *HLH_json_get_object_object(HLH_json5 *json, HLH_string name, HLH_json5 *fallback);
HLH_json5      *HLH_json_get_object_array(HLH_json5 *json, HLH_string name, HLH_json5 *fallback);
HLH_string      HLH_json_get_array_string(HLH_json5 *json, int index, HLH_string); // String has same lifetime as json
double          HLH_json_get_array_real(HLH_json5 *json, int index, double fallback);
int64_t         HLH_json_get_array_integer(HLH_json5 *json, int index, int64_t fallback);
int             HLH_json_get_array_boolean(HLH_json5 *json, int index, int fallback);
HLH_json5      *HLH_json_get_array_object(HLH_json5 *json, int index, HLH_json5 *fallback);

#endif
