#pragma once

#include "ccngen/ast.h"

node_st *parse_fields(char *pattern);
node_st *associate_type_info(node_st *fields, node_st *rettype);
