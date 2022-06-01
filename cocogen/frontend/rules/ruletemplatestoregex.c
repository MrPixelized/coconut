/**
 * @file
 *
 * Traversal that iterates over the rule table and reduces every rule
 * template to a format as would be used in the language itself
 */

#define _GNU_SOURCE
#include "ccn/dynamic_core.h"
#include "ccngen/ast.h"
#include "frontend/symboltable.h"
#include "globals.h"
#include "palm/ctinfo.h"
#include "palm/hash_table.h"
#include "palm/str.h"
#include "stdbool.h"
#include <stddef.h>
#include <stdio.h>

node_st *RTTRrte(node_st *node) {
    TRAVopt(RTE_RULE(node));
    TRAVopt(RTE_NEXT(node));

    return node;
}

node_st *RTTRrule(node_st *node) {
    node_st *pattern = RULE_PATTERN(node);
    node_st *field = PATTERN_FIELDS(pattern);

    // Iterate over the fields of this rule
    while (field) {
        char *token;
        char *type;

        // Get their direct representation in the template
        asprintf(&token, "{%s}", FIELD_NAME(field));

        // Find the shorthand type specifier for this field
        if (!FIELD_IS_ATTRIBUTE(field))
            type = "%n";
        else
            switch (FIELD_ATTR_TYPE(field)) {
            case AT_int:
                type = "%i";
                break;
            case AT_float:
            case AT_double:
                type = "%f";
                break;
            case AT_bool:
                type = "%b";
                break;
            case AT_string:
                type = "%s";
                break;
            default:
                continue;
            }

        // Replace the rule syntax ({field}) with the type specifier
        PATTERN_TEMPLATE(pattern) =
            STRsubstToken(PATTERN_TEMPLATE(pattern), token, type);

        field = FIELD_NEXT(field);
    }

    return node;
}
