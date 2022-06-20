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
#include <stdbool.h>
#include <stddef.h>

static node_st *last_rte = NULL;
static node_st *first_rte = NULL;
static node_st *rule;
static bool done = false;

node_st *BRTinode(node_st *node) {
    // Do stuff to the rule of this node if it still has any left
    if (rule = INODE_IRULES(node)) {
        // Add an extra RTE if needed
        if (RTE_TYPE(last_rte)) {
            RTE_NEXT(last_rte) = ASTrte();
            last_rte = RTE_NEXT(last_rte);
        }

        done = false;

        INODE_IRULES(node) = RAW_RULE_NEXT(rule);
        RTE_RULE(last_rte) = CCNcopy(rule);
        RTE_TYPE(last_rte) = CCNcopy(INODE_NAME(node));
    }

    TRAVopt(INODE_NEXT(node));
    return node;
}

node_st *BRTast(node_st *node) {
    first_rte = ASTrte();
    AST_RTABLE(node) = first_rte;
    last_rte = first_rte;

    while (!done) {
        done = true;
        TRAVchildren(node);
    }

    if (!RTE_TYPE(AST_RTABLE(node)))
        AST_RTABLE(node) = NULL;

    return node;
}
