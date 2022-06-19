#pragma once
#include <regex.h>

struct shorthand_arg {
    union  {
        int i;
        double f;
        bool b;
        char *s;
        node_st *n;
    };

    struct shorthand_arg *next;
};

node_st *SHrunrule(int r, struct shorthand_arg *args) {
    switch (r) {
        case 0:
            {
                node_st *node = NULL;
                node = NEWrte();
                return node;
            }
            break;
        case 1:
            {
                node_st *rule = args->n;
                args = args->next;
                node_st *node = NULL;
                node = NEWrte(); RTE_RULE(node) = rule;
                return node;
            }
            break;
        case 2:
            {
                node_st *rule = args->n;
                args = args->next;
                node_st *next = args->n;
                if (NODE_TYPE(next) != NT_RTE) return NULL;
                args = args->next;
                node_st *node = NULL;
                node = NEWrte(); RTE_RULE(node) = rule; RTE_NEXT(node) = next;
                return node;
            }
            break;
        case 3:
            {
                node_st *rule = args->n;
                args = args->next;
                node_st *next = args->n;
                if (NODE_TYPE(next) != NT_RTE) return NULL;
                args = args->next;
                node_st *node = NULL;
                node = rule; RTE_NEXT(node) = next;
                return node;
            }
            break;
        case 4:
            {
                node_st *node = NULL;
                node = ASTrule(ASTpattern());
                return node;
            }
            break;
    }

    return NULL;
}

const char *expressions[5] = {
    "RTE<>",
    "RTE<%n>",
    "RTE<%n, %n>",
    "%n; *%n;?$",
    "RULE<>",
};

const int rules_sh = 5;
static regex_t regex_sh[5];

void __attribute__ ((constructor)) regex_sh_compile() {
    for (int i = 0; i < rules_sh; i++)
    	regcomp(&regex_sh[i], expressions[i], REG_EXTENDED | REG_NEWLINE);
}

