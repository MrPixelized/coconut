#include "assert.h"
#include <globals.h>
#include <stddef.h>
#include <stdio.h>

#include "ccn/dynamic_core.h"
#include "frontend/symboltable.h"
#include "gen_helpers/out_macros.h"
#include "palm/ctinfo.h"
#include "palm/str.h"

GeneratorContext *ctx;

static int rule_count = 0;
node_st *rte;
node_st *field;
node_st *type;
node_st *id;
node_st *st;

node_st *DGSHTast(node_st *node) {
    ctx = globals.gen_ctx;
    st = AST_STABLE(node);

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
            OUT_FIELD("node_st *n");
        }
        OUT_STRUCT_END();

        OUT_FIELD("struct shorthand_arg *next");
    }
    OUT_STRUCT_END();

    // Write a function to take a rule and its parameters and output the
    // node
    OUT_START_FUNC("node_st *SHrunrule(int r, struct shorthand_arg *args)");
    {
        // Expose "node" to the user
        OUT("node_st *node = NULL;\n");

        OUT_BEGIN_SWITCH("r");
        {
            rte = AST_RTABLE(node);
            while (rte) {
                OUT_BEGIN_CASE("%i", rule_count++);
                {
                    OUT("{\n");
                    GNindentIncrease(ctx);

                    field = PATTERN_FIELDS(RULE_PATTERN(RTE_RULE(rte)));

                    while (field) {
                        if (!FIELD_IS_ATTRIBUTE(field)) {
                            OUT("node_st *%s = args->n;\n", FIELD_NAME(field));

                            // Check if the given node matches the required type
                            type = STlookup(st, FIELD_NODE_TYPE(field));

                            if (NODE_TYPE(type) == NT_INODE)
                                OUT("if (NODE_TYPE(%s) != NT_%s) return "
                                    "NULL;\n",
                                    FIELD_NAME(field),
                                    ID_UPR(FIELD_NODE_TYPE(field)));
                            else if (NODE_TYPE(type) == NT_INODESET &&
                                     INODESET_UNPACKED(type)) {
                                id = INODESET_UNPACKED(type);

                                OUT("if (");
                                while (id) {
                                    OUT_NO_INDENT("NODE_TYPE(%s) != NT_%s",
                                                  FIELD_NAME(field),
                                                  ID_UPR(id));
                                    if (ID_NEXT(id))
                                        OUT_NO_INDENT(" &&");
                                    id = ID_NEXT(id);
                                }
                                OUT(") return NULL;\n");
                            }

                        } else
                            switch (FIELD_ATTR_TYPE(field)) {
                            case AT_int:
                                OUT("int %s = args->i;\n", FIELD_NAME(field));
                                break;
                            case AT_float:
                            case AT_double:
                                OUT("float %s = args->f;\n", FIELD_NAME(field));
                                break;
                            case AT_bool:
                                OUT("bool %s = args->b;\n", FIELD_NAME(field));
                                break;
                            case AT_string:
                                OUT("char *%s = args->s;\n", FIELD_NAME(field));
                                break;
                            default:
                                continue;
                            }
                        OUT("args = args->next;\n");

                        field = FIELD_NEXT(field);
                    }

                    // Insert user code and return exposed "node" variable
                    OUT("%s;\n", PATTERN_TEMPLATE(RULE_RESULT(RTE_RULE(rte))));
                    OUT("return node;\n");

                    GNindentDecrease(ctx);
                    OUT("}\n");
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
