#include "assert.h"
#include <globals.h>
#include <stddef.h>
#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "gen_helpers/out_macros.h"
#include "palm/ctinfo.h"
#include "palm/str.h"

GeneratorContext *ctx;

node_st *DGSHTast(node_st *node) {
    ctx = globals.gen_ctx;
    GNopenIncludeFile(ctx, "shorthand.h");

    // Include necessary files
    OUT("#include <regex.h>\n\n");

    // Generate the regular expression used for the recursive parsing function
    OUT("static regex_t regex_sh;\n");
    OUT("const char *expression =\n");
    GNindentIncrease(ctx);
    {
        if (AST_RTABLE(node))
            TRAVdo(AST_RTABLE(node));
        else
            OUT("\"\";\n");
    }
    GNindentDecrease(ctx);
    OUT("\n");

    OUT_START_FUNC("void __attribute__ ((constructor)) regex_sh_compile()");
    { OUT("regcomp(&regex_sh, expression, REG_EXTENDED | REG_NEWLINE);\n"); }
    OUT_END_FUNC();

    return node;
}

node_st *DGSHTrte(node_st *node) {
    // Add the regular expression as a group
    OUT("\"(%s)\"", PATTERN_TEMPLATE(RULE_PATTERN(RTE_RULE(node))));

    // Terminate it with "next regular expression" or "end"
    if (RTE_NEXT(node))
        OUT_NO_INDENT(" \"|\"");
    else
        OUT_NO_INDENT(";");

    OUT("\n");

    // Go to the next rule in the table
    TRAVopt(RTE_NEXT(node));

    return node;
}
