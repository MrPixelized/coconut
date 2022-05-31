#include "assert.h"
#include <globals.h>
#include <stddef.h>
#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "gen_helpers/out_macros.h"
#include "palm/ctinfo.h"
#include "palm/str.h"

GeneratorContext *ctx;

static int rule_count = 0;
node_st *rte;

node_st *DGSHTast(node_st *node) {
    ctx = globals.gen_ctx;
    GNopenIncludeFile(ctx, "shorthand.h");

    // Include necessary files
    OUT("#include <regex.h>\n\n");

    // Create a struct for dynamic parameters (also used by copra)
    OUT_STRUCT("shorthand_arg");
    {
        OUT_UNION("");
        {
            OUT_FIELD("int i");
            OUT_FIELD("double f");
            OUT_FIELD("bool b");
            OUT_FIELD("char *s");
        }
        OUT_STRUCT_END();

        OUT_FIELD("struct shorthand_arg *next");
    }
    OUT_STRUCT_END();

    // Write a function to take a rule and its parameters and output the
    // node
    OUT_START_FUNC("node_st *SHrunrule(int r, struct shorthand_arg *args)");
    {
        OUT_BEGIN_SWITCH("r");
        {
            rte = AST_RTABLE(node);
            while (rte) {
                OUT_BEGIN_CASE("%i", rule_count++);
                {
                    OUT("printf(\"%s\\n\");\n",
                        PATTERN_TEMPLATE(RULE_RESULT(RTE_RULE(rte))));
                }
                OUT_END_CASE();

                rte = RTE_NEXT(rte);
            }
        }
        OUT_END_SWITCH();
        OUT("return NULL;\n");
    }
    OUT_END_FUNC();

    // Generate the regular expression used for the recursive parsing
    // function
    OUT("const char *expressions[%i] = {\n", rule_count);
    GNindentIncrease(ctx);
    {
        rte = AST_RTABLE(node);
        while (rte) {
            // Add the rule to the list of regular expressions
            OUT("\"%s\",\n", PATTERN_TEMPLATE(RULE_PATTERN(RTE_RULE(rte))));
            rte = RTE_NEXT(rte);
        }
    }
    GNindentDecrease(ctx);
    OUT("};\n\n");

    // Save the amount of groups in the regex
    OUT("const int rules_sh = %i;\n", rule_count);
    OUT("static regex_t regex_sh[%i];\n", rule_count);
    OUT("\n");

    // Compile the regular expression before main()
    OUT_START_FUNC("void __attribute__ ((constructor)) regex_sh_compile()");
    {
        OUT("for (int i = 0; i < rules_sh; i++)\n");
        OUT("\tregcomp(&regex_sh[i], expressions[i], REG_EXTENDED | "
            "REG_NEWLINE);\n");
    }
    OUT_END_FUNC();

    return node;
}
