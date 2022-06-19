/**
 * This exposes some helper functions to work with rules and patterns.
 */
#include <regex.h>
#include <stddef.h>

#include "ccngen/ast.h"
#include "palm/dbug.h"
#include "palm/str.h"

node_st *parse_fields(char *pattern) {
    char *cursor;
    node_st *curr;
    node_st *fields;
    regex_t regex;
    regmatch_t groups[2];
    int i = 0;

    // Compile the regular expression - return NULL on failure
    if (regcomp(&regex, "\\{([A-Za-z0-9_]+)\\}", REG_EXTENDED | REG_NEWLINE))
        return NULL;

    // Set up the fields to store matches in
    fields = ASTfield();
    curr = fields;

    // Iterate over substrings of pattern in cursor
    cursor = pattern;

    // Attempt to find the next match in a loop; break if none is found
    while (regexec(&regex, cursor, 2, groups, 0) == 0) {
        // Advance to the next node in the chain
        if (FIELD_NEXT(curr))
            curr = FIELD_NEXT(curr);

        // Set data
        FIELD_NAME(curr) = STRsubStr(cursor, groups[1].rm_so,
                                     groups[1].rm_eo - groups[1].rm_so);
        FIELD_INDEX(curr) = i++;
        FIELD_NEXT(curr) = ASTfield();

        // Advance the search to the potential next match
        cursor += groups[0].rm_eo;
    }

    // Clean up the compiled regular expression
    regfree(&regex);

    // Make sure the last NEXT in the chain is NULL, and return NULL
    // if no matches were found
    if (FIELD_NEXT(fields) == NULL)
        fields = NULL;
    else
        FIELD_NEXT(curr) = NULL;

    return fields;
}
