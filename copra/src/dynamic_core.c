#include <err.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "ccn/ccn_types.h"
#include "ccn/dynamic_core.h"

#include "ccngen/ast.h"
#include "ccngen/shorthand.h"

#include "palm/memory.h"
#include "palm/str.h"

static ccn_trav_st *current_traversal;

void TRAVdataNOP(ccn_trav_st *trav) {
    trav = (void *)trav;
    return;
}

void TRAVpush(enum ccn_traversal_type trav_type) {
    ccn_trav_st *trav = MEMmalloc(sizeof(ccn_trav_st));
    trav->trav_type = trav_type;
    trav->prev = current_traversal;
    current_traversal = trav;

    ccn_trav_data_ft init_func = trav_data_init_vtable[trav_type];
    init_func(trav);
}

void TRAVpop() {
    if (current_traversal == NULL) {
        err(EXIT_FAILURE, "[coconut] Error in framework.");
    }
    ccn_trav_st *prev = current_traversal->prev;

    ccn_trav_data_ft free_func =
        trav_data_free_vtable[current_traversal->trav_type];
    free_func(current_traversal);

    MEMfree(current_traversal);
    current_traversal = prev;
}

ccn_trav_st *TRAVgetCurrent(void) { return current_traversal; }

/**
 * Traverse the supplied node.
 * This function requires that arg_node != NULL
 * */
struct ccn_node *TRAVdo(struct ccn_node *arg_node) {
    // assert(arg_node != NULL);
    ccn_trav_ft trav_func =
        ccn_trav_vtable[current_traversal->trav_type][NODE_TYPE(arg_node)];
    return trav_func(arg_node);
}

/* Start new traversal and push it to the traversal stack */
struct ccn_node *TRAVstart(struct ccn_node *syntaxtree,
                           enum ccn_traversal_type trav_type) {
    TRAVpush(trav_type);
    syntaxtree = TRAVopt(syntaxtree);
    TRAVpop();
    return syntaxtree;
}

/** Optional traverse function.
 * Adds a null check to signify optional child nodes
 */
struct ccn_node *TRAVopt(struct ccn_node *arg_node) {
    if (arg_node != NULL) {
        return TRAVdo(arg_node);
    }
    return arg_node;
}

/* Don't traverse through children and return */
struct ccn_node *TRAVnop(struct ccn_node *arg_node) {
    return arg_node;
}

/**
 * Helper function that traverses all children in a node.
 * This function assigns the result of the traversals to it respective child.
 * The order is determined by the order in the specification file.
 */
struct ccn_node *TRAVchildren(struct ccn_node *arg_node) {
    for (int i = 0; i < NODE_NUMCHILDREN(arg_node); i++) {
        NODE_CHILDREN(arg_node)[i] = TRAVopt(NODE_CHILDREN(arg_node)[i]);
    }
    return arg_node;
}

struct ccn_node *TRAVerror(struct ccn_node *arg_node) {
    //"Trying to traverse through node of unknown type.");
    fprintf(stderr, "Traversing trough unkown node, is the node corrupted?");
    abort();
    return arg_node;
}

struct ccn_node *PASSstart(struct ccn_node *syntaxtree,
                           enum ccn_pass_type pass_type) {
    return ccn_pass_vtable[pass_type](syntaxtree);
}

struct ccn_node *PASSerror(struct ccn_node *arg_node) {
    (void)arg_node;
    abort();
}

/**
 * Makes a deep copy of the given node.
 */
struct ccn_node *CCNcopy(struct ccn_node *arg_node) {
    return TRAVstart(arg_node, TRAV_cpy);
}

/**
 * Free a node, all its children, and allocated attributes.
 */
struct ccn_node *CCNfree(struct ccn_node *arg_node) {
    return TRAVstart(arg_node, TRAV_free);
}

/**
 * Returns the index of the letter in the first `%n, %f, ...` type of
 * construction, 0 if there are no such data placeholders;
 * the letter will always be char 1 or greater
 */
int CCNpatternnext(char *pattern) {
    char c;

    if (!pattern)
        return 0;

    int len = STRlen(pattern);

    for (int i = 0; i < len; i++) {
        c = pattern[i];

        if (c == '\\')
            i += 1;
        else if (c == '%' && i < len - 1)
            return i + 1;
    }

    return 0;
}

struct ccn_node *CCNparsepattern(char *pattern, struct shorthand_arg *args) {
    struct shorthand_arg *arg = args;    // Copy of args to not lose head
    struct shorthand_arg *rrargs = NULL; // Args for single node generator
    struct shorthand_arg *larg = NULL;   // Tail of regular args
    regmatch_t match;                    // Regex match of the rule
    node_st *new;                        // New node generated for the rule
    int r;                               // Rule number
    int i = 0;                           // Index of placeholder in pattern
    int offset = 0;                      // Index of placeholder irt previous
    char *remainder = pattern;           // Pattern from index i onward

    /*       rrargs
     *         V
     * a %n b %n c %n d %n e
     *       |         |         "%n c %n d" is matched, those two args are
     *       |         |         represented by rrargs
     * a %n b    %n     %n e
     *    ^
     *  larg                     larg is the last arg before the removed
     args,
     *                           in the new shorthand_arg chain with the
     newly
     *                           inserted single %n
     */

    // Loop over all generated regex rules until one is found in the pattern
    for (r = 0; r < rules_sh; r++) {
        if (regexec(&regex_sh[r], pattern, 1, &match, 0))
            continue;

        // Find the rrargs for this match
        i = 0;
        offset = 0;
        remainder = pattern;
        arg = args;
        while ((offset = CCNpatternnext(remainder))) {
            remainder += offset;
            i += offset;

            if (i >= match.rm_so && i < match.rm_eo) {
                rrargs = arg;
                break;
            }

            arg = arg->next;
        }

        // Attempt to get the generated node from runrule
        new = SHrunrule(r, rrargs);

        // If this rule cannot be used with these arguments, try the next one
        if (new)
            break;
    }

    // If no new rules can be applied, return the generated node
    if (r == rules_sh) {
        if (!args)
            return NULL;
        return args->n;
    }

    // Loop over the placeholders in the pattern, removing shadowed placeholders
    // from the argument list and storing them for a call to SHrunrule
    i = 0;
    offset = 0;
    remainder = pattern;
    arg = args;
    while ((offset = CCNpatternnext(remainder))) {
        remainder += offset;
        i += offset;

        if (i < match.rm_so) {
            // Store the last argument before the rrargs
            larg = arg;
        } else if (i < match.rm_eo) {
            // Remove the placeholder in rrargs
            if (larg)
                larg->next = arg->next;
        } else {
            break;
        }

        arg = arg->next;
    }

    // Print some status ifo
    printf("Pattern: %s -> Rule %i: [%u - %u]: %s\n", pattern, r, match.rm_so,
           match.rm_eo,
           STRsubStr(pattern, match.rm_so, match.rm_eo - match.rm_so));

    // Add the newly generated node into the argument list
    struct shorthand_arg *new_node = MEMmalloc(sizeof(struct shorthand_arg));
    new_node->n = new;

    if (larg) {
        new_node->next = larg->next;
        larg->next = new_node;
    } else {
        new_node->next = arg;
        args = new_node;
    }
    // Replace the matched part of the language with %n
    pattern =
        STRsubstSlice(pattern, match.rm_so, match.rm_eo - match.rm_so, "%n");

    return CCNparsepattern(pattern, args);
}

struct ccn_node *CCNshorthand(char *pattern, ...) {
    struct shorthand_arg *head = NULL; // Typed argument list to pass
    struct shorthand_arg *node;        // Current argument to add to the list

    // Initialize the variable argument list
    va_list args;
    va_start(args, pattern);

    // Find consecutive indices of pattern placeholders
    int offset = 0;
    char *remainder = pattern;
    while ((offset = CCNpatternnext(remainder))) {
        remainder = remainder + offset;

        if (!head) {
            head = MEMmalloc(sizeof(struct shorthand_arg));
            node = head;
        } else {
            node->next = MEMmalloc(sizeof(struct shorthand_arg));
            node = node->next;
        }

        // Give it the value of the next va_arg, correctly typed
        switch (*remainder) {
        case 'i':
            node->i = va_arg(args, int);
            break;
        case 'f':
            node->f = va_arg(args, double);
            break;
        case 'b':
            node->b = va_arg(args, int);
            break;
        case 's':
            node->s = va_arg(args, char *);
            break;
        case 'n':
            node->n = va_arg(args, node_st *);
            break;
        }
    }

    // Free the variable argument list
    va_end(args);

    // Use the list of values to parse the pattern
    node_st *res = CCNparsepattern(pattern, head);
    return res;
}
