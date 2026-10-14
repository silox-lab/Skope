#ifndef JSON
#define JSON
#include "errors.h"

char *nodata_result_to_json(Result r);
char *wiki_arr_to_json(Result wiki_arr_result);
char *scope_arr_to_json(Result scope_arr_result);
char *scopefile_arr_to_json(Result scopefile_arr_result);
char *globalentity_arr_to_json(Result globalentity_arr_result);
char *relation_arr_to_json(Result relation_arr_result);

#endif