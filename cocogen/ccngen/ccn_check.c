#include <stdbool.h>
#include "ccn/dynamic_core.h"
#include "ccn/phase_driver.h"
#include "ccngen/ast.h"
#include "palm/ctinfo.h"
static bool TypeIssetexpr(node_st *arg_node) {
    enum ccn_nodetype node_type = NODE_TYPE(arg_node);
    return (false || node_type == NT_SETOPERATION || node_type == NT_SETLITERAL || node_type == NT_SETREFERENCE    );
}

struct ccn_node *CHKid(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (ID_NEXT(arg_node)) {
        if (NODE_TYPE(ID_NEXT(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child next of node id has disallowed type %d ", NODE_TYPE(ID_NEXT(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKienum(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (IENUM_VALS(arg_node)) {
        if (NODE_TYPE(IENUM_VALS(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child vals of node ienum has disallowed type %d ", NODE_TYPE(IENUM_VALS(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKattribute(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (ATTRIBUTE_NAME(arg_node)) {
        if (NODE_TYPE(ATTRIBUTE_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node attribute has disallowed type %d ", NODE_TYPE(ATTRIBUTE_NAME(arg_node)));
        }

    }

    if (ATTRIBUTE_NAME(arg_node) == NULL) {
        CTIerror("Child(name) in node(attribute) is missing, but specified as mandatory.\n");;
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKitravdata(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (ITRAVDATA_NAME(arg_node)) {
        if (NODE_TYPE(ITRAVDATA_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node itravdata has disallowed type %d ", NODE_TYPE(ITRAVDATA_NAME(arg_node)));
        }

    }

    if (ITRAVDATA_NAME(arg_node) == NULL) {
        CTIerror("Child(name) in node(itravdata) is missing, but specified as mandatory.\n");;
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKsetoperation(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (SETOPERATION_LEFT(arg_node)) {
        if (!TypeIssetexpr(SETOPERATION_LEFT(arg_node))) {
            CTIerror("Inconsistent node found in AST. Child left of node setoperation has disallowed type %d ", NODE_TYPE(SETOPERATION_LEFT(arg_node)));
        }

    }

    if (SETOPERATION_LEFT(arg_node) == NULL) {
        CTIerror("Child(left) in node(setoperation) is missing, but specified as mandatory.\n");;
    }

    if (action_id >= 8 && true) {
        CTIerror("Found disallowed node(setoperation) in tree.\n");
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKsetliteral(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (SETLITERAL_REFERENCE(arg_node)) {
        if (NODE_TYPE(SETLITERAL_REFERENCE(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child reference of node setliteral has disallowed type %d ", NODE_TYPE(SETLITERAL_REFERENCE(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKsetreference(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (SETREFERENCE_REFERENCE(arg_node)) {
        if (NODE_TYPE(SETREFERENCE_REFERENCE(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child reference of node setreference has disallowed type %d ", NODE_TYPE(SETREFERENCE_REFERENCE(arg_node)));
        }

    }

    if (action_id >= 8 || false) {
        CTIerror("Found disallowed Found disallowed node(setreference) in tree. in tree.\n");
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKste(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (STE_NEXT(arg_node)) {
        if (NODE_TYPE(STE_NEXT(arg_node)) != NT_STE) {
            CTIerror("Inconsistent node found in AST. Child next of node ste has disallowed type %d ", NODE_TYPE(STE_NEXT(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKchild(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (CHILD_NAME(arg_node)) {
        if (NODE_TYPE(CHILD_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node child has disallowed type %d ", NODE_TYPE(CHILD_NAME(arg_node)));
        }

    }

    if (CHILD_NAME(arg_node) == NULL) {
        CTIerror("Child(name) in node(child) is missing, but specified as mandatory.\n");;
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKlifetime_range(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (LIFETIME_RANGE_TARGET(arg_node)) {
        if (NODE_TYPE(LIFETIME_RANGE_TARGET(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child target of node lifetime_range has disallowed type %d ", NODE_TYPE(LIFETIME_RANGE_TARGET(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKilifetime(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (ILIFETIME_BEGIN(arg_node)) {
        if (NODE_TYPE(ILIFETIME_BEGIN(arg_node)) != NT_LIFETIME_RANGE) {
            CTIerror("Inconsistent node found in AST. Child begin of node ilifetime has disallowed type %d ", NODE_TYPE(ILIFETIME_BEGIN(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKinodeset(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (INODESET_NAME(arg_node)) {
        if (NODE_TYPE(INODESET_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node inodeset has disallowed type %d ", NODE_TYPE(INODESET_NAME(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKinode(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (INODE_NAME(arg_node)) {
        if (NODE_TYPE(INODE_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node inode has disallowed type %d ", NODE_TYPE(INODE_NAME(arg_node)));
        }

    }

    if (INODE_NAME(arg_node) == NULL) {
        CTIerror("Child(name) in node(inode) is missing, but specified as mandatory.\n");;
    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKipass(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (IPASS_NAME(arg_node)) {
        if (NODE_TYPE(IPASS_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node ipass has disallowed type %d ", NODE_TYPE(IPASS_NAME(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKitraversal(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (ITRAVERSAL_NAME(arg_node)) {
        if (NODE_TYPE(ITRAVERSAL_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node itraversal has disallowed type %d ", NODE_TYPE(ITRAVERSAL_NAME(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKiphase(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (IPHASE_NAME(arg_node)) {
        if (NODE_TYPE(IPHASE_NAME(arg_node)) != NT_ID) {
            CTIerror("Inconsistent node found in AST. Child name of node iphase has disallowed type %d ", NODE_TYPE(IPHASE_NAME(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKiactions(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (IACTIONS_NEXT(arg_node)) {
        if (NODE_TYPE(IACTIONS_NEXT(arg_node)) != NT_IACTIONS) {
            CTIerror("Inconsistent node found in AST. Child next of node iactions has disallowed type %d ", NODE_TYPE(IACTIONS_NEXT(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

struct ccn_node *CHKast(struct ccn_node *arg_node) {
    size_t action_id = CCNgetCurrentActionId();
    action_id = action_id;
    if (AST_IPHASES(arg_node)) {
        if (NODE_TYPE(AST_IPHASES(arg_node)) != NT_IPHASE) {
            CTIerror("Inconsistent node found in AST. Child iphases of node ast has disallowed type %d ", NODE_TYPE(AST_IPHASES(arg_node)));
        }

    }

    TRAVchildren(arg_node);
    return arg_node;
}

