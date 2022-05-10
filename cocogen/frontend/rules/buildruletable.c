/**
 * @file
 *
 * Traversal that builds a table of various rules for the sub-DSL
 * to create ASTs
 */

#include "ccn/dynamic_core.h"
#include "ccngen/ast.h"
#include "globals.h"
#include "palm/ctinfo.h"
#include "palm/hash_table.h"
#include "palm/str.h"
#include <stddef.h>

static node_st *last_rte = NULL;
static node_st *first_rte = NULL;
static node_st *curr_node = NULL;

node_st *BRTinode(node_st *node) {
    curr_node = node;

    TRAVchildren(node);
    return node;
}

node_st *BRTast(node_st *node) {
    first_rte = ASTrte();
    AST_RTABLE(node) = first_rte;
    last_rte = first_rte;

    TRAVchildren(node);
    return node;
}

node_st *BRTrule(node_st *node) {
    RTE_RULE(last_rte) = node;

    if (RULE_NEXT(node)) {
        RTE_NEXT(last_rte) = ASTrte();
        last_rte = RTE_NEXT(last_rte);
        TRAVchildren(node);
        RULE_NEXT(node) = NULL;
    }

    return node;
}

node_st *BRTrte(node_st *node) { return node; }
