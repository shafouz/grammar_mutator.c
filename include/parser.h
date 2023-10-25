#ifndef PARSER_H
#define PARSER_H

#include "tree.h"
#include <cjson/cJSON.h>

Node *json_to_tree(cJSON *json);

#endif
