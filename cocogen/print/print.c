#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "ccn/phase_driver.h"
#include "ccngen/ast.h"
#include "ccngen/trav.h"
#include "globals.h"

extern int **reachability_matrix;

static int opt_counter = 0;
static int indent = 0;

#define INDENT indent += 4
#define UNINDENT indent -= 4

static void PrintIndent() {
    for (int i = 0; i < indent; i++) {
        putchar(' ');
    }
}

node_st *doOpts(node_st *ast) {
    printf("Opt: %d\n", opt_counter);
    if (opt_counter < 50) {
        CCNcycleNotify();
    }
    opt_counter++;
    return ast;
}

node_st *PRTast(node_st *ast) {
    if (!globals.show_ast) {
        return ast;
    }
    TRAVchildren(ast);

    if (reachability_matrix) {
        printf("Reachability table:\n");
        for (int i = 0; i < AST_NUM_TRAVERSALS(ast); i++) {
            printf("%d: ", i);
            for (int j = 0; j < AST_NUM_NODES(ast); j++) {
                printf("%d, ", reachability_matrix[i][j]);
            }
            printf("\n");
        }
    }
    return ast;
}

node_st *PRTiphase(node_st *node) {
    printf("Phase %s {\n", ID_ORIG(IPHASE_NAME(node)));
    INDENT;
    PrintIndent();
    printf("Actions {\n");
    INDENT;
    TRAViactions(node);
    UNINDENT;
    PrintIndent();
    printf("}\n");
    UNINDENT;
    printf("}\n");
    TRAVnext(node);
    return node;
}

node_st *PRTitraversal(node_st *node) {
    printf("Trav %s {\n", ID_ORIG(ITRAVERSAL_NAME(node)));
    INDENT;
    PrintIndent();
    printf("Traversal index: %d\n", ITRAVERSAL_INDEX(node));
    PrintIndent();
    printf("nodes {\n");
    INDENT;
    TRAVinodes(node);
    UNINDENT;
    PrintIndent();
    printf("}\n");
    UNINDENT;
    printf("}\n");
    TRAVnext(node);
    return node;
}

node_st *PRTitravdata(node_st *node) {
    PrintIndent();
    printf("Travdata %s\n", ID_ORIG(ITRAVDATA_NAME(node)));
    return node;
}

node_st *PRTipass(node_st *node) {
    printf("Pass: %s\n", ID_ORIG(IPASS_NAME(node)));
    return node;
}

node_st *PRTiactions(node_st *node) {
    PrintIndent();
    printf("%s: %d\n", ID_ORIG(IACTIONS_REFERENCE(node)),
           IACTIONS_ACTION_ID(node));
    TRAVnext(node);
    return node;
}

node_st *PRTinode(node_st *node) {
    printf("NODE: %s(%d) {\n", ID_ORIG(INODE_NAME(node)), INODE_INDEX(node));
    INDENT;
    PrintIndent();
    printf("Children {\n");
    INDENT;
    TRAVopt(INODE_ICHILDREN(node));
    UNINDENT;
    PrintIndent();
    printf("}\n");
    PrintIndent();
    printf("Attributes {\n");
    INDENT;
    TRAVopt(INODE_IATTRIBUTES(node));
    UNINDENT;
    PrintIndent();
    printf("}\n");
    PrintIndent();
    printf("Rules {\n");
    INDENT;
    TRAVopt(INODE_IRULES(node));
    UNINDENT;
    PrintIndent();
    printf("}\n");
    UNINDENT;
    printf("}\n");
    TRAVopt(INODE_NEXT(node));
    return node;
}

node_st *PRTchild(node_st *node) {
    TRAVopt(CHILD_NAME(node));
    TRAVopt(CHILD_NEXT(node));
    return node;
}

node_st *PRTrte(node_st *node) {
    printf("RTE: %s\n", ID_ORIG(RTE_TYPE(node)));
    TRAVchildren(node);
    return node;
}

// TODO: condense duplicate code
node_st *PRTraw_rule(node_st *node) {
    printf(", %s", RAW_RULE_PATTERN(node));
    switch (RAW_RULE_TYPE(node)) {
    case RT_NULL:
    case RT_template:
        printf("\n");
        break;
    case RT_rewrite:
        printf(" -> ");
        break;
    case RT_map:
        printf(" = ");
        break;
    }
    if (RAW_RULE_RESULT(node) != NULL)
        printf("%s\n", RAW_RULE_PATTERN(node));
    TRAVopt(RAW_RULE_NEXT(node));
    return node;
}

node_st *PRTrule(node_st *node) {
    PrintIndent();
    INDENT;
    TRAVopt(RULE_PATTERN(node));
    PrintIndent();
    switch (RULE_TYPE(node)) {
    case RT_NULL:
    case RT_template:
        printf("\n");
        break;
    case RT_rewrite:
        printf("->\n");
        break;
    case RT_map:
        printf("=\n");
        break;
    }
    TRAVopt(RULE_RESULT(node));
    UNINDENT;
    return node;
}

// TODO: include type
node_st *PRTfield(node_st *node) {
    PrintIndent();
    if (FIELD_TYPE(node))
        printf("%i: %s: %s\n", FIELD_INDEX(node), FIELD_NAME(node),
               ID_ORIG(FIELD_TYPE(node)));
    else
        printf("%i: %s\n", FIELD_INDEX(node), FIELD_NAME(node));

    TRAVopt(FIELD_NEXT(node));
    return node;
}

node_st *PRTpattern(node_st *node) {
    PrintIndent();
    if (PATTERN_TEMPLATE(node))
        printf("%s\n", PATTERN_TEMPLATE(node));
    else
        printf("\n");
    INDENT;
    TRAVopt(PATTERN_FIELDS(node));
    UNINDENT;
    return node;
}

node_st *PRTattribute(node_st *node) {
    PrintIndent();
    printf("%s\n", ID_ORIG(ATTRIBUTE_NAME(node)));
    TRAVopt(ATTRIBUTE_NEXT(node));
    return node;
}

node_st *PRTsetreference(node_st *node) { return node; }

node_st *PRTsetliteral(node_st *node) {
    if (SETLITERAL_REFERENCE(node)) {
        PrintIndent();
        printf("%s\n", ID_ORIG(SETLITERAL_REFERENCE(node)));
    }
    TRAVopt(SETLITERAL_LEFT(node));
    TRAVopt(SETLITERAL_RIGHT(node));
    return node;
}

node_st *PRTinodeset(node_st *node) {
    printf("nodeset %s {\n", ID_ORIG(INODESET_NAME(node)));
    INDENT;
    TRAVopt(INODESET_EXPR(node));
    UNINDENT;
    PrintIndent();
    printf("}\n");
    TRAVopt(INODESET_NEXT(node));
    return node;
}

node_st *PRTid(node_st *node) {
    PrintIndent();
    printf("%s\n", ID_ORIG(node));
    return node;
}

node_st *PRTsetoperation(node_st *node) { return node; }

node_st *PRTienum(node_st *node) { return node; }

node_st *PRTste(node_st *node) {
    printf("STE: %s, %d\n", ID_ORIG(STE_KEY(node)), NODE_TYPE(STE_VALUE(node)));
    TRAVchildren(node);
    return node;
}

node_st *PRTilifetime(node_st *node) {
    if (ILIFETIME_TYPE(node) == LT_mandatory) {
        printf("mandatory");
    } else {
        printf("disallowed");
    }
    printf("(");
    TRAVopt(ILIFETIME_BEGIN(node));
    printf(" -> ");
    TRAVopt(ILIFETIME_END(node));
    printf(")\n");

    TRAVopt(ILIFETIME_NEXT(node));
    return node;
}

node_st *PRTlifetime_range(node_st *node) {
    printf("%d", LIFETIME_RANGE_ACTION_ID(node));
    return node;
}
