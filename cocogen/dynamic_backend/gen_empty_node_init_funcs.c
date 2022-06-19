#include "assert.h"
#include <stddef.h>
#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "dynamic_backend/gen_helpers.h"
#include "gen_helpers/out_macros.h"
#include "globals.h"
#include "palm/ctinfo.h"
#include "palm/str.h"

node_st *DGEIFinode(node_st *node) {
    GeneratorContext *ctx = globals.gen_ctx;
    OUT_FIELD("node_st *NEW%s()", ID_LWR(INODE_NAME(node)));

    TRAVopt(INODE_NEXT(node));
    return node;
}
