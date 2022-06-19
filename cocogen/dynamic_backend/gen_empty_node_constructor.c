#include "assert.h"
#include <stddef.h>
#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "dynamic_backend/gen_helpers.h"
#include "gen_helpers/out_macros.h"
#include "globals.h"
#include "palm/ctinfo.h"
#include "palm/str.h"

static int num_children;

node_st *DGENCinode(node_st *node) {
    GeneratorContext *ctx = globals.gen_ctx;
    num_children = 0;
    OUT_START_FUNC("node_st *NEW%s()", ID_LWR(INODE_NAME(node)));
    {
        OUT_FIELD("node_st *node = NewNode()");
        OUT_FIELD("node->data.N_%s = MEMmalloc(sizeof(struct NODE_DATA_%s))",
                  ID_LWR(INODE_NAME(node)), ID_UPR(INODE_NAME(node)));
        OUT_FIELD("NODE_TYPE(node) = NT_%s", ID_UPR(INODE_NAME(node)));

        TRAVopt(INODE_ICHILDREN(node));

        OUT_FIELD("NODE_NUMCHILDREN(node) = %ld", num_children);
        if (num_children) {
            char *name_lwr = ID_LWR(INODE_NAME(node));
            OUT_FIELD("NODE_CHILDREN(node) = "
                      "node->data.N_%s->%s_children.%s_children_at",
                      name_lwr, name_lwr, name_lwr);
        }
        OUT_FIELD("return node");
    }
    OUT_END_FUNC();

    TRAVopt(INODE_NEXT(node));
    return node;
}

node_st *DGENCchild(node_st *node) {
    num_children++;
    TRAVchildren(node);
    return node;
}
