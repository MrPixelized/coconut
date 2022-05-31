/**
 * @file
 *
 * Traversal that iterates over the rule table and reduces every rule
 * to a map rule
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

static node_st *ste = NULL;   // symbol table
static node_st *first = NULL; // first rule table entry
static node_st *last = NULL;  // last rule table entry
static node_st *rule = NULL;  // rule of the current rule table entry
static node_st *type = NULL;  // node corresponding to the return type of the
                              // current rule table entry

bool rtable_is_map(node_st *rte) {
    while (rte && RTE_NEXT(rte)) {
        if (RULE_TYPE(RTE_RULE(rte)) != RT_map)
            return false;
        rte = RTE_NEXT(rte);
    }

    return true;
}

node_st *IRTast(node_st *node) {
    // Find the first and last rules
    first = AST_RTABLE(node);
    last = AST_RTABLE(node);
    ste = AST_STABLE(node);
    while (last && RTE_NEXT(last))
        last = RTE_NEXT(last);

    // Keep iterating over the table until all rules are of type "map"
    while (!rtable_is_map(first))
        TRAVopt(first);

    return node;
}

node_st *IRTrte(node_st *node) {
    rule = RTE_RULE(node);
    type = STlookup(ste, RTE_TYPE(node));

    TRAVchildren(node);
    return node;
}

node_st *IRTrule(node_st *node) {
    char *result;
    node_st *field;

    switch (RULE_TYPE(node)) {
    case RT_template:
        // If this is a template rule, remap the placeholders to an actual
        // C expression -> a map rule
        field = PATTERN_FIELDS(RULE_PATTERN(node));

        // TODO: make ordering in constructor correct
        asprintf(&result, "AST%s(", ID_LWR(INODE_NAME(type)));

        while (field) {
            asprintf(&result, "%s%s", result, FIELD_NAME(field));
            if (FIELD_NEXT(field))
                asprintf(&result, "%s, ", result);

            field = FIELD_NEXT(field);
        }

        asprintf(&result, "%s)", result);

        // Assign the generated C code to the node
        PATTERN_TEMPLATE(RULE_RESULT(node)) = result;
        RULE_TYPE(node) = RT_map;
        return node;
    case RT_rewrite:
        // TODO
        // If this is a rewrite rule, match for the rule pattern in the
        // current rule table (first) and rewrite the 'result' to match
        RULE_TYPE(node) = RT_map;
        return node;
    default:
        // If this is a map rule, the rule is already finished and we can
        // move on
        return node;
    }
}
