/**
 * @file
 *
 * Traversal that iterates over the rule table and converts every raw rule
 * (parsed directly) to a rule using patterns
 */

#include "ccn/dynamic_core.h"
#include "ccngen/ast.h"
#include "frontend/rules/rules.h"
#include "frontend/symboltable.h"
#include "globals.h"
#include "palm/ctinfo.h"
#include "palm/hash_table.h"
#include "palm/str.h"
#include "stdbool.h"
#include <assert.h>
#include <regex.h>
#include <stddef.h>
#include <string.h>

static node_st *ste = NULL;   // symbol table
static node_st *first = NULL; // first rule table entry
static node_st *last = NULL;  // last rule table entry
static node_st *type = NULL;  // node corresponding to the return type of the

node_st *CRRast(node_st *node) {
    // Find the first and last rules
    first = AST_RTABLE(node);
    last = AST_RTABLE(node);
    ste = AST_STABLE(node);
    while (last && RTE_NEXT(last))
        last = RTE_NEXT(last);

    node_st *test;

    // This actually works!
    node_st *rule = CCNshorthand("RULE<>");
    test = CCNshorthand("RTE<RULE<>>;"
                        "RTE<%n>;"
                        "RTE<RULE<>>;",
                        rule);

    while (test) {
        if (NODE_TYPE(test) == NT_RTE)
            printf("OK\n");
        else
            printf("FAIL\n");

        if (NODE_TYPE(RTE_RULE(test)) == NT_RULE)
            printf("OK\n");
        else
            printf("FAIL\n");

        test = RTE_NEXT(test);
    }

    // Keep iterating over the table and rewrite every raw string to a
    // pattern
    TRAVopt(first);

    return node;
}

node_st *CRRrte(node_st *node) {
    type = STlookup(ste, RTE_TYPE(node));

    TRAVopt(RTE_RULE(node));
    TRAVopt(RTE_NEXT(node));
    return node;
}

node_st *CRRraw_rule(node_st *node) {
    // Available information: output type, pattern result
    // - obtain fields (DONE)
    // - link type (from output type fields)
    node_st *pattern = ASTpattern();
    node_st *result = ASTpattern();

    node_st *fields = parse_fields(RAW_RULE_PATTERN(node));
    node_st *curr = fields;

    while (curr) {
        // Lookup type of a matching child/attribute and link it to the field
        node_st *child = INODE_ICHILDREN(type);
        while (child) {
            if (strcasecmp(ID_LWR(CHILD_NAME(child)), FIELD_NAME(curr)) == 0) {
                FIELD_NODE_TYPE(curr) = CHILD_TYPE_REFERENCE(child);
                FIELD_IS_ATTRIBUTE(curr) = false;
                break;
            }
            child = CHILD_NEXT(child);
        }

        node_st *attribute = INODE_IATTRIBUTES(type);
        while (attribute && !child) {
            if (strcasecmp(ID_LWR(ATTRIBUTE_NAME(attribute)),
                           FIELD_NAME(curr)) == 0) {
                FIELD_ATTR_TYPE(curr) = ATTRIBUTE_TYPE(attribute);
                FIELD_IS_ATTRIBUTE(curr) = true;
                break;
            }
            attribute = ATTRIBUTE_NEXT(attribute);
        }

        curr = FIELD_NEXT(curr);
    }

    PATTERN_TEMPLATE(pattern) = RAW_RULE_PATTERN(node);
    PATTERN_FIELDS(pattern) = fields;
    PATTERN_TEMPLATE(result) = RAW_RULE_RESULT(node);

    node_st *rule = ASTrule(pattern);
    RULE_RESULT(rule) = result;
    RULE_TYPE(rule) = RAW_RULE_TYPE(node);

    *node = *rule;
    return rule;
}
